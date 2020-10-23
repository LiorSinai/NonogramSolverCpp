#include <iostream>

#include "Nonogram.h"
#include "matcherNFA.h"
#include "matcher_tester.h"


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


    std::unique_ptr<Nonogram> puzzle = std::make_unique<Nonogram>(r_rows, r_cols);
    puzzle->set_grid(solution);

    puzzle->show_grid(puzzle->get_grid(), true, true);
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

    nfa_tester->addTest(t1);
    nfa_tester->addTest(t2);
    nfa_tester->addTest(t3);
    nfa_tester->addTest(t4);
    nfa_tester->addTest(t5);
    nfa_tester->addTest(t6);
    nfa_tester->addTest(t7);
    nfa_tester->run();
    
    delete t1, t2, t3, t4, t5, t6;
}

int main()
{   
   //nonogram_basics();
   nfa_tester();

    return 0;
}




