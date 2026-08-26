#pragma once

#include <bits/stdc++.h>
using namespace std;

class KnapsackSolver {
public:
    KnapsackSolver(vector<array<int, 2>> items, int W) {
        this->items = items;
        this->W = W;
        this->n = items.size();
        get_max_value();
    }
    int solve_topdown() {
        init_dp();
        for (int i = 1; i <= n; ++i) {
            for (int j = 0; j <= W; j++) {
                const int v = items[i-1][0];
                const int w = items[i-1][1];
                dp[i * W + j] = dp[(i - 1) * W + j];
                if (w <= j) {
                    dp[i * W + j] = max(dp[(i - 1) * W + j + w] + v, dp[(i - 1) * W + j]);
                }
            }
        }
        return dp[n * W + W];
    }

    ~KnapsackSolver() {
        free(this->dp);
    }
private:
    vector<array<int, 2>> items;
    int W;
    int n;
    int max_value;
    int* dp = NULL;
    void init_dp() {
        free(dp);
        int* alloc = (int*)malloc(sizeof(int) * n * W);
        if (alloc == NULL) {
            exit(0);
        }
        dp = alloc;
    }
    void get_max_value() {
        this->max_value = -1;
        for (const auto& p : this->items) {
            this->max_value = max(this->max_value, p[0]);
        }
    }

};