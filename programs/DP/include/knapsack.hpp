#pragma once

#include <bits/stdc++.h>
using namespace std;

class KnapsackSolver {
public:
    KnapsackSolver(vector<array<int, 2>> items, int W)
        : items(std::move(items)),
          W(W),
          n(static_cast<int>(this->items.size())) {
        get_max_value();
    }

    int solve_topdown() {
        init_dp_capacity_param();
        const int cols = W + 1;

        for (int i = 1; i <= n; ++i) {
            const int value = items[i - 1][0];
            const int weight = items[i - 1][1];

            for (int capacity = 0; capacity <= W; ++capacity) {
                dp[i * cols + capacity] =
                    dp[(i - 1) * cols + capacity];

                if (weight <= capacity) {
                    dp[i * cols + capacity] = max(
                        dp[i * cols + capacity],
                        dp[(i - 1) * cols + capacity - weight] + value
                    );
                }
            }
        }

        return dp[n * cols + W];
    }

    int solve_value_parameterized() {
        init_dp_value_param();
        return -1;
    }

    ~KnapsackSolver() {
        free(dp);
    }

private:
    vector<array<int, 2>> items;
    int W;
    int n;
    int max_value;
    int* dp = NULL;

    void init_dp_capacity_param() {
        free(dp);
        const size_t rows = static_cast<size_t>(n + 1);
        const size_t cols = static_cast<size_t>(W + 1);
        dp = static_cast<int*>(calloc(rows * cols, sizeof(*dp)));
        if (dp == NULL) {
            exit(0);
        }
    }

    void get_max_value() {
        max_value = -1;
        for (const auto& item : items) {
            max_value = max(max_value, item[0]);
        }
    }

    void init_dp_value_param() {
        free(dp);
        get_max_value();
        int* alloc = (int*)malloc(sizeof(int) * n * n * max_value);
        if (alloc == NULL) {
            exit(0);
        }
        dp = alloc;
    }
};
