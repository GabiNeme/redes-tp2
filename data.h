#ifndef DATA_H
#define DATA_H

#include <map>
#include <iostream>
#include <sstream>
#include <string> 
#include <algorithm>

class Data{
private:
    std::map<std::string, std::string> hostname_ip_table;

public:
    Data();

    void add(std::string cmd_line);

    std::string search(std::string cmd_line);
    std::string search_hostname(std::string hostname);


};

#endif