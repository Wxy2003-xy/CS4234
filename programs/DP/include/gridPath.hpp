#pragma once

#include <bits/stdc++.h>

using namespace std;

// given a grid of size nxm, each cell has a weight. starting from any top row cell to any bottom row cell, one can only
// walk directly down, or down left, or down right. but you can only walk down left or down right total at most k times. 
// no limit to directly walk down. find the minimum total cost path. 


int find_min_path(vector<vector<int>>& g, int k) {
    int n = g.size();
    int m = g[0].size();
    vector<vector<vector<int>>> dp(n+1, vector<vector<int>>(m, vector<int>(k, INFINITY)));
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < k; j++) {
            dp[0][i][j] = g[i][j];
        }
    }
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j < m; j++) {
            for (int t = 0; t < k; t++) {
                if (t == 0) {
                    dp[i][j][0] = dp[i-1][j][0] + g[i][j];
                } else if (j == 0) {
                    dp[i][j][t] = min(dp[i-1][j][t], dp[i-1][j+1][t-1]) + g[i][j];
                } else if (j == m - 1) {
                    dp[i][j][t] = min(dp[i-1][j][t], dp[i-1][j-1][t-1]) + g[i][j];
                } else {
                    dp[i][j][t] = min(min(dp[i-1][j][t], dp[i-1][j-1][t-1]), dp[i-1][j+1][t-1]) + g[i][j];
                }
            }
        }
    }

    int res = INFINITY;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < k; j++) {
            res = min(res, dp[n-1][i][j]);
        }
    }    
    return res;
}
