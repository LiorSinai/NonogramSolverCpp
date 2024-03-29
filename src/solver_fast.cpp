/*
 *  Created on: 23 October, 2020
 *      Author: Lior Sinai
 * Description: Fast solver for the nonogram.
 * See:
 * http://scc-forge.lancaster.ac.uk/open/nonogram/ls-fast
 * https://webpbn.com/pbnsolve.html
 */

#include "solver_fast.h"

namespace
{
    int guesses{0};
    int probes{0};
    std::unique_ptr<NonDeterministicFiniteAutomation> nfa = std::make_unique<NonDeterministicFiniteAutomation>();
}

Nonogram::matrix2D solve_fast(std::shared_ptr<Nonogram> puzzle, bool make_guess = true)
{
    std::cout << "solving puzzle ..." << std::endl;
    if (make_guess)
    {
        std::cout << "#guess progress" << std::endl;
    }
    Nonogram::matrix2D grid = solve_fast_(puzzle->get_grid(), puzzle, make_guess);
    float progress = get_progress(grid);
    std::cout << printf("%.3lf", 100 * progress) << "% complete" << std::endl;
    nfa->cache->info();
    printf("%d probes\n", probes);
    return grid;
}

float get_progress(Nonogram::matrix2D &grid)
{
    float n_rows = (float)grid.size();
    float n_cols = (float)grid[0].size();
    float count{0.0};
    for (auto r = grid.begin(); r != grid.end(); r++)
    {
        std::vector<int> row = *r;
        for (auto c = row.begin(); c != row.end(); c++)
        {
            if (*c != EITHER)
            {
                count += 1;
            }
        }
    }
    return count / (n_rows * n_cols);
}

Nonogram::matrix2D solve_fast_(Nonogram::matrix2D grid, std::shared_ptr<Nonogram> puzzle, bool make_guess,
                               std::set<int> rows_to_edit, std::set<int> columns_to_edit)
{
    // extract values from Nonogram object
    int n_rows = puzzle->get_n_rows();
    int n_cols = puzzle->get_n_cols();
    std::vector<std::vector<int>> runs_row = puzzle->get_runs_row();
    std::vector<std::vector<int>> runs_col = puzzle->get_runs_col();

    // initialise
    int sweeps{1};
    if (rows_to_edit.empty() && columns_to_edit.empty())
    {
        for (int j{0}; j < n_cols; j++)
        {
            columns_to_edit.insert(j);
        }

        for (int i{0}; i < n_rows; i++)
        {
            fix_row(grid, runs_row[i], columns_to_edit, i);
        }
    }

    float progress = 0;
    while (progress < 1)
    {
        // constraint propagation
        while (!columns_to_edit.empty())
        {
            for (int j : columns_to_edit)
            {
                fix_col(grid, runs_col[j], rows_to_edit, j);
            }
            columns_to_edit.clear();
            for (int i : rows_to_edit)
            {
                fix_row(grid, runs_row[i], columns_to_edit, i);
            }
            rows_to_edit.clear();
            sweeps += 2;
        }
        progress = get_progress(grid);
        if (!make_guess)
        {
            if (guesses == 0)
            {
                std::cout << "constraint propagation done in " << sweeps << " sweeps" << std::endl;
            }
            break;
        }

        // make guess
        if ((progress < 1) && make_guess)
        {
            ++guesses;
            Guess best_guess = probe(puzzle, grid);
            int i = best_guess.i;
            int j = best_guess.j; // guess on the highest ranked element
            std::cout << printf("%d %.3lf", guesses - 1, 100 * progress) << "%";
            if (best_guess.values.size() == 1)
            { // a contradiction was found
                std::cout << std::endl;
                grid[i][j] = best_guess.values[0];
                rows_to_edit = {i};
                columns_to_edit = {j};
                // go back to constraint propagation
            }
            else
            {
                for (const auto &x : {BOX, BLANK})
                {
                    try
                    {
                        std::cout << " guess" << std::endl;
                        Nonogram::matrix2D grid_next = grid;
                        grid_next[i][j] = x;
                        grid_next = solve_fast_(grid_next, puzzle, true, {i}, {j});
                        if (get_progress(grid_next) >= 1)
                        {
                            return grid_next;
                        }
                    }
                    catch (SolverError &e)
                    {
                    }
                }
            }
        }
    }
    return grid;
}

