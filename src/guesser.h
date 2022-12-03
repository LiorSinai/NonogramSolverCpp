/*
 *  Created on: 27 October, 2020
 *      Author: Lior Sinai
 * Description: rank guesses for the fast solver. Cells with more completed neighbours are better candidates for guesses.
 */

#ifndef GUESSER_H
#define GUESSER_H

#include <vector>
#include <algorithm> // for sort

#include "Nonogram.h"

template <typename T>
struct Rank
{
    int i;
    int j;
    T rank;
};

std::vector<Rank<int>> rank_solved_neighbours(const std::vector<std::vector<int>> &grid);
std::vector<Rank<int>> rank_solved_lines(const std::vector<std::vector<int>> &grid);
Rank<int> rank_max_neighbours(const std::vector<std::vector<int>> &grid);

#endif // GUESSER_H
