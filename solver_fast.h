/*
 *  Created on: 23 October, 2020
 *      Author: Lior Sinai
 * Description: Fast solver for the nonogram.
 * See: 
 * http://scc-forge.lancaster.ac.uk/open/nonogram/ls-fast 
 * https://webpbn.com/pbnsolve.html 
 */

#ifndef SOLVER_FAST_H
#define SOLVER_FAST_H

#include <set>
#include <exception>

#include "Nonogram.h"
#include "matcherNFA.h"



class NonogramSolver{
public:
    NonogramSolver(std::shared_ptr<Nonogram> puzzle_in, bool make_guess_in=false): 
        puzzle(puzzle_in), make_guess(make_guess_in) {
            // extract values from Nonogram object
            this -> runs_row = puzzle_in->get_runs_row();
            this -> runs_col = puzzle_in->get_runs_col();
            this -> n_rows = puzzle_in->get_n_rows();
            this -> n_cols = puzzle_in->get_n_cols();

            precompile_nfas();
        }
    Nonogram::matrix2D solve_fast();

private:
    std::shared_ptr<Nonogram> puzzle;
    bool make_guess;
    std::vector<std::vector<int>> runs_row;
    std::vector<std::vector<int>> runs_col;
    int n_rows;
    int n_cols;
    int guesses;

    struct nfa_list {
        std::vector<std::shared_ptr<NonDeterministicFiniteAutomation>>left;  
        std::vector<std::shared_ptr<NonDeterministicFiniteAutomation>>right;
    }; 
    nfa_list nfa_rows;
    nfa_list nfa_cols;

    struct SolverError : public std::exception
    {
        const char * what () const throw ()
        {
            return "found a contradiction during solving";
        }
    };

    Nonogram::matrix2D solve_fast_(Nonogram::matrix2D grid, bool make_guess=false, 
                                std::set<int> rows_to_edit={}, std::set<int> columns_to_edit={});

    void fix_row(Nonogram::matrix2D& grid, std::set<int>& columns_to_edit, int i);
    void fix_col(Nonogram::matrix2D& grid, std::set<int>& rows_to_edit, int j);  

    std::vector<int> get_column(Nonogram::matrix2D& grid, int j);
    std::vector<int> changer_sequence(std::vector<int>& line);
    std::vector<int> overlap(std::vector<int>& a, std::vector<int> &b);
    //std::vector<int> left_rightmost_overlap(std::vector<int> line, std::vector<int>runs);  // don't pass by reference because these are edited
    //std::vector<int> apply_strategies(std::vector<int>& line, std::vector<int> &runs);
    
    std::vector<int> left_rightmost_overlap_precompiled(std::vector<int> line, 
                std::shared_ptr<NonDeterministicFiniteAutomation> nfa_left, 
                std::shared_ptr<NonDeterministicFiniteAutomation> nfa_right);
    void precompile_nfas ();
};

//std::vector<int> simple_filler(std::vector<int>& line, std::vector<int> &runs);


#endif // SOLVER_FAST_H
