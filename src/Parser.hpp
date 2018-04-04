//
//  Parser.hpp
//  Simple WebServer
//
//  Created by George Walk on 3/26/18.
//  Copyright © 2018 George Walk. All rights reserved.
//

#ifndef Parser_hpp
#define Parser_hpp

#include <stdio.h>

int parse_request (const char *value);
void parseFirstLine(std::string line);
void parseHeader(std::string line);
int GetRequestType (void);
char *GetPath (void);

off_t Get_File_Size (char *fname);
int GetContent_Length (void);

std::string getPathName(const std::string& s);
std::string getFileName(const std::string& s);

#endif /* Parser_hpp */
