/*
 *  Created on: 22 October, 2020
 *      Author: Lior Sinai
 * Description: A nonogram class. See https://en.wikipedia.org/wiki/Nonogram 
 */


#ifndef NONOGRAM_H
#define NONOGRAM_H

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <time.h>
#include <ctime>

class Nonogram
{
private:
    std::vector<std::vector<int>> runs_row;
    std::vector<std::vector<int>> runs_col;
    std::vector<std::vector<int>> grid; 

    int n_rows;
    int n_cols;
    int guesses {0};

    std::vector<int> get_column(std::vector<std::vector<int>> grid, int j);
    std::vector<std::vector<std::string>> make_box(std::vector<std::vector<int>> grid, std::string symbols ="x#.?-");
public:
	/// Constructor 
	Nonogram(std::vector<std::vector<int>> runs_row_, std::vector<std::vector<int>> runs_col_);

    // setters and getters
    int get_n_rows(){ return n_rows;} 
    int get_n_cols(){ return n_cols;}  
    int get_guesses(){return guesses;}
    void set_grid(std::vector<std::vector<int>> &new_grid){grid = new_grid;}
    std::vector<std::vector<int>> &get_runs_row() {return runs_row;}
    std::vector<std::vector<int>> &get_runs_col() {return runs_col;}
    std::vector<std::vector<int>> &get_grid() {return grid;}

    // useful functions
    void show_grid(std::vector<std::vector<int>> &grid, bool show_instructions=false, bool to_file=false, std::string symbols ="x#.?-");
    std::vector<int> get_sequence(std::vector<int> line);
    bool is_valid_line(std::vector<int> line, std::vector<int> target_runs);
    bool is_valid_grid(std::vector<std::vector<int>> &grid);
};

enum cell_types {BOX=1, BLANK=2, EITHER=3, DUMMY=4};
enum cell_types2 {BLACK=1, WHITE=2, EITHER_=3};

#endif // NONOGRAM_H
