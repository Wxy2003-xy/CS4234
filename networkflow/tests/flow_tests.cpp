#include "flow.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>
#include <string>

namespace {
void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}
void near(double a, double b, const std::string& message) {
    require(std::fabs(a - b) < 0.002, message);
}
double cut_oracle(const nf::Graph& g) {
    double minimum = std::numeric_limits<double>::max();
    for (unsigned mask = 0; mask < (1u << g.vertices); ++mask) {
        if (!(mask & 1u) || (mask & (1u << (g.vertices - 1)))) continue;
        double cut = 0;
        for (const auto& e : g.edges)
            if ((mask & (1u << e.from)) && !(mask & (1u << e.to))) cut += e.capacity;
        minimum = std::min(minimum, cut);
    }
    return minimum;
}
void check(const nf::Graph& g, double expected) {
    for (auto algorithm : {nf::Algorithm::EdmondsKarp, nf::Algorithm::Dinic}) {
        auto r = nf::solve(g, 0, g.vertices - 1, algorithm);
        near(r.value, expected, "Incorrect maximum flow");
        require(!r.source_side.back(), "Residual augmenting path remains");
        std::vector<double> balance(g.vertices);
        double cut = 0;
        for (std::size_t i = 0; i < g.edges.size(); ++i) {
            const auto& e = g.edges[i];
            require(r.flow[i] >= -nf::epsilon && r.flow[i] <= e.capacity + nf::epsilon, "Capacity violation");
            near(r.flow[i] * 100, std::round(r.flow[i] * 100), "Flow lost hundredths precision");
            balance[e.from] -= r.flow[i]; balance[e.to] += r.flow[i];
            if (r.source_side[e.from] && !r.source_side[e.to]) cut += e.capacity;
        }
        for (int v = 1; v < g.vertices - 1; ++v) near(balance[v], 0, "Flow conservation failed");
        near(-balance.front(), r.value, "Source balance failed");
        near(balance.back(), r.value, "Sink balance failed");
        near(cut, r.value, "Max-flow/min-cut mismatch");
    }
}
template<class F> void rejects(F f) {
    bool caught = false;
    try { f(); } catch (const std::invalid_argument&) { caught = true; }
    require(caught, "Invalid input was accepted");
}
} // namespace

int main() {
    try {
        check({6, {{0,1,16},{0,2,13},{1,2,10},{2,1,4},{1,3,12},{3,2,9},{2,4,14},{4,3,7},{3,5,20},{4,5,4}}}, 23);
        check({4, {{0,1,0.3f},{0,2,0.2f},{1,2,0.1f},{1,3,0.2f},{2,3,0.3f}}}, 0.5);
        check({3, {{0,1,5}}}, 0);
        check({2, {{0,1,0.01f},{1,0,5},{0,1,0.02f}}}, 0.03);
        check({2, {{0,1,0}}}, 0);
        // The first shortest path uses 1->3. Reaching flow 2 requires cancelling it.
        check({6, {{0,1,1},{0,2,1},{1,3,1},{1,4,1},{2,3,1},{3,5,1},{4,5,1}}}, 2);
        require(nf::edges_for_density(8, 0) == 7, "Sparse density endpoint");
        require(nf::edges_for_density(8, 1) == 56, "Dense density endpoint");
        require(nf::edges_for_density(8, 0.5f) == 32, "Density rounding");
        for (int n = 2; n <= 8; ++n) {
            for (unsigned seed = 0; seed < 60; ++seed) {
                for (bool decimal : {false, true}) {
                    int count = nf::edges_for_density(n, (seed % 11) / 10.0f);
                    auto graph = nf::generate(n, count, decimal, seed);
                    require(static_cast<int>(graph.edges.size()) == count, "Wrong generated edge count");
                    std::set<std::pair<int,int>> edges;
                    for (const auto& e : graph.edges) {
                        require(e.from != e.to && edges.insert({e.from,e.to}).second, "Loop or duplicate edge");
                        require(e.capacity > 0 && e.capacity <= 20, "Generated capacity range");
                        if (!decimal) near(e.capacity, std::round(e.capacity), "Noninteger capacity");
                    }
                    auto repeated = nf::generate(n, count, decimal, seed);
                    for (std::size_t i = 0; i < graph.edges.size(); ++i) {
                        require(graph.edges[i].from == repeated.edges[i].from && graph.edges[i].to == repeated.edges[i].to &&
                                graph.edges[i].capacity == repeated.edges[i].capacity, "Seed not reproducible");
                    }
                    double expected = cut_oracle(graph);
                    require(expected > 0, "Generated graph lacks source-to-sink path");
                    check(graph, expected);
                }
            }
        }
        rejects([] { nf::generate(1, 0, false, 0); });
        rejects([] { nf::generate(4, 2, false, 0); });
        rejects([] { nf::generate(4, 13, false, 0); });
        rejects([] { nf::edges_for_density(4, -0.1f); });
        rejects([] { nf::edges_for_density(4, std::numeric_limits<float>::quiet_NaN()); });
        rejects([] { nf::solve({2, {{0,1,0.001f}}}, 0, 1, nf::Algorithm::Dinic); });
        rejects([] { nf::solve({2, {}}, 0, 0, nf::Algorithm::Dinic); });
        std::cout << "Passed: known flows, cancellation, fractions, validation, and 840 random graphs\n";
    } catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
}
