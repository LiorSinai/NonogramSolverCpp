/*
 *  Created on: 23 October, 2020
 *      Author: Lior Sinai
 * Description: read in nonogram files
 */

#include "non_file_reader.h"

Runs read_non_file(std::string file_name){
    std::string line;
    std::ifstream infile(file_name);

    std::vector<std::vector<int>> runs_row{};
    std::vector<std::vector<int>> runs_col{};

    std::vector<std::vector<int>>* active_runs;

    int read_mode{0};

    while (std::getline(infile, line))
    {   
        std::vector<int> runs;
        std::stringstream ss(line); 
        switch (read_mode){
            case 0: // read in words
                if (line=="rows"){
                    active_runs = &runs_row;
                    read_mode = 1;
                }
                else if (line=="columns"){
                    active_runs = &runs_col;
                    read_mode = 1;
                }
                break;
            case 1:
                // double check if there are more words
                if (line==""){
                    read_mode = 0;
                    break;
                }
                else if (line=="rows"){
                    active_runs = &runs_row;
                    read_mode = 1;
                    break;
                }
                else if (line=="columns"){
                    active_runs = &runs_col;
                    read_mode = 1;
                    break;
                }
                // Parse the string 
                for (int i; ss >> i;) { 
                    runs.push_back(i); 
                    if (ss.peek() == ',') 
                        ss.ignore(); 
                } 
                active_runs->push_back(runs);
                break;
        }
    }
    if (runs_row.empty() || runs_col.empty()){
        throw "WARNING no file read";
    }
    else{
        std::cout << "file loaded" << std::endl;
    }

    return Runs{runs_row, runs_col};
    }

