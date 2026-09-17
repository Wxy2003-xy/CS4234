#pragma once

#include <bits/stdc++.h>
using namespace std;

static void print_2D_dp(vector<vector<unsigned long long>>& dp) {
    for (int i = 0; i < dp.size(); i++) {
        for (int j = 0; j < dp[0].size(); j++) {
            cout << dp[i][j] << " ";
        }
        cout << endl;
    }
}


int numDistinct(string s, string t) {
    int n = s.length();
    int m = t.length();
    vector<vector<unsigned long long>> dp(n+1, vector<unsigned long long>(m+1, 0));
    for (int i = 0; i <= n; i++) {
        dp[i][0] = 1;
    }

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            int sidx = i-1;
            int tidx = j-1;
            if (i < j) {
                dp[i][j] = 0;
            }
            else if (s[sidx] == t[tidx]) {
                dp[i][j] = dp[i - 1][j] + dp[i - 1][j - 1];
            } else {
                dp[i][j] = dp[i - 1][j];
            }
        }
    }
    // print_2D_dp(dp);
    return dp[n][m];
}


int distinctSubseqII(string s) {
    int n = s.length();

    vector<vector<unsigned long long>> dp(n, vector<unsigned long long>(n, 0));
    for (int i = 0; i < n; i++) {
        for (int j = n-1; j >= 0; j--) {
            if (i == j) {
                dp[i][j] = 1;
            } else if (i < j || j == n - 1 || i == 0) {
                dp[i][j] = 0;
            } else {
                dp[i][j] = dp[i - 1][j] + dp[i][j + 1];
                if (s[i] != s[j]) {
                    dp[i][j] = dp[i][j] + 1;
                }
            }
        }
    }
    print_2D_dp(dp);
    return dp[n - 1][0];
}