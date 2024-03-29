#include <iostream>
#include <chrono>
#include <algorithm>

#include "Nonogram.h"
#include "matcherNFA.h"
#include "matcher_tester.h"
#include "solver_fast.h"
#include "non_file_reader.h"
#include "writer.h"

void nonogram_basics()
{
    /* demonstrate basic capabilities of the class */
    // elephant-mai
    std::vector<std::vector<int>> r_rows = {{3}, {4, 2}, {6, 6}, {6, 2, 1}, {1, 4, 2, 1}, {6, 3, 2}, {6, 7}, {6, 8}, {1, 10}, {1, 10}, {1, 10}, {1, 1, 4, 4}, {3, 4, 4}, {4, 4}, {4, 4}};
    std::vector<std::vector<int>> r_cols = {{1}, {11}, {3, 3, 1}, {7, 2}, {7}, {15}, {1, 5, 7}, {2, 8}, {14}, {9}, {1, 6}, {1, 9}, {1, 9}, {1, 10}, {12}};
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
        {2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1}};

    std::shared_ptr<Nonogram> puzzle = std::make_shared<Nonogram>(r_rows, r_cols);
    Nonogram::matrix2D solution2 = solve_fast(puzzle, false);

    // puzzle->set_grid(solution);
    puzzle->set_grid(solution2);

    bool show_runs = true;
    std::string printed_grid = puzzle->print_grid(puzzle->get_grid(), show_runs);
    std::cout << printed_grid << std::endl;
    std::cout << "Is complete: " << puzzle->is_valid_grid(puzzle->get_grid()) << std::endl;
}

void nfa_tester()
{
    std::unique_ptr<TestSuite> nfa_tester = std::make_unique<TestSuite>();

    BasicTest *t1 = new BasicTest();
    LancasterExample *t2 = new LancasterExample();
    LongMiddle *t3 = new LongMiddle();
    WorstCase *t4 = new WorstCase();
    VeryLong *t5 = new VeryLong();
    EmptyArrays *t6 = new EmptyArrays();
    ProblemRun *t7 = new ProblemRun();

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

void solve_nonogram(std::vector<std::vector<int>> run_rows, std::vector<std::vector<int>> run_col,
                    bool make_guess = false, bool show_runs = false)
{
    /*Solves a single nonogram puzzle. It can be of any size */
    std::shared_ptr<Nonogram> puzzle = std::make_shared<Nonogram>(run_rows, run_col);

    auto start = std::chrono::high_resolution_clock::now();
    Nonogram::matrix2D solution = solve_fast(puzzle, make_guess);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    puzzle->set_grid(solution);

    std::cout << puzzle->print_grid(puzzle->get_grid(), show_runs) << std::endl;
    std::cout << "time to solve: " << (float)duration.count() / 1000 << "s" << std::endl;
    std::cout << "Is valid solution: " << puzzle->is_valid_grid(puzzle->get_grid()) << std::endl;
}

void solve_collection(std::string file_name,
                      bool make_guess = false, bool show_runs = false)
{
    /*Solves a collection of puzzles encoded in a single file. These puzzles should be all be smaller than 30x30 */

    std::ifstream infile(file_name); // note that non_file_reader.h imports the appropriate libraries
    if (infile.fail())
    {
        std::cout << "reading file " << file_name << " failed";
        return;
    }
    std::string line;
    std::getline(infile, line); // skip the description

    while (std::getline(infile, line))
    {
        Runs runs = read_non_compact_collection(infile);
        solve_nonogram(runs.rows, runs.columns, make_guess, show_runs);
    }
}

char *getCmdOption(char **begin, char **end, const std::string &option)
{
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return (char *)"";
}

bool cmdOptionExists(char **begin, char **end, const std::string &option)
{
    return std::find(begin, end, option) != end;
}

void show_help()
{
    printf("Usage: NonogramSolver by Lior Sinai, 2020/10/31 ");
    printf("\n\nOptions");
    printf("\n--help -h     show usage options");
    printf("\n--test-basics solve a small 15x15 elephant puzzle");
    printf("\n--test-nfa    run the tester for the matching algorithms");
    printf("\n--solve -s    solve a puzzle. Must also include a filename");
    printf("\n--solve-collection -sc solve a collection of puzzles. Must also have a corresponding filename");
    printf("\n--file-path -f     a filename to read");
    printf("\n");
    printf("\nsolving and display options");
    printf("\n--guess -g    make guesses if the line solver gets stuck");
    printf("\n--show-runs -r show the runs (numbers) to the left and on top of the image");
    printf("\n");
    printf("\nTo write to a file, use > e.g. NonogramSolver --solve --file-path puzzle.txt > solution.txt");
    printf("\n");
}

/* cmd line executables */
int main_cmd_executables(int argc, char *argv[])
{
    if (cmdOptionExists(argv, argv + argc, "--help") || cmdOptionExists(argv, argv + argc, "-h") || argc == 1)
    {
        show_help();
    }
    else if (cmdOptionExists(argv, argv + argc, "--test-basics"))
    {
        nonogram_basics();
    }
    else if (cmdOptionExists(argv, argv + argc, "--test-nfa"))
    {
        nfa_tester();
    }
    else
    {
        // get file name
        std::string file_name;
        file_name = getCmdOption(argv, argv + argc, "--file-path");
        if (file_name == "")
        {
            file_name = getCmdOption(argv, argv + argc, "-f");
        }

        // set solving options
        bool make_guess = cmdOptionExists(argv, argv + argc, "--guess") || cmdOptionExists(argv, argv + argc, "-g");
        bool show_runs = cmdOptionExists(argv, argv + argc, "--show-runs") || cmdOptionExists(argv, argv + argc, "-r");

        if (cmdOptionExists(argv, argv + argc, "--solve-collection") || cmdOptionExists(argv, argv + argc, "-sc"))
        {
            if (file_name == "")
            {
                printf("Must include a file with the option -f [--file-path]");
            }
            else
            {
                solve_collection(file_name, make_guess, show_runs);
            }
        }
        if (cmdOptionExists(argv, argv + argc, "--solve") || cmdOptionExists(argv, argv + argc, "-s"))
        {
            if (file_name == "")
            {
                printf("Must include a file with the option -f [--file-path]");
            }
            else
            {
                Runs r = read_non_file(file_name);
                solve_nonogram(r.rows, r.columns, make_guess, show_runs);
            }
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
   //nonogram_basics();
   //nfa_tester();
   //std::string path = "puzzles/lost.txt";
   //std::string path = "jsimlo-puzzles/03 advanced/huskie.sgriddler";
   //std::string path = "jsimlo-puzzles/04 possibilities/lighthouse.sgriddler";
   //Runs r = read_non_file(path);
   //solve_nonogram(r.rows, r.columns, true, false);
   //solve_collection("collections/activity_workshop_puzzles.txt", true, false);
   main_cmd_executables(argc, argv);
   return 0;
}