Guess probe(std::shared_ptr<Nonogram> puzzle, Nonogram::matrix2D grid)
{
    /* Try many guesses and look for contradictions.
     * The opposite of contradictions are definitely valid and can be returned immediately.
     * Else go with the guess which solves the most cells
     */
    std::vector<Rank<int>> rankings = rank_solved_lines(grid);
    Guess best_guess;
    float max_solve{0};
    float progress;
    Nonogram::matrix2D grid_next;
    for (const auto &guess : rankings)
    {
        int rank = guess.rank;
        int i = guess.i;
        int j = guess.j;
        for (const auto &x : {BOX, BLANK})
        {
            ++probes;
            try
            {
                grid_next = grid;
                grid_next[i][j] = x;
                grid_next = solve_fast_(grid_next, puzzle, false, {i}, {j});
            }
            catch (SolverError &e)
            { // a contradiction was found :)
                best_guess.i = i;
                best_guess.j = j;
                best_guess.rank = rank;
                best_guess.values = {x ^ 3}; // the opposite color is definitely correct
                return best_guess;
            }
            progress = get_progress(grid_next);
            if (progress >= 1.0)
            { // solved the puzzle
                best_guess.i = i;
                best_guess.j = j;
                best_guess.rank = rank;
                best_guess.values = {x};
                best_guess.progress = 1.0;
                return best_guess;
            }
            else if (progress > max_solve)
            {
                best_guess.i = i;
                best_guess.j = j;
                best_guess.rank = rank;
                best_guess.values = {BOX, BLANK}; // either might still be correct
            }
        }
    }
    return best_guess;
}

void fix_row(Nonogram::matrix2D &grid, std::vector<int> runs, std::set<int> &columns_to_edit, int i)
{
    std::vector<int> *row = &grid[i];
    int n_cols = row->size();
    std::vector<int> allowed = apply_strategies(*row, runs);
    for (int j{0}; j < n_cols; ++j)
    {
        if (((*row)[j] != allowed[j]) && (allowed[j] != EITHER))
        {
            columns_to_edit.insert(j);
            grid[i][j] = allowed[j];
        }
    }
}

void fix_col(Nonogram::matrix2D &grid, std::vector<int> runs, std::set<int> &rows_to_edit, int j)
{
    std::vector<int> col = get_column(grid, j);
    int n_cols = col.size();
    std::vector<int> allowed = apply_strategies(col, runs);
    for (int i{0}; i < n_cols; ++i)
    {
        if ((col[i] != allowed[i]) && (allowed[i] != EITHER))
        {
            rows_to_edit.insert(i);
            grid[i][j] = allowed[i];
        }
    }
}

std::vector<int> get_column(Nonogram::matrix2D &grid, int j)
{
    std::vector<int> column;
    for (int i{0}; i < (int)grid.size(); i++)
    {
        column.push_back(grid[i][j]);
    }
    return column;
}

std::vector<int> apply_strategies(std::vector<int> &line, std::vector<int> &runs)
{
    std::vector<int> allowed(line.size(), EITHER);
    std::vector<int> allowed1 = left_rightmost_overlap(line, runs);
    std::vector<int> allowed2 = simple_filler(line, runs);
    std::vector<int> line_r = line;
    std::vector<int> runs_r = runs_r;
    std::reverse(line_r.begin(), line_r.end());
    std::reverse(runs_r.begin(), runs_r.end());
    std::vector<int> allowed3 = simple_filler(line_r, runs_r);
    std::reverse(allowed3.begin(), allowed3.end());
    for (int i{0}; i < line.size(); i++)
    {
        allowed[i] = allowed[i] & allowed1[i];
        allowed[i] = allowed[i] & allowed2[i];
        allowed[i] = allowed[i] & allowed3[i];
    }
    return allowed;
}

