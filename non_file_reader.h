/*
 *  Created on: 23 October, 2020
 *      Author: Lior Sinai
 * Description: read in nonogram files (.non or .txt)
 */

#ifndef NON_FILE_READER_H
#define NON_FILE_READER_H
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

struct Runs{

    std::vector<std::vector<int>> runs_row;
    std::vector<std::vector<int>> runs_col;
};

Runs read_non_file(std::string file_name);
Runs read_non_ext(std::string file_name);
Runs read_sgriddler_ext(std::string file_name);

# endif //NON_FILE_READER_H