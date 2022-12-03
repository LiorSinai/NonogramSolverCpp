/*
 *  Created on: 23 October, 2020
 *      Author: Lior Sinai
 * Description: Tests for the NFA Matcher
 *
 */

#include "matcher_tester.h"

// NFA_tests
bool NFA_test::setUp()
{
    try
    {
        nfa = std::make_unique<NonDeterministicFiniteAutomation>();
    }
    catch (std::bad_alloc &e)
    {
        std::cerr << "Error setting up test: " << e.what() << std::endl;
        return false;
    }
    return true;
}

// NFA_tests
void BasicTest::run()
{
    pattern = {3, 2, 1};
    this->nfa->compile(pattern);

    line = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
    result = {BLACK, BLACK, BLACK, WHITE, BLACK, BLACK, WHITE, BLACK, WHITE, WHITE};
    m = this->nfa->find_match(line);
    IS_TRUE(m.match == result);

    line = {BLACK, BLACK, BLACK, BLACK, 3, 3, 3, 3, 3, 3}; // too many blacks at start
    m = this->nfa->find_match(line);
    IS_FALSE(m.is_match);

    line = {BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK}; // no space for last 1
    m = this->nfa->find_match(line);
    IS_FALSE(m.is_match);

    line = {WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, 3, 3, 3}; // # no space for 2 or 1
    m = this->nfa->find_match(line);
    IS_FALSE(m.is_match);

    line = {3, 3, BLACK, 3, 3, 3, BLACK, BLACK, 3, BLACK};
    result = {BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, WHITE, BLACK};
    m = this->nfa->find_match(line);
    IS_TRUE(m.match == result);

    line = {3, 3, 3, 3, BLACK, 3, BLACK, BLACK, 3, BLACK};
    result = {WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, BLACK, BLACK, WHITE, BLACK}; // right-most shift
    m = this->nfa->find_match(line);
    IS_TRUE(m.match == result);
}

std::vector<int> string_to_vec(std::string line_str)
{
    std::unordered_map<char, int> symbols2int;
    symbols2int['-'] = BLANK;
    symbols2int['#'] = BOX;
    symbols2int[' '] = EITHER;
    std::vector<int> line = {};
    for (char c : line_str)
    {
        line.push_back(symbols2int[c]);
    }
    return line;
}

void LancasterExample::run()
{
    std::string s0 = "---#--         -      # ";
    std::string s1 = "---#--#-----------#####-";
    std::string s2 = "---#-------------#-#####";

    line = string_to_vec(s0);
    result = string_to_vec(s1);
    // left match
    pattern = {1, 1, 5};
    this->nfa->compile(pattern);
    m = this->nfa->find_match(line);
    IS_TRUE(m.match == result);

    // right match
    std::reverse(pattern.begin(), pattern.end());
    this->nfa->compile(pattern);
    std::reverse(line.begin(), line.end());
    m = this->nfa->find_match(line);
    std::reverse(m.match.begin(), m.match.end());
    result = string_to_vec(s2);
    IS_TRUE(m.match == result);
}

void LongMiddle::run()
{
    pattern = {10};
    // left most
    line = {WHITE, EITHER, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, EITHER, WHITE, WHITE};
    result = {WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE};
    this->nfa->compile(pattern);
    m = this->nfa->find_match(line);
    IS_TRUE(m.match == result);

    // right most
    std::reverse(line.begin(), line.end());
    result = {WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE};
    m = this->nfa->find_match(line);
    std::reverse(m.match.begin(), m.match.end());
    IS_TRUE(m.match == result);
}

void WorstCase::run()
{
    pattern = {2, 5, 3, 2};
    line = {};
    for (int i{0}; i < 30; ++i)
    {
        line.push_back(EITHER);
    }
    line.push_back(BLACK); // one black at the end
    result = {BLACK, BLACK, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, BLACK, BLACK, BLACK};
    for (int i{12}; i < 29; ++i)
    {
        result.push_back(WHITE);
    }
    line[0] = BLACK;
    result.push_back(BLACK);
    result.push_back(BLACK);

    this->nfa->compile(pattern);
    m = this->nfa->find_match(line);
    IS_TRUE(m.match == result);

    pattern = {4, 1, 3, 4, 1, 1, 3};
    std::vector<int> line2(40, EITHER);
    line2[0] = BLACK;
    line2[1] = BLACK;
    line2[2] = BLACK;
    line2[3] = BLACK;
    line2[38] = BLACK;
    line2[39] = BLACK;
    result = {BLACK, BLACK, BLACK, BLACK, WHITE, BLACK, WHITE, BLACK, BLACK, BLACK, WHITE, BLACK, BLACK, BLACK, BLACK, WHITE, BLACK, WHITE, BLACK};
    for (int i = 19; i < 37; i++)
    {
        result.push_back(WHITE);
    }
    result.push_back(BLACK);
    result.push_back(BLACK);
    result.push_back(BLACK);
    this->nfa->compile(pattern);
    m = this->nfa->find_match(line2);
    IS_TRUE(m.match == result);
}

void VeryLong::run()
{
    pattern = {2, 4, 9, 7, 1, 1, 8, 8, 3};
    line = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3, 3, 1, 1, 1, 1, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 3, 3, 3, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
    result = {1, 1, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

    this->nfa->compile(pattern);
    m = this->nfa->find_match(line);
    IS_TRUE(m.match == result);
}

void EmptyArrays::run()
{
    // no pattern
    pattern = {};
    line = {3, 3, 3, 3};
    result = {BLANK, BLANK, BLANK, BLANK};
    this->nfa->compile(pattern);
    m = this->nfa->find_match(line);
    IS_TRUE(m.match == result);

    line = {3, 3, 1, 3};
    m = this->nfa->find_match(line);
    IS_FALSE(m.is_match);

    // No line
    pattern = {3, 2, 1};
    line = {};
    result = {BLANK, BLANK, BLACK, BLANK};
    this->nfa->compile(pattern);
    m = this->nfa->find_match(line);
    IS_FALSE(m.is_match);

    // no line or pattern
    this->nfa->compile({});
    line = {};
    m = this->nfa->find_match(line);
    IS_TRUE(m.is_match);
}

void ProblemRun::run()
{
    pattern = {4, 2, 1, 3, 1, 3};
    std::vector<int> line2(40, EITHER);
    line2[0] = BLACK;
    line2[1] = BLACK;
    line2[2] = BLACK;
    line2[3] = BLACK;
    line2[10] = BLACK;
    line2[38] = BLACK;
    line2[39] = BLACK;
    result = {BLACK, BLACK, BLACK, BLACK, WHITE, BLACK, BLACK, WHITE, BLACK, WHITE, BLACK, BLACK, BLACK, WHITE, BLACK};
    for (int i = 14; i < 36; i++)
    {
        result.push_back(WHITE);
    }
    result.push_back(BLACK);
    result.push_back(BLACK);
    result.push_back(BLACK);

    this->nfa->compile(pattern);
    m = this->nfa->find_match(line2);
    IS_TRUE(m.match == result);
}