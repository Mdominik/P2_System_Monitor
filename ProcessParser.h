#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>

#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"
#include <string>

class ProcessParser{
private:
    public:
    static string getCmd(string pid);
    static vector<string> getPidList();
    static string getVmSize(string pid);
    static string getCpuPercent(string pid);
    static long int getSysUpTime();
    static std::string getProcUpTime(string pid);
    static string getProcUser(string pid);
    static vector<string> getSysCpuPercent(string coreNumber = "");
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getNumberOfCores();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static float getSysActiveCpuTime(vector<string> values);
    static float getSysIdleCpuTime(vector<string> values);
    static string getOSName();
    static std::string PrintCpuStats(vector<string> values1, vector<string>values2);
    static bool isPidExisting(string pid);
};

// TODO: Define all of the above functions below:

//command to run the process
string ProcessParser::getCmd(string pid) {
    std::ifstream stream = Util::getStream(Path::basePath() + pid + Path::cmdPath());
    string line;
    getline(stream, line);
    return line;
}


//list of all processes running
vector<string> ProcessParser::getPidList() {
    DIR* dir;
    vector<string> container;
    
    if(!(dir = opendir("/proc"))) {
        throw std::runtime_error(std::strerror(errno));
    }

    while(dirent* dirp = readdir(dir)) {
        //is this a dir?
        if(dirp->d_type != DT_DIR)
            continue;
        
        //is every character of the name a digit?
        if(all_of(dirp->d_name, dirp->d_name + std::strlen(dirp->d_name), [](char c){return std::isdigit(c);})){
            container.push_back(dirp->d_name);
        }
    }
    if(closedir(dir))
        throw std::runtime_error(std::strerror(errno));
    return container;
}

//virtual memory size
std::string ProcessParser::getVmSize(string pid) {
    std::string line;
    std::string name = "VmData";
    std::string value;
    float result;
    std::ifstream stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
    while(std::getline(stream, line)){
        // Searching line by line
        if (line.compare(0, name.size(),name) == 0) {
            // slicing string line on ws for values using sstream
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            //conversion kB -> GB
            result = (stof(values[1])/float(1024));
            break;
        }
    }
    return to_string(result);
}

//return the time of the current CPU taken by a process (in %)
std::string ProcessParser::getCpuPercent(string pid)
{
    string line;
    string value;
    float result;
    std::ifstream stream = Util::getStream((Path::basePath()+ pid + Path::statPath()));
    std::getline(stream, line);
    string str = line;
    istringstream buf(str);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end); // values is a vector of strings from the file
    // acquiring relevant times for calculation of active occupation of CPU for selected process
    float utime = stof(ProcessParser::getProcUpTime(pid)); //
    float stime = stof(values[14]);
    float cutime = stof(values[15]);
    float cstime = stof(values[16]);
    float starttime = stof(values[21]);
    float uptime = ProcessParser::getSysUpTime();
    float freq = sysconf(_SC_CLK_TCK);
    float total_time = utime + stime + cutime + cstime;
    float seconds = uptime - (starttime/freq);
    result = 100.0*((total_time/freq)/seconds);
    return to_string(result);
}

long int ProcessParser::getSysUpTime() {
    string line;
    std::ifstream stream = Util::getStream(Path::basePath() + Path::upTimePath());
    getline(stream,line);
    istringstream buf(line);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);
    return stoi(values[0]); //
}
std::string ProcessParser::getProcUpTime(string pid) {
    string line;
    std::ifstream stream = Util::getStream(Path::basePath()+pid+Path::statPath());
    getline(stream,line);
    return to_string(float(getSysUpTime())/sysconf(_SC_CLK_TCK));
}
string ProcessParser::getProcUser(string pid) {
    string name = "Uid:"; //owner of the process
    std::ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
    string line;
    string result = "";
    while(std::getline(stream,line)) {
        if(line.compare(0, name.size(), name) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            result = values[1];
            break;
        }
    }

    stream = Util::getStream("/etc/passwd");
    name = ("x:"+result);

    //Searching for name of the user with selected UID
    while(std::getline(stream,line)) {
        if(line.find(name) != std::string::npos) {//name==line
            result = line.substr(0, line.find(":"));
            return result;
        }
    }
    return "";
}
vector<string> ProcessParser::getSysCpuPercent(string coreNumber) {
    string line;
    std::string name = "cpu" + coreNumber;
    std::ifstream stream = Util::getStream(Path::basePath() + Path::statPath());
    while(std::getline(stream, line)){
        // Searching line by line
        if (line.compare(0, name.size(),name) == 0) {
            // slicing string line on ws for values using sstream
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            return values;
        }
    }
}

