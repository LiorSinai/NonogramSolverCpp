/*
 *  Created on: 23 October, 2020
 *      Author: Lior Sinai
 * Description: Fast solver for the nonogram.
 * See: 
 * http://scc-forge.lancaster.ac.uk/open/nonogram/ls-fast 
 * https://webpbn.com/pbnsolve.html 
 */

#include "solver_fast.h"


Nonogram::matrix2D NonogramSolver::solve_fast(){
    return this->solve_fast_(puzzle->get_grid(), make_guess);
}

Nonogram::matrix2D NonogramSolver::solve_fast_(
    Nonogram::matrix2D grid, bool make_guess, 
    std::set<int> rows_to_edit, std::set<int> columns_to_edit){
    // extract values from Nonogram object

    //initialise
    if (rows_to_edit.empty() && columns_to_edit.empty()){
        for (int j{0}; j < n_cols; j++){
            columns_to_edit.insert(j);
        }

        for (int i{0}; i < n_rows; i++){
            fix_row(grid, columns_to_edit, i);
        }
    }  
    // constraint propogation
    int sweeps{1};
    while (!columns_to_edit.empty()){
        for (int j: columns_to_edit){
            fix_col(grid, rows_to_edit, j);
        }
        columns_to_edit.clear();
        for (int i: rows_to_edit){
            fix_row(grid,columns_to_edit,  i);
        }
        rows_to_edit.clear();
        sweeps += 2;
    }
    std::cout<< "constraint propogation done in " << sweeps << " sweeps" << std::endl;

    return grid;
}

void NonogramSolver::fix_row(Nonogram::matrix2D& grid, std::set<int>& columns_to_edit, int i){
    std::vector<int> * row = &grid[i];
    int n_cols = row->size();
    //std::vector<int> allowed = apply_strategies(*row, this->runs_row[i]);
    // apply strategies
    std::vector<int> allowed(row->size(), EITHER);
    std::vector<int> allowed1 = left_rightmost_overlap_precompiled(*row, nfa_rows.left[i], nfa_rows.right[i]);
    for (int j{0}; j < n_cols; j ++){
        allowed[j] = allowed[j] & allowed1[j];
    }
    for (int j{0}; j < n_cols; ++j){
        if ((*row)[j] != allowed[j] && allowed[j]!=EITHER){
            columns_to_edit.insert(j);
            grid[i][j] = allowed[j];
        }
    }
}

void NonogramSolver::fix_col(Nonogram::matrix2D& grid, std::set<int>& rows_to_edit, int j){
    std::vector<int> col =  this->get_column(grid, j);
    int n_rows = col.size();
    //std::vector<int> allowed = apply_strategies(col, this->runs_col[j]);  
    // apply strategies
    std::vector<int> allowed(col.size(), EITHER);
    std::vector<int> allowed1 = left_rightmost_overlap_precompiled(col, nfa_cols.left[j], nfa_cols.right[j]);
    for (int i{0}; i < n_rows; ++i){
        allowed[i] = allowed[i] & allowed1[i];
    }
    for (int i{0}; i < n_rows; ++i){
        if (col[i] != allowed[i] && allowed[i]!=EITHER){
            rows_to_edit.insert(i);
            grid[i][j] = allowed[i];
        }
    }
}

std::vector<int> NonogramSolver::get_column(Nonogram::matrix2D& grid, int j){
    std::vector<int> column(grid.size());
    for (int i{0}; i < (int)grid.size(); i++){
        column[i] = grid[i][j];
    }
    return column;
}

// std::vector<int> NonogramSolver::apply_strategies(std::vector<int>& line, std::vector<int> &runs){
//     std::vector<int> allowed(line.size(), EITHER);
//     std::vector<int> allowed1 = left_rightmost_overlap(line, runs);
//     for (int i{0}; i < line.size(); i ++){
//         allowed[i] = allowed[i] & allowed1[i];
//     }
//     return allowed;
// }

// std::vector<int> NonogramSolver::left_rightmost_overlap(std::vector<int> line, std::vector<int>runs){
//     // left most
//     std::unique_ptr<NonDeterministicFiniteAutomation> nfa = std::make_unique<NonDeterministicFiniteAutomation>();
//     nfa->compile(runs);
//     Match m_left = nfa->find_match(line);

//     //right most
//     std::reverse(line.begin(), line.end());
//     std::reverse(runs.begin(), runs.end());
//     nfa->compile(runs);
//     Match m_right = nfa->find_match(line);
//     std::reverse(m_right.match.begin(), m_right.match.end());

//     if(m_left.is_match && m_right.is_match){
//         std::vector<int> allowed = overlap(m_left.match, m_right.match);
//         return allowed;
//     }
//     else{
//         throw SolverError();
//     }
// }

std::vector<int> NonogramSolver::left_rightmost_overlap_precompiled(
        std::vector<int> line, 
        std::shared_ptr<NonDeterministicFiniteAutomation> nfa_left,
        std::shared_ptr<NonDeterministicFiniteAutomation> nfa_right){
        // left most    
        Match m_left = nfa_left->find_match(line);

        //right most
        std::reverse(line.begin(), line.end());
        Match m_right = nfa_right->find_match(line);
        std::reverse(m_right.match.begin(), m_right.match.end());

        if(m_left.is_match && m_right.is_match){
        std::vector<int> allowed = overlap(m_left.match, m_right.match);
            return allowed;
        }
        else{
            throw SolverError();
    }
}

std::vector<int> NonogramSolver::overlap(std::vector<int>& a, std::vector<int> &b){
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


std::vector<int> NonogramSolver::changer_sequence(std::vector<int>& line){
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

void NonogramSolver::precompile_nfas(){
    for (int j{0}; j < this->n_cols; j++){
        // left most
        std::shared_ptr<NonDeterministicFiniteAutomation> nfa_l = std::make_shared<NonDeterministicFiniteAutomation>();
        nfa_l->compile(this->runs_col[j]);
        this->nfa_cols.left.push_back(nfa_l);
        // right most
        std::vector<int> runs_reversed = this->runs_col[j];
        std::reverse(runs_reversed.begin(), runs_reversed.end());
        std::shared_ptr<NonDeterministicFiniteAutomation> nfa_r = std::make_shared<NonDeterministicFiniteAutomation>();
        nfa_r->compile(runs_reversed);
        this->nfa_cols.right.push_back(nfa_r);
    }
    for (int i{0}; i < this->n_rows; i++){
        // left most
        std::shared_ptr<NonDeterministicFiniteAutomation> nfa_l = std::make_shared<NonDeterministicFiniteAutomation>();
        nfa_l->compile(runs_row[i]);
        this->nfa_rows.left.push_back(nfa_l);
        // right most
        std::vector<int> runs_reversed = runs_row[i];
        std::reverse(runs_reversed.begin(), runs_reversed.end());
        std::shared_ptr<NonDeterministicFiniteAutomation> nfa_r = std::make_shared<NonDeterministicFiniteAutomation>();
        nfa_r->compile(runs_reversed);
        this->nfa_rows.right.push_back(nfa_r);
    }
}