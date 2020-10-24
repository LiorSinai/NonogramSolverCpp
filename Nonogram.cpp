/*
 *  Created on: 22 October 2020
 *      Author: Lior Sinai
 * Description: 
 */

#include "Nonogram.h"

Nonogram::Nonogram(std::vector<std::vector<int>> runs_row_, std::vector<std::vector<int>> runs_col_)
{   
    runs_row = runs_row_;
    runs_col = runs_col_;
    n_rows = runs_row.size();
    n_cols = runs_col.size();

    for (int i{0}; i < n_rows; i++){
        std::vector<int> row;
        for (int j{0}; j < n_cols; j++){
            row.push_back(EITHER);
        }
        grid.push_back(row);
    }
}

std::vector<int> Nonogram::get_column(std::vector<std::vector<int>>& grid, int j){
    std::vector<int> column;
    for (int i{0}; i < n_rows; i++){
        column.push_back(grid[i][j]);
    }
    return column;
}

int max_length (std::vector<std::vector<int>> array){
    int length {0};
    for (auto vec: array){
        if (vec.size() > length){
            length = vec.size();
        }
    }
    return length;    
}


std::vector<std::vector<std::string>> Nonogram::make_box(std::vector<std::vector<int>>& grid, std::string symbols)
{   
    /* make a box which includes the instructions for the Nonogram */
    std::vector<std::vector<std::string>> box;
    int row_max_length = max_length(Nonogram::runs_row);
    int col_max_length = max_length(Nonogram::runs_col);

    for (int i{0}; i < col_max_length; ++i){
        std::vector<std::string> row;
        for (int j{0}; j < row_max_length + n_cols; ++j){
            if (j >= row_max_length && runs_col[j - row_max_length].size() >= col_max_length - i){
                auto runs = runs_col[j - row_max_length];
                int r = runs[runs.size() - (col_max_length - i)];
                std::string s = std::to_string(r);
                std::string s1 = std::string(2 - s.length(), '0') + s; //pad with zeros
                row.push_back(s1); 
            }
            else{
                std::string s (1, symbols[DUMMY]);
                row.push_back(s + " "); // push back a dummy variable
            }
        }
        box.push_back(row);
    }
    for (int i{0}; i < n_rows; ++i){
        std::vector<std::string> row;
        for (int j{0}; j < row_max_length; ++j){
            if (runs_row[i].size() >= row_max_length - j){
                int r = runs_row[i][runs_row[i].size() - (row_max_length - j)];
                std::string s = std::to_string(r);
                std::string s1 = std::string(2 - s.length(), '0') + s; //pad with zeros
                row.push_back(s1); 
            } 
            else{
                std::string s (1, symbols[DUMMY]);
                row.push_back(s + " "); // push back a dummy variable
            }            
        }
        for (int j{0}; j < n_cols; ++j){
            std::string s (1, symbols[grid[i][j]]);
            row.push_back(s + " ");
        }
        box.push_back(row);
    }


    return box;
}

void Nonogram::show_grid(matrix2D &grid, bool show_instructions, bool to_screen, bool to_file, std::string symbols)
{    
    /* Print nonogram to screen and possible a file as well*/
    std::ofstream outfile;
    if (to_file){
        // construct file name
        std::string base = "nonogram_";
        std::string ext = ".txt";
        // get current and date
        time_t  now = time(0); // current time
        struct tm * timeinfo;
        char buffer [80];
        time (&now);
        timeinfo = localtime (&now);
        strftime (buffer,80,"%Y%m%d_%H%M",timeinfo);
        // open (and create) the file
        std::string file_name = base + buffer + ext;
        outfile.open (file_name);
    }

    if (show_instructions){
        std::vector<std::vector<std::string>> box = make_box(grid, symbols);
        for (int i{0}; i< box.size(); i++){
            for (int j{0}; j < box[0].size(); j++){
                if (to_screen){
                    std::cout << box[i][j] << "  ";
                }
                if (to_file){
                    outfile << box[i][j] << " ";
                }
            }
            if (to_screen){
                std::cout << "\n";
            }
            if (to_file){
                outfile << "\n";
            }
        }
    }
    else{ 
        for (int i{0}; i < n_rows; i++){
            for (int j{0}; j < n_cols; j++){
                if (to_screen){
                    std::cout << symbols[grid[i][j]] << " ";
                }
                if (to_file){
                    outfile << symbols[grid[i][j]] << " ";
                }
            }
            if (to_screen){
                std::cout << "\n";
            }
            if (to_file){
                outfile << "\n";
            }
        }
    }
    std::cout << std::endl;
    if (to_file){
        outfile.close();
    }
}


std::vector<int> Nonogram::get_sequence(std::vector<int>& line){
    std::vector<int> sequence;
    bool on_blank = true;
    for (int cell: line){
        if (cell == cell_types::BOX && on_blank){
            sequence.push_back(1);
            on_blank = false;
        }
        else if (cell == cell_types::BOX && !on_blank){
            sequence.back() += 1;
        }
        else { //BLANK or EITHER
            on_blank = true;
        }
    } 
    return sequence;
}


bool Nonogram::is_valid_line(std::vector<int>& line, std::vector<int>& target_runs){
    std::vector<int> sequence = get_sequence(line);
    return sequence == target_runs;
}

bool Nonogram::is_valid_grid(matrix2D &grid)
{
    for (int i{0}; i < n_rows; i++){
        if (!is_valid_line(grid[i], runs_row[i])){
            return false;
        }
    } 
    for (int j{0}; j < n_cols; j++){
        if (!this->is_valid_line(get_column(grid, j), this->runs_col[j])){
            return false;
        }
    }
    return true;
}