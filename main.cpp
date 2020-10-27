#include <iostream>
#include <chrono>

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

    puzzle->show_grid(puzzle->get_grid(), true, true, false);//show_instructions, to_screen, to_file
    std::cout << "Is complete: " << puzzle->is_valid_grid(puzzle->get_grid()) << std::endl;
}

void nfa_tester(){
    std::unique_ptr<TestSuite> nfa_tester = std::make_unique<TestSuite>();

    BasicTest* t1 = new BasicTest();
    LancasterExample* t2 = new LancasterExample();
    LongMiddle* t3 = new LongMiddle();
    WorstCase* t4 = new WorstCase();
    VeryLong* t5 = new VeryLong();
    EmptyArrays* t6 = new EmptyArrays();
    ProblemRun* t7 = new ProblemRun();

    nfa_tester->addTest(t1);
    nfa_tester->addTest(t2);
    nfa_tester->addTest(t3);
    nfa_tester->addTest(t4);
    nfa_tester->addTest(t5);
    nfa_tester->addTest(t6);
    nfa_tester->addTest(t7);
    nfa_tester->run();
    
    delete t1, t2, t3, t4, t5, t6, t7;
}

void solve_nonogram(std::vector<std::vector<int>> run_rows, std::vector<std::vector<int>> run_col)
{   
    std::shared_ptr<Nonogram> puzzle = std::make_shared<Nonogram>(run_rows, run_col);

    auto start = std::chrono::high_resolution_clock::now(); 
    Nonogram::matrix2D solution = solve_fast(puzzle, false);
    auto stop = std::chrono::high_resolution_clock::now(); 
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start); 
    std::cout << "time to solve: " << (float)duration.count()/1000 << "s" << std::endl; 

    puzzle->set_grid(solution);

    puzzle->show_grid(puzzle->get_grid(), false, false, false); //show_instructions, to_screen, to_file
    std::cout << "Is valid solution: " << puzzle->is_valid_grid(puzzle->get_grid()) << std::endl;

}


void solve_small_nonograms(std::string file_name)
{   
    std::ifstream infile(file_name); //note that non_file_reader.h references the appropriate libraries
    std::string line;
    std::vector<int> runs;
    std::stringstream ss;

    while (std::getline(infile, line)){
        std::vector<std::vector<std::vector<int>>> both_runs {{}, {}};
        // Parse the rows and columns
        for (int k{0}; k<2; ++k){
            std::getline(infile, line); // read the next line
            ss = std::stringstream (line); //convert to stream
            runs = {};  
            for (char i; ss >> i;) { 
                runs.push_back(i -'A' + 1); 
                if (ss.peek() == ' '){
                    both_runs[k].push_back(runs);
                    runs = {}; 
                }
            } 
            if (!runs.empty()){
                both_runs[k].push_back(runs);
            }
        }
        solve_nonogram(both_runs[0], both_runs[1]);
    }
}

int main()
{   
   //nonogram_basics();
   //nfa_tester();
   Runs r = read_non_file("puzzles/lost.txt");
   solve_nonogram(r.runs_row, r.runs_col);
   //solve_small_nonograms("activity_workshop_puzzles.txt");

    return 0;
}
