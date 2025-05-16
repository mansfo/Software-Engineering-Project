#include "main.h"

address str_to_addr(std::string str){
    address result;
    result.number = atoi(str.substr(0,str.find(",")).c_str());
    str = str.substr(str.find(",")+1,str.length());
    result.roadname = str.substr(0,str.find(",")).c_str();
    str = str.substr(str.find(",")+1,str.length());
    result.city = str.substr(0,str.find(",")).c_str();
    str = str.substr(str.find(",")+1,str.length());
    result.nation = str.substr(0,str.find(",")).c_str();
    result.continent = str.substr(str.find(",")+1,str.length()).c_str();
    return result;
}