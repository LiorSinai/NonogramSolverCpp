/*
 *  Created on: 22 October, 2020
 *      Author: Lior Sinai
 * Description: a dedicated non-deterministic finite state automation matcher
 * 
 * Sources:
 * https://swtch.com/~rsc/regexp/regexp1.html
 * https://deniskyashif.com/2019/02/17/implementing-a-regular-expression-engine/
 */

#ifndef MATCHER_NFA_H
#define MATCHER_NFA_H

#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>

#include "nonogram.h"

struct State
{
    char symbol = '\0';
    int id = 0;         // position in the NFA list
    char qualifier = '\0'; // purely for descriptive purposes
    bool is_end = false;
    std::vector<int> transitions; // states that this transitions to
};

struct Match
{
    std::vector<int> match{};
    std::vector<int> pattern{};
    bool is_match {false};
};

class NonDeterministicFiniteAutomation
{
public:
    NonDeterministicFiniteAutomation(){};
    void compile(std::vector<int>& pattern_); //convert a numerical pattern to a regex pattern
    Match find_match(std::vector<int>& array); //returns the left-most match

    std::vector<State> change_state(State &state, char in_symbol);

private:
    std::vector<State> states;
    std::vector<int> pattern;
    int num_states{0};     // also used to determine the state ids
    bool is_compiled{false};

    std::vector<char> convert_pattern(std::vector<int>& pattern); 
};

#endif // MATCHER_NFA_H