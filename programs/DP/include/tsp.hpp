#pragma once

#include <bits/stdc++.h>
#include "_graph.hpp"

int exact_tsp(dp::graph::Graph g, int src) {
    std::vector<std::vector<int>> dp(g.V, std::vector<int>(exp2(g.V), INFINITY));
    for (int i = 0; i < g.V; i++) {
        if (i == src) {
            continue;
        } else {
            // starting from 3 to i (2)
            // 00000001 << 3 | 00000001 << 2 = 00001100
            dp[i][1 << src | 1 << i] = g.adj[src][i];
        }
    }
    
}
