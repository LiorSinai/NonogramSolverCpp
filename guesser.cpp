#include "guesser.h"


bool in_bounds(int i, int j, int n_rows, int n_cols){
    return ((i >= 0) && (i < n_rows) && (j >= 0) && (j < n_cols));
}

bool on_edge(int idx, int n){
    return (idx == 0) || (idx == n- 1);
}


std::vector< std::vector<int> > rank_solved_neighbours(std::vector< std::vector<int> > grid){
    int n_rows = grid.size();
    int n_cols = grid[0].size();
    int min_rank = 2; 
    std::vector< std::vector<int> > rankings;

    std::vector< std::vector<int> > neighbours;
    int rank{0};
    int i; int j;
    int i_n; int j_n; 
    for (int idx{0}; idx < n_rows * n_cols; ++idx){
        i = idx/n_cols; j = idx % n_cols;
        if (grid[i][j] == EITHER){
            rank = 0;
            neighbours = {{i - 1, j}, {i + 1, j}, {i, j - 1}, {i, j + 1}};
            // increase rank for solved neighbours
            for (const auto &ij: neighbours){
                i_n = ij[0]; j_n = ij[1];
                if (in_bounds(i_n, j_n, n_rows, n_cols) && (grid[i_n][j_n] != EITHER)){
                    ++ rank;
                }
            }
            // edges count as solved
            if (on_edge(i, n_rows)){ ++ rank;};
            if (on_edge(j, n_cols)){ ++ rank;};
            
            if (rank >= min_rank){
                rankings.push_back(std::vector<int>{rank, i, j});
            }
        }
    }
    std::sort(rankings.begin(), rankings.end(),
          [](const std::vector<int>& a, const std::vector<int>& b) {return a[0] > b[0];});

    return rankings;
}

std::vector<int> rank_max_neighbours(std::vector< std::vector<int> > grid){
    int n_rows = grid.size();
    int n_cols = grid[0].size();
    std::vector<int> best_guess {-1, -1, -1};

    std::vector< std::vector<int> > neighbours;
    int rank{0};
    int i; int j;
    int i_n; int j_n; 
    for (int idx{0}; idx < n_rows * n_cols; ++idx){
        int i = idx/n_cols; j = idx % n_cols;
        if (grid[i][j] == EITHER){
            rank = 0;
            neighbours = {{i - 1, j}, {i + 1, j}, {i, j - 1}, {i, j + 1}};
            // increase rank for solved neighbours
            for (const auto &ij: neighbours){
                i_n = ij[0]; j_n = ij[1];
                if (in_bounds(i_n, j_n, n_rows, n_cols) && (grid[i_n][j_n] != EITHER)){
                    ++ rank;
                }
            }
            // edges count as solved
            if (on_edge(i, n_rows)){ ++ rank;};
            if (on_edge(j, n_cols)){ ++ rank;};

            if (rank > best_guess[0]){
                best_guess = {rank, i, j};
            }
            if (best_guess[0] == 4){
                break; // this is the highest ranking possible
            }
        }
    }

    return best_guess;
}