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


using namespace std::string;
using namespace std::vector;
class ProcessParser{
private:
    std::ifstream stream;
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
    static string getOSName();
    static std::string PrintCpuStats(vector<string> values1, vector<string>values2);
    static bool isPidExisting(string pid);
};

// TODO: Define all of the above functions below:

static string ProcessParser::getCmd(string pid) {
    stream = Util::getStream(Path::basePath() + pid + Path::cmdPath());
    string line;
    while(getline(stream, line)) {
        return line;
    }
  }

}

static vector<string> ProcessParser::getPidList() {

}

static std::string ProcessParser::getVmSize(string pid) {
    std::string line;
    std::string name = "VmData";
    std::string value;
    float result;
    stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
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
    return 
}
static std::string ProcessParser::getCpuPercent(string pid) {
    return 
}
static long int ProcessParser::getSysUpTime() {
    Util::getStream(Path::basePath() + pid + Path::cmdPath(), stream);
    return 
}
static std::string ProcessParser::getProcUpTime(string pid) {
    return 
}
static string ProcessParser::getProcUser(string pid) {
    return  
}
static vector<string> ProcessParser::getSysCpuPercent(string coreNumber = "") {
    return 
}
static float ProcessParser::getSysRamPercent() {
    return 
}
static string ProcessParser::getSysKernelVersion() {
    return 
}
static int ProcessParser::getNumberOfCores() {
    return 
}
static int ProcessParser::getTotalThreads() {
    return 
}
static int ProcessParser::getTotalNumberOfProcesses() {
    return 
}
static int ProcessParser::getNumberOfRunningProcesses() {
    return 
}
static string ProcessParser::getOSName() {
    Util::getStream(Path::basePath() + Path::versionPath(), stream);
    return ;
}
static std::string ProcessParser::PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2)  {
    return 
}
static bool ProcessParser::isPidExisting(string pid) {
    return 
}