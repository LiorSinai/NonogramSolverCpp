/*
 *  Created on: 23 October, 2020
 *      Author: Lior Sinai
 * Description: Fast solver for the nonogram.
 * See: 
 * http://scc-forge.lancaster.ac.uk/open/nonogram/ls-fast 
 * https://webpbn.com/pbnsolve.html 
 */

#include "solver_fast.h"


Nonogram::matrix2D solve_fast(std::shared_ptr<Nonogram> puzzle, bool make_guess=true){
    return solve_fast_(puzzle->get_grid(), puzzle, make_guess);
}

Nonogram::matrix2D solve_fast_(Nonogram::matrix2D grid, std::shared_ptr<Nonogram> puzzle, bool make_guess, 
                               std::set<int> rows_to_edit, std::set<int> columns_to_edit){
    // extract values from Nonogram object
    int n_rows = puzzle->get_n_rows();
    int n_cols = puzzle->get_n_cols();
    std::vector<std::vector<int>> runs_row = puzzle->get_runs_row();
    std::vector<std::vector<int>> runs_col = puzzle->get_runs_col();

    //initialise
    if (rows_to_edit.empty() && columns_to_edit.empty()){
        for (int j{0}; j < n_cols; j++){
            columns_to_edit.insert(j);
        }

        for (int i{0}; i < n_rows; i++){
            fix_row(grid, runs_row[i], columns_to_edit, i);
        }
    }

    // constraint propagation
    int sweeps{1};
    while (!columns_to_edit.empty()){
        for (int j: columns_to_edit){
            fix_col(grid, runs_col[j], rows_to_edit, j);
        }
        columns_to_edit.clear();
        for (int i: rows_to_edit){
            fix_row(grid, runs_row[i], columns_to_edit, i);
        }
        rows_to_edit.clear();
        sweeps += 2;
    }
    std::cout<< "constraint propogation done in " << sweeps << " sweeps" << std::endl;

    return grid;
}

void fix_row(Nonogram::matrix2D& grid, std::vector<int>runs, std::set<int> &columns_to_edit, int i){
    std::vector<int> * row = &grid[i];
    int n_cols = row->size();
    std::vector<int> allowed = apply_strategies(*row, runs);
    for (int j{0}; j < n_cols; ++j){
        if ((*row)[j] != allowed[j] && allowed[j]!=EITHER){
            columns_to_edit.insert(j);
            grid[i][j] = allowed[j];
        }
    }
}

void fix_col(Nonogram::matrix2D& grid, std::vector<int>runs, std::set<int> &rows_to_edit, int j){
    std::vector<int> col =  get_column(grid, j);
    int n_cols = col.size();
    std::vector<int> allowed = apply_strategies(col, runs);
    for (int i{0}; i < n_cols; ++i){
        if (col[i] != allowed[i] && allowed[i]!=EITHER){
            rows_to_edit.insert(i);
            grid[i][j] = allowed[i];
        }
    }
}

std::vector<int> get_column(Nonogram::matrix2D& grid, int j){
    std::vector<int> column;
    for (int i{0}; i < (int)grid.size(); i++){
        column.push_back(grid[i][j]);
    }
    return column;
}

std::vector<int> apply_strategies(std::vector<int>& line, std::vector<int> &runs){
    std::vector<int> allowed(line.size(), EITHER);
    std::vector<int> allowed1 = left_rightmost_overlap(line, runs);
    std::vector<int> allowed2 = simple_filler(line, runs);
    for (int i{0}; i < line.size(); i ++){
        allowed[i] = allowed[i] & allowed1[i]; 
        allowed[i] = allowed[i] & allowed2[i];
    }
    return allowed;
}

std::vector<int> left_rightmost_overlap(std::vector<int> line, std::vector<int>runs){
    // left most
    std::unique_ptr<NonDeterministicFiniteAutomation> nfa = std::make_unique<NonDeterministicFiniteAutomation>();
    nfa->compile(runs);
    Match m_left = nfa->find_match(line);

    //right most
    std::reverse(line.begin(), line.end());
    std::reverse(runs.begin(), runs.end());
    nfa->compile(runs);
    Match m_right = nfa->find_match(line);
    std::reverse(m_right.match.begin(), m_right.match.end());

    if(m_left.is_match && m_right.is_match){
        std::vector<int> allowed = overlap(m_left.match, m_right.match);
        return allowed;
    }
    else{
        throw SolverError();
    }
}

std::vector<int> overlap(std::vector<int>& a, std::vector<int> &b){
    /* return the overlap between 2 vectors a and b
    *  There is an overlap if the numbers in each sequence are equal
    *  Even numbers (and zero) are BLANK and odd numbers are BOX
    */
    assert(a.size() == b.size());
    std::vector<int> out (a.size(), EITHER);
    std::vector<int> changer_a = changer_sequence(a);
    std::vector<int> changer_b = changer_sequence(b);

    for (int i{0}; i <(int)a.size(); i++){
        int x = changer_a[i] ;
        if (x == changer_b[i]){
            if ((x+2)% 2 == 0){
                out[i] = BLANK;
            }
            else{
                out[i] = BOX;
            }
        }
    }
    return out;
}


std::vector<int> changer_sequence(std::vector<int>& line){
    int counter = (int) line[0] == BOX;
    int prev = line[0];
    std::vector<int> out(line.size(), 0);
    out[0] = counter;
    for (int i{1}; i < out.size(); i++){
        counter += (int)(prev!= line[i]);
        out[i] = counter;
        prev = line[i];
    }
    return out;
}

std::vector<int> simple_filler(std::vector<int>& line, std::vector<int> &runs){
    /* easy to spot boxes and blank that the left-right matcher might miss*/
    int k{0};
    int on_run {0};
    std::vector<int> allowed = line; //copy
    for (int i{0}; i < (int)line.size(); ++i){
        int x = line[i];
        if (x == BLANK){
            if(on_run > 0){
                ++k; // move to the next pattern
            }
            on_run = 0;
        }
        else if (x == BOX){
            ++on_run;
        }
        else{ //x==EITHER
            if (k >= (int)runs.size()){
                break;
            }
            else if (0 < on_run && on_run < runs[k]){ //must be a gap in a run
                allowed[i] = BOX;
                ++on_run;
            }
            else if (on_run==0 && k > 0 && i > 0 && line[i-1]==BOX){ //this must be a BLANK ending a run
                allowed[i] = BLANK;
            }
            else{
                break; //too many unknowns
            }
        }
    }
    return allowed;
}