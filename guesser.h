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

std::vector< std::vector<int> > rank_guesses(std::vector< std::vector<int> > grid);
std::vector<int> best_guess(std::vector< std::vector<int> > grid);


#endif // GUESSER_H
