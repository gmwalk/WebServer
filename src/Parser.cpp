//
//  Parser.cpp
//  Simple WebServer
//
//  Created by George Walk on 3/26/18.
//  Copyright © 2018 George Walk. All rights reserved.
//
#include <sys/types.h>
#include <errno.h>
#include <iostream>
#include <map>
#include <string>
#include "http.h"
#include "Parser.hpp"

std::map<std::string, std::string> http_request;

std::map<std::string, int> token_map = {
    {"GET", GET},
    {"POST", POST},
    {"PUT", PUT},
    {"DELETE", DELETE},
    {"CONNECT", CONNECT},
    {"OPTIONS", OPTIONS},
    {"TRACE", TRACE},
    {"PATCH", PATCH},
    {"HEAD", HEAD}
};

/* Header parsing funtions. */
int GetContent_Length (void)
{
    auto itr = http_request.find("Content-Length");
    if (itr == http_request.end())
    {
        printf ("""Content-Length"" not found!\n");
        return 0;
    }
    return atoi(itr->second.c_str());
}

int GetRequestType (void)
{
    auto itr = http_request.find("Type");
    if (itr == http_request.end())
    {
        printf ("""Type"" not found!\n");
    }
    return parse_request(itr->second.c_str());
}

char *GetPath (void)
{
    auto itr = http_request.find("Path");
    if (itr == http_request.end())
    {
        printf ("""Path"" not found!\n");
    }
    return (char *)(itr->second.c_str());
}

int parse_request (const char *value)
{
    auto itr = token_map.find(value);
    
    if (itr != token_map.end())
    {
        return (itr->second);
    }else
        printf ("Request type not found\n");
    return 0;
}

void parseFirstLine(std::string line)
{
    
    std::string key = "";
    std::string value = "";
    unsigned long  position, lpost, req;
    
    // Find request type
    position = line.find(' ');
    value =line.substr(0, position);
    http_request["Type"] = line.substr(0, position);
    
    if ( !(req = parse_request(value.c_str()) ) )
    {
        printf ("Error: HTTP Req %s not found!\n", line.substr(0, position).c_str());
    }
    
//    printf ("HTTP Req: %d\n", (int)req);
    
    http_request[ "Type" ] = line.substr(0, position);
    position++; //Skip character ' '
    
    // Find path
    lpost = line.find(' ', position);
    http_request[ "Path" ] = line.substr(position, lpost-position);
    position = lpost; //Skip character ' '
    
    // Find HTTP version
    http_request[ "Version" ] = line.substr(position);
}

void parseHeader(std::string line)
{
    
    std::string key = "";
    std::string value = "";
    
    if(line.size() == 0) return;
    
    unsigned long posFirst = line.find(":",0); //Look for separator ':'
    
    key = line.substr(0, posFirst);
    value = line.substr(posFirst + 1);
    
    http_request[key] = value;
}

/* Misc. parsing functions */
using std::string;

string getPathName(const string& s) {
    
    char sep = '/';
    
#ifdef _WIN32
    sep = '\\';
#endif
    
    size_t i = s.rfind(sep, s.length());
    if (i != string::npos) {
        return(s.substr(0, i));
    }
    
    return("");
}

string getFileName(const string& s) {
    
    char sep = '/';
    
#ifdef _WIN32
    sep = '\\';
#endif
   
    string res = s.substr(s.find_last_of(sep)+1);
    
    size_t i = res.rfind ("?", res.length());

    return(res.substr(0,i));
}
