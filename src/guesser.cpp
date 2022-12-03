#include "guesser.h"

bool in_bounds(int i, int j, int n_rows, int n_cols)
{
    return ((i >= 0) && (i < n_rows) && (j >= 0) && (j < n_cols));
}

bool on_edge(int idx, int n)
{
    return (idx == 0) || (idx == n - 1);
}

std::vector<std::vector<int>> get_neighbours(int i, int j)
{
    return {
        {i - 1, j},
        {i + 1, j},
        {i, j - 1},
        {i, j + 1},
    };
}

int count_solved_neighbours(const std::vector<std::vector<int>> &grid, int i, int j)
{
    int count = 0;
    int i_n;
    int j_n;
    int n_rows = grid.size();
    int n_cols = grid[0].size();
    std::vector<std::vector<int>> neighbours = get_neighbours(i, j);
    // increase rank for solved neighbours
    for (const auto &ij : neighbours)
    {
        i_n = ij[0];
        j_n = ij[1];
        if (in_bounds(i_n, j_n, n_rows, n_cols) && (grid[i_n][j_n] != EITHER))
        {
            ++count;
        }
    }
    // edges count as solved
    if (on_edge(i, n_rows))
    {
        ++count;
    };
    if (on_edge(j, n_cols))
    {
        ++count;
    };
    return count;
}

std::vector<Rank<int>> rank_solved_neighbours(const std::vector<std::vector<int>> &grid)
{
    int n_rows = grid.size();
    int n_cols = grid[0].size();
    int min_rank = 2;
    std::vector<Rank<int>> rankings;

    std::vector<std::vector<int>> neighbours;
    int rank{0};
    int i;
    int j;
    int i_n;
    int j_n;
    for (int idx{0}; idx < n_rows * n_cols; ++idx)
    {
        i = idx / n_cols;
        j = idx % n_cols;
        if (grid[i][j] == EITHER)
        {
            rank = count_solved_neighbours(grid, i, j);
            if (rank >= min_rank)
            {
                rankings.push_back(Rank<int>{i, j, rank});
            }
        }
    }
    std::sort(rankings.begin(), rankings.end(),
              [](const Rank<int> &a, const Rank<int> &b)
              { return a.rank > b.rank; });

    return rankings;
}

std::vector<Rank<int>> rank_solved_lines(const std::vector<std::vector<int>> &grid)
{
    int n_rows = grid.size();
    int n_cols = grid[0].size();
    int min_rank = 1;
    std::vector<Rank<int>> rankings;

    std::vector<int> solved_in_column(n_cols, 0);
    std::vector<int> solved_in_row(n_rows, 0);

    int rank{0};
    int i;
    int j;
    for (int idx{0}; idx < n_rows * n_cols; ++idx)
    {
        i = idx / n_cols;
        j = idx % n_cols;
        if (grid[i][j] != EITHER)
        {
            solved_in_column[j] += 1;
            solved_in_row[i] += 1;
        }
    }
    for (int idx{0}; idx < n_rows * n_cols; ++idx)
    {
        i = idx / n_cols;
        j = idx % n_cols;
        if (grid[i][j] == EITHER)
        {
            rank = solved_in_row[i] + solved_in_column[j] + count_solved_neighbours(grid, i, j);
            if (rank >= min_rank)
            {
                rankings.push_back(Rank<int>{i, j, rank});
            }
        }
    }

    std::sort(rankings.begin(), rankings.end(),
              [](const Rank<int> &a, const Rank<int> &b)
              { return a.rank > b.rank; });

    return rankings;
}

Rank<int> rank_max_neighbours(const std::vector<std::vector<int>> &grid)
{
    int n_rows = grid.size();
    int n_cols = grid[0].size();
    Rank<int> best_guess{-1, -1, -1};

    std::vector<std::vector<int>> neighbours;
    int rank{0};
    int i;
    int j;
    int i_n;
    int j_n;
    for (int idx{0}; idx < n_rows * n_cols; ++idx)
    {
        int i = idx / n_cols;
        j = idx % n_cols;
        if (grid[i][j] == EITHER)
        {
            rank = count_solved_neighbours(grid, i, j);

            if (rank > best_guess.rank)
            {
                best_guess = Rank<int>{i, j, rank}; ///{rank, i, j};
            }
            if (best_guess.rank == 4)
            {
                break; // this is the highest ranking possible
            }
        }
    }

    return best_guess;
}