// std::vector<int> apply_strategies(std::vector<int>& line, std::vector<int> &runs){
//     /* split and apply strategies*/
//     std::vector<std::pair<std::vector<int>, std::vector<int>>> segments = splitter(line, runs);
//     std::vector<int> allowed_full;
//     for (auto & line_run: segments){
//         if (line_run.first.empty()){
//             continue;
//         }
//         std::vector<int> line_seg = line_run.first;
//         std::vector<int> runs_seg = line_run.second;
//         std::vector<int> allowed(line_seg.size(), EITHER);
//         std::vector<int> allowed1 = left_rightmost_overlap(line_seg, runs_seg);
//         std::vector<int> allowed2 = simple_filler(line_seg, runs_seg);
//         std::reverse(line_seg.begin(), line_seg.end());
//         std::reverse(runs_seg.begin(), runs_seg.end());
//         std::vector<int> allowed3 = simple_filler(line_seg, runs_seg);
//         std::reverse(allowed3.begin(), allowed3.end());
//         for (int i{0}; i < line_seg.size(); i ++){
//             allowed[i] = allowed[i] & allowed1[i];
//             allowed[i] = allowed[i] & allowed2[i];
//             allowed[i] = allowed[i] & allowed3[i];
//         }
//         allowed_full.insert(allowed_full.end(), allowed.begin(), allowed.end());
//     }
//     return allowed_full;
// }

bool all_unknown(std::vector<int> &line)
{
    for (const auto x : line)
    {
        if (x != EITHER)
        {
            return false;
        }
    }
    return true;
}

std::vector<int> left_rightmost_overlap(std::vector<int> &line, std::vector<int> &runs)
{
    Match m_left;
    Match m_right;
    // for right
    std::vector<int> line_r = line;
    std::vector<int> runs_r = runs;
    std::reverse(line_r.begin(), line_r.end());
    std::reverse(runs_r.begin(), runs_r.end());
    if (all_unknown(line))
    {
        // use minimum match
        m_left = minumum_match(line, runs);
        m_right = minumum_match(line_r, runs_r);
    }
    else
    {
        //  left most
        nfa->compile(runs);
        m_left = nfa->find_match(line);
        // right most
        nfa->compile(runs_r);
        m_right = nfa->find_match(line_r);
    }
    std::reverse(m_right.match.begin(), m_right.match.end());

    if (m_left.is_match && m_right.is_match)
    {
        std::vector<int> allowed = overlap(m_left.match, m_right.match);
        return allowed;
    }
    else
    {
        throw SolverError();
    }
}

std::vector<int> overlap(std::vector<int> &a, std::vector<int> &b)
{
    /* return the overlap between 2 vectors a and b
     *  There is an overlap if the numbers in each sequence are equal
     *  Even numbers (and zero) are BLANK and odd numbers are BOX
     *  http://scc-forge.lancaster.ac.uk/open/nonogram/ls-fast
     */
    assert(a.size() == b.size());
    std::vector<int> out(a.size(), EITHER);
    std::vector<int> changer_a = changer_sequence(a);
    std::vector<int> changer_b = changer_sequence(b);

    for (int i{0}; i < (int)a.size(); i++)
    {
        int x = changer_a[i];
        if (x == changer_b[i])
        {
            if ((x + 2) % 2 == 0)
            {
                out[i] = BLANK;
            }
            else
            {
                out[i] = BOX;
            }
        }
    }
    return out;
}

std::vector<int> changer_sequence(std::vector<int> &line)
{
    int counter = (int)line[0] == BOX;
    int prev = line[0];
    std::vector<int> out(line.size(), 0);
    out[0] = counter;
    for (int i{1}; i < out.size(); i++)
    {
        counter += (int)(prev != line[i]);
        out[i] = counter;
        prev = line[i];
    }
    return out;
}

std::vector<int> simple_filler(std::vector<int> &line, std::vector<int> &runs)
{
    /* easy to spot boxes and blank that the left-right matcher might miss*/
    int k{0};
    int on_run{0};
    std::vector<int> allowed = line; // copy
    for (int i{0}; i < (int)line.size(); ++i)
    {
        int x = line[i];
        if (x == BLANK)
        {
            if (on_run > 0)
            {
                ++k; // move to the next pattern
            }
            on_run = 0;
        }
        else if (x == BOX)
        {
            ++on_run;
        }
        else
        { // x==EITHER
            if (k >= (int)runs.size())
            {
                break;
            }
            else if ((0 < on_run) && (on_run < runs[k]))
            { // must be a gap in a run
                allowed[i] = BOX;
                ++on_run;
            }
            else if ((on_run == 0) && (k > 0) && (i > 0) && (line[i - 1] == BOX))
            { // this must be a BLANK ending a run
                allowed[i] = BLANK;
            }
            else
            {
                break; // too many unknowns
            }
        }
    }
    return allowed;
}

