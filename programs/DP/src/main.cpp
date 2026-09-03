#include <bits/stdc++.h>

#include "knapsack.hpp"
#include "regexMatch.hpp"

using namespace std;

namespace {

void run_knapsack() {
    vector<array<int, 2>> items = {
        {2, 3},
        {1, 2},
        {5, 4},
        {3, 3},
        {6, 7}
    };

    KnapsackSolver solver(items, 12);
    cout << "Knapsack maximum value: "
         << solver.solve_topdown() << '\n';
}

void run_regex_match() {
    RegexMatcher solver;
    const string text = "aa";
    const string pattern = "a*";

    cout << "Regex DP table:\n";
    int result = solver.match(text, pattern);
    cout << "Regex match result: " << result << '\n';
}

void print_usage(const char* executable) {
    cerr << "Usage: " << executable
         << " [all|knapsack|regex]\n";
}

} // namespace

int main(int argc, char* argv[]) {
    const string problem = argc > 1 ? argv[1] : "all";

    if (problem == "all" || problem == "knapsack") {
        run_knapsack();
    }

    if (problem == "all" || problem == "regex") {
        run_regex_match();
    }

    if (problem != "all"
        && problem != "knapsack"
        && problem != "regex") {
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}
