/*
 *  Created on: 22 October 2020
 *      Author: Lior Sinai
 * Description:
 */

#include "Nonogram.h"

Nonogram::Nonogram(std::vector<std::vector<int>> runs_row_, std::vector<std::vector<int>> runs_col_)
{
    runs_row = runs_row_;
    runs_col = runs_col_;
    n_rows = runs_row.size();
    n_cols = runs_col.size();

    for (int i{0}; i < n_rows; i++)
    {
        std::vector<int> row;
        for (int j{0}; j < n_cols; j++)
        {
            row.push_back(EITHER);
        }
        grid.push_back(row);
    }
}

std::vector<int> Nonogram::get_column(std::vector<std::vector<int>> &grid, int j)
{
    std::vector<int> column;
    for (int i{0}; i < n_rows; i++)
    {
        column.push_back(grid[i][j]);
    }
    return column;
}

int max_length(std::vector<std::vector<int>> array)
{
    int length{0};
    for (auto vec : array)
    {
        if (vec.size() > length)
        {
            length = vec.size();
        }
    }
    return length;
}

std::string run_to_string(int run, int pad)
{
    std::string s = std::to_string(run);
    std::string s_padded = std::string(pad - s.length(), '0') + s; // pad with zeros
    return s_padded;
}

std::vector<std::vector<std::string>> Nonogram::make_box(std::vector<std::vector<int>> &grid, std::string symbols, int repeats)
{
    /* make a box which includes the runs for the Nonogram */
    std::vector<std::vector<std::string>> box;
    int row_max_length = max_length(Nonogram::runs_row);
    int col_max_length = max_length(Nonogram::runs_col);
    int npad = repeats + 1;
    std::string dummy = std::string(repeats, symbols[DUMMY]) + " ";

    // column runs
    for (int i{0}; i < col_max_length; ++i)
    {
        std::vector<std::string> row;
        for (int j{0}; j < row_max_length + n_cols; ++j)
        {
            if (j >= row_max_length && runs_col[j - row_max_length].size() >= col_max_length - i)
            {
                auto runs = runs_col[j - row_max_length];
                int r = runs[runs.size() - (col_max_length - i)];
                std::string s = run_to_string(r, npad);
                row.push_back(s);
            }
            else
            {
                row.push_back(dummy); 
            }
        }
        box.push_back(row);
    }
    for (int i{0}; i < n_rows; ++i)
    {
        // rows runs
        std::vector<std::string> row;
        for (int j{0}; j < row_max_length; ++j)
        {
            if (runs_row[i].size() >= row_max_length - j)
            {
                auto runs = runs_row[i];
                int r = runs[runs.size() - (row_max_length - j)];
                std::string s = run_to_string(r, npad);
                row.push_back(s);
            }
            else
            {
                row.push_back(dummy); 
            }
        }
        // grid
        for (int j{0}; j < n_cols; ++j)
        {
            std::string s(repeats, symbols[grid[i][j]]);
            row.push_back(s + " ");
        }
        box.push_back(row);
    }

    return box;
}

std::string Nonogram::print_grid(matrix2D &grid, bool show_runs, std::string symbols)
{
    std::string string_out = "";
    int repeats = 1;
    if (show_runs)
    {
        std::vector<std::vector<std::string>> box = make_box(grid, symbols, repeats);
        for (int i{0}; i < box.size(); i++)
        {
            for (int j{0}; j < box[0].size(); j++)
            {
                string_out = string_out + box[i][j]+ " ";
            }
            string_out = string_out + "\n";
        }
    }
    else
    {
        for (int i{0}; i < n_rows; i++)
        {
            for (int j{0}; j < n_cols; j++)
            {
                std::string s(repeats, symbols[grid[i][j]]);
                string_out = string_out + s + " ";
            }
            string_out = string_out + "\n";
        }
    }
    return string_out;
}

std::vector<int> Nonogram::get_sequence(std::vector<int> &line)
{
    std::vector<int> sequence;
    bool on_blank = true;
    for (int cell : line)
    {
        if (cell == cell_types::BOX && on_blank)
        {
            sequence.push_back(1);
            on_blank = false;
        }
        else if (cell == cell_types::BOX && !on_blank)
        {
            sequence.back() += 1;
        }
        else
        { // BLANK or EITHER
            on_blank = true;
        }
    }
    return sequence;
}

bool Nonogram::is_valid_line(std::vector<int> &line, std::vector<int> &target_runs)
{
    std::vector<int> sequence = get_sequence(line);
    return sequence == target_runs;
}

bool Nonogram::is_valid_grid(matrix2D &grid)
{
    for (int i{0}; i < n_rows; i++)
    {
        if (!is_valid_line(grid[i], runs_row[i]))
        {
            return false;
        }
    }
    for (int j{0}; j < n_cols; j++)
    {
        if (!this->is_valid_line(get_column(grid, j), this->runs_col[j]))
        {
            return false;
        }
    }
    return true;
}