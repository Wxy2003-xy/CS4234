#include<bits/stdc++.h>
#include"knapsack.hpp"
using namespace std;

int main() {
    vector<array<int, 2>> items = {{2, 3}, {1, 2}, {5, 4}, {3, 3}, {6, 7}};
    int w = 12;
    KnapsackSolver* s = new KnapsackSolver(items, w);
    int res = s->solve_topdown();
    cout << res << endl;
}