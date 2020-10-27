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
#include <algorithm> // for max

#include "Nonogram.h"
#include "matcherNFA.h"
#include "guesser.h"

Nonogram::matrix2D solve_fast_(Nonogram::matrix2D grid, std::shared_ptr<Nonogram> puzzle, bool make_guess=false, 
                               std::set<int> rows_to_edit={}, std::set<int> columns_to_edit={});
Nonogram::matrix2D solve_fast(std::shared_ptr<Nonogram> puzzle, bool make_guess);

float get_progress(Nonogram::matrix2D& grid);

void fix_row(Nonogram::matrix2D& grid, std::vector<int>runs, std::set<int> &columns_to_edit, int i);
void fix_col(Nonogram::matrix2D& grid, std::vector<int>runs, std::set<int> &rows_to_edit, int j);
std::vector<int> get_column(Nonogram::matrix2D& grid, int j);

std::vector<int> apply_strategies(std::vector<int>& line, std::vector<int> &runs);
std::vector<int> left_rightmost_overlap(std::vector<int>& line, std::vector<int>&runs); 
std::vector<int> overlap(std::vector<int>& a, std::vector<int> &b);
std::vector<int> changer_sequence(std::vector<int>& line);
std::vector<int> simple_filler(std::vector<int>& line, std::vector<int> &runs);
std::vector<std::pair<std::vector<int>, std::vector<int>>> splitter(std::vector<int>& line, std::vector<int>&runs);

Match minumum_match(std::vector<int>& line, std::vector<int> &runs);

struct SolverError : public std::exception
{
	const char * what () const throw ()
    {
    	return "found a contradiction during solving";
    }
};

#endif // SOLVER_FAST_H
