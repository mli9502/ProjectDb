//
// Created by Mengwen Li (ml4643) on 4/10/2021.
//

#ifndef PROJECTDB_CONFIG_PARSER_H
#define PROJECTDB_CONFIG_PARSER_H

#include <string>

using namespace std;

namespace projectdb {

class ConfigParser {
   public:
    static void parse(const string& configFilePath);
};

}  // namespace projectdb

#endif  // PROJECTDB_CONFIG_PARSER_H