Match minumum_match(std::vector<int> &line, std::vector<int> &runs)
{
    // build minimum match: [BOX]*x + [BLANK] + [BOX]*x + [BLANK] + ... + [BLANK]
    if (runs.empty())
    {
        return Match{std::vector<int>(line.size(), BLANK), {}, true};
    }
    Match m{{}, runs, true};
    std::vector<int> &match = m.match;
    for (const int &r : runs)
    {
        std::vector<int> run(r, BOX);
        match.insert(match.end(), run.begin(), run.end());
        match.push_back(BLANK);
    }
    match.pop_back(); // last blank might not be needed
    std::vector<int> trailing_zeros(line.size() - match.size(), BLANK);
    match.insert(match.end(), trailing_zeros.begin(), trailing_zeros.end());
    return m;
}

std::pair<std::vector<int>, std::vector<std::pair<int, int>>> get_sequence(std::vector<int> &line)
{
    std::vector<int> sequence;
    std::vector<std::pair<int, int>> positions;
    bool on_blank = true;
    for (int i{0}; i < (int)line.size(); ++i)
    {
        int cell = line[i];
        if ((cell == cell_types::BOX) && on_blank)
        {
            sequence.push_back(1);
            positions.push_back(std::make_pair(i, i));
            on_blank = false;
        }
        else if ((cell == cell_types::BOX) && !on_blank)
        {
            sequence.back() += 1;
            positions.back().second += 1;
        }
        else
        {
            on_blank = true;
        }
    }
    return std::make_pair(sequence, positions);
}

std::vector<std::pair<std::vector<int>, std::vector<int>>> splitter(std::vector<int> &line, std::vector<int> &runs)
{
    /* split a line into smaller segments at the maximum runs */
    if (line.empty() || runs.empty())
    {
        return {std::make_pair(line, runs)};
    }
    auto sequence = get_sequence(line);
    std::vector<int> &runs_ = sequence.first;
    std::vector<std::pair<int, int>> &positions = sequence.second;
    // get split value and check if unique in runs and in runs_
    auto split_value = std::max_element(runs.begin(), runs.end());
    if ((std::count(runs.begin(), runs.end(), *split_value) != 1) ||
        (std::count(runs_.begin(), runs_.end(), *split_value) != 1))
    {
        return {std::make_pair(line, runs)};
    }
    // get split idx in runs
    int split_idx = split_value - runs.begin();
    int split_idx_ = std::find(runs_.begin(), runs_.end(), *split_value) - runs_.begin();
    // split
    std::vector<std::pair<std::vector<int>, std::vector<int>>> splits;
    auto pos = positions[split_idx_];
    // add whites on either side
    int i0 = std::max(pos.first - 1, 0);
    int i1 = std::min(pos.second + 1, (int)line.size() - 1);
    // recursive call on left
    // std::vector<std::pair<std::vector<int>, std::vector<int>>> left;
    std::vector<int> &left_line = std::vector<int>(line.begin(), line.begin() + i0);
    std::vector<int> &left_runs = std::vector<int>(runs.begin(), runs.begin() + split_idx - 1 + 1);
    std::vector<std::pair<std::vector<int>, std::vector<int>>> left = splitter(left_line, left_runs);
    // middle
    std::vector<int> &middle_line = std::vector<int>(line.begin() + i0, line.begin() + i1 + 1);
    std::vector<int> middle_run = {*split_value};
    // recursive call on right
    std::vector<int> &right_line = std::vector<int>(line.begin() + i1 + 1, line.end());
    std::vector<int> &right_runs = std::vector<int>(runs.begin() + split_idx + 1, runs.end());
    std::vector<std::pair<std::vector<int>, std::vector<int>>> right = splitter(right_line, right_runs);
    // join all together
    splits.insert(splits.begin(), left.begin(), left.end());
    splits.push_back(make_pair(middle_line, middle_run));
    splits.insert(splits.end(), right.begin(), right.end());
    return splits;
}
