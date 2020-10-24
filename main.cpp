#include <iostream>
#include <chrono>
using namespace std::chrono; 

#include "Nonogram.h"
#include "matcherNFA.h"
#include "matcher_tester.h"
#include "solver_fast.h"
#include "non_file_reader.h"


void nonogram_basics()
{
     /* demonstrate basic capabilities of the class */
     //elephant
    std::vector<std::vector<int>> r_rows = {{3},{4,2},{6,6},{6,2,1},{1,4,2,1}, {6,3,2},{6,7},{6,8},{1,10},{1,10}, {1,10},{1,1,4,4},{3,4,4},{4,4},{4,4}};
    std::vector<std::vector<int>> r_cols = {{1},{11},{3,3,1},{7,2},{7}, {15}, {1,5,7},{2,8},{14},{9}, {1,6},{1,9},{1,9},{1,10},{12}};
    std::vector<std::vector<int>> solution = { 
        {2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2},
        {2, 2, 1, 1, 1, 1, 2, 1, 1, 2, 2, 2, 2, 2, 2},
        {2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2},
        {2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 2, 2, 2, 1},
        {2, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 2, 2, 2, 1},
        {2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 2, 1, 1},
        {2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1},
        {2, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {2, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {2, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {2, 1, 2, 1, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1},
        {2, 1, 1, 1, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1},
        {2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1},
        {2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1}
        };


    std::shared_ptr<Nonogram> puzzle = std::make_shared<Nonogram>(r_rows, r_cols);
    Nonogram::matrix2D solution2 = solve_fast(puzzle, false);

    //puzzle->set_grid(solution);
    puzzle->set_grid(solution2);

    puzzle->show_grid(puzzle->get_grid(), true, false);
    std::cout << "Is complete: " << puzzle->is_valid_grid(puzzle->get_grid()) << std::endl;
}

void nfa_tester(){
    std::unique_ptr<TestSuite> nfa_tester = std::make_unique<TestSuite>();

    BasicTest* t1 = new BasicTest();
    LancasterExample* t2 = new LancasterExample();
    LongMiddle* t3 = new LongMiddle();
    WorstCase* t4 = new WorstCase();
    VeryLong* t5 = new VeryLong();
    NoPattern* t6 = new NoPattern();
    NoLine* t7 = new NoLine();
    ProblemRun* t8 = new ProblemRun();

    nfa_tester->addTest(t1);
    nfa_tester->addTest(t2);
    nfa_tester->addTest(t3);
    nfa_tester->addTest(t4);
    nfa_tester->addTest(t5);
    nfa_tester->addTest(t6);
    nfa_tester->addTest(t7);
    nfa_tester->addTest(t8);
    nfa_tester->run();
    
    delete t1, t2, t3, t4, t5, t6, t7, t8;
}

void solve_nonogram(std::vector<std::vector<int>> run_rows, std::vector<std::vector<int>> run_col)
{   
    std::shared_ptr<Nonogram> puzzle = std::make_shared<Nonogram>(run_rows, run_col);

    auto start = high_resolution_clock::now(); 
    Nonogram::matrix2D solution = solve_fast(puzzle, false);
    auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(stop - start); 
    std::cout << "time to solve: " << (float)duration.count()/1000 << "s" << std::endl; 

    puzzle->set_grid(solution);

    puzzle->show_grid(puzzle->get_grid(), false, false, false);
    std::cout << "Is complete: " << puzzle->is_valid_grid(puzzle->get_grid()) << std::endl;

}

int main()
{   
   //nonogram_basics();
   //nfa_tester();
   Runs r = read_non_file("puzzles/balance_puzzle.txt");
   solve_nonogram(r.runs_row, r.runs_col);

    return 0;
}
