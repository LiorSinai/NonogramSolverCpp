/*
 *  Created on: 23 October, 2020
 *      Author: Lior Sinai
 * Description: Tests for the NFA Matcher
 * 
 */

#include "matcherNFA.h"
#include "unit_tester.h"


class NFA_test: public UnitTest {
  // create a common setup for all the tests
  public:
    NFA_test(const std::string& name) : UnitTest(name) {}
    virtual bool setUp();
    virtual void run() = 0;
    std::unique_ptr<NonDeterministicFiniteAutomation> nfa;
    std::vector<int> line;
    std::vector<int> pattern;
    std::vector<int> result;
    Match m;
};

class BasicTest: public NFA_test{
public:
    BasicTest() : NFA_test("Basics"){}
    void run();
};

class LancasterExample: public NFA_test{
  //http://scc-forge.lancaster.ac.uk/open/nonogram/ls-fast
public:
    LancasterExample() : NFA_test("LancasterExample"){}
    void run();
};

class LongMiddle: public NFA_test{
public:
    LongMiddle() : NFA_test("LongMiddle"){}
    void run();
};

class WorstCase: public NFA_test{
public:
    WorstCase() : NFA_test("WorstCase"){}
    void run();
};

class VeryLong: public NFA_test{
public:
    VeryLong() : NFA_test("VeryLong"){}
    void run();
};


class NoPattern: public NFA_test{
public:
    NoPattern() : NFA_test("NoPattern"){}
    void run();
};

class NoLine: public NFA_test{
public:
    NoLine() : NFA_test("NoLine"){}
    void run();
};


class FullLine: public NFA_test{
public:
    FullLine() : NFA_test("FullLine"){}
    void run();
};


class ProblemRun: public NFA_test{
public:
    /* initial versions of the matcher failed on this*/
    ProblemRun() : NFA_test("ProblemRun"){}
    void run();
};