float ProcessParser::getSysActiveCpuTime(vector<string> values) {
    return(stof(values[S_USER]) +
    stof(values[S_NICE]) +
    stof(values[S_SYSTEM]) +
    stof(values[S_IRQ]) +
    stof(values[S_SOFTIRQ]) +
    stof(values[S_STEAL]) +
    stof(values[S_GUEST]) +
    stof(values[S_GUEST_NICE]));
}
float ProcessParser::getSysRamPercent() {
    string name1 = "MemAvailable";
    string name2 = "MemFree";
    string name3 = "Buffers";
    std::ifstream stream = Util::getStream((Path::basePath() + Path::memInfoPath()));
    string line;
    float totalMem = 0;
    float freeMem = 0;
    float buffers = 0;

    while (std::getline(stream, line)) {
        if (line.compare(0, name1.size(),name1) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            totalMem = stof(values[1]);
        }

        else if (line.compare(0, name2.size(),name2) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            freeMem = stof(values[1]);
        }

        else if (line.compare(0, name3.size(),name3) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            buffers = stof(values[1]);
        }
    }
    return float(100.0*(1-(freeMem/(totalMem-buffers))));
}
float ProcessParser::getSysIdleCpuTime(vector<string> values) {
    return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}

std::string ProcessParser::getSysKernelVersion() {
    string name = "Linux version ";
    string line;
    std::ifstream stream = Util::getStream((Path::basePath() + Path::versionPath()));
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(),name) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            return values[2];
        }
    }

}
int ProcessParser::getNumberOfCores()
{
    // Get the number of host cpu cores
    string line;
    string name = "cpu cores";
    std::ifstream stream = Util::getStream((Path::basePath() + "cpuinfo"));
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(),name) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            return stoi(values[3]);
        }
    }
    return 0;
}


int ProcessParser::getTotalThreads() {
    string line;
    int result = 0;
    string name = "Threads:";
    vector<string>_list = ProcessParser::getPidList();
    for(int i=0; i<_list.size(); i++) {
        string pid = _list[i];
        //getting every process and reading their number of their threads
        std::ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));

        while(std::getline(stream,line)) {
            if(line.compare(0, name.size(), name) == 0) {
                istringstream buf(line);
                istream_iterator<string> beg(buf), end;
                vector<string> values(beg, end);
                result += stoi(values[1]);
            }
        }
    }
    return result;
}

int ProcessParser::getTotalNumberOfProcesses()
{
    string line;
    int result = 0;
    string name = "processes";
    std::ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(), name) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            result += stoi(values[1]);
            break;
        }
    }
    return result;
}
int ProcessParser::getNumberOfRunningProcesses() {
    string line;
    int result = 0;
    string name = "procs_running";
    std::ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(), name) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            result += stoi(values[1]);
            break;
        }
    }
    return result;
}
std::string ProcessParser::getOSName() {
    std::ifstream stream = Util::getStream("/etc/os-release");
    string line;
    string name = "PRETTY_NAME=";
    int name_length = name.size();
    while (std::getline(stream, line)) {
        if (line.compare(0, name_length,name) == 0) {
            return line.substr(name_length,line.size()-name_length);
        }
    }
}
std::string ProcessParser::PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2)  {
    float activeTime = getSysActiveCpuTime(values2)-getSysActiveCpuTime(values1);
    float idleTime = getSysIdleCpuTime(values2)-getSysIdleCpuTime(values1);
    float totalTime = activeTime + idleTime;
    float result = 100.0*(activeTime/totalTime);
    return to_string(result);
}

bool ProcessParser::isPidExisting(string pid) {
    vector<string> v = getPidList();
    return std::find(v.begin(), v.end(), pid) != v.end();
}