#include "flow.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <numeric>
#include <queue>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>

namespace nf {
    float hundredths(float value) { return std::round(value * 100.0f) / 100.0f; }

    namespace {
        void check_vertices(int n) {
            if (n < 2 || n > 1000) throw std::invalid_argument("Vertex count must be between 2 and 1000.");
        }
        int maximum_generated_edges(int n, bool simple) {
            // No edges enter source 0 or leave sink n-1. In simple mode,
            // every unordered pair still has at least one allowed orientation.
            return simple ? n * (n - 1) / 2 : (n - 1) * (n - 2) + 1;
        }
        struct Arc { int to, reverse; float residual; };
        using Residual = std::vector<std::vector<Arc>>;
        void augment(Residual& residual, int from, int index, float amount) {
            Arc& arc = residual[from][index];
            Arc& reverse = residual[arc.to][arc.reverse];
            // Capacities have at most two decimals: snap after every update to avoid drift.
            arc.residual = hundredths(arc.residual - amount);
            reverse.residual = hundredths(reverse.residual + amount);
        }
    } // namespace

    int edges_for_density(int vertices, float density, bool simple) {
        check_vertices(vertices);
        if (!std::isfinite(density) || density < 0 || density > 1)
            throw std::invalid_argument("Density must be between 0 and 1.");
        const int minimum = vertices - 1;
        const int maximum = maximum_generated_edges(vertices, simple);
        return minimum + static_cast<int>(std::lround(density * (maximum - minimum)));
    }

    Graph generate(int vertices, int edge_count, bool decimal, std::uint32_t seed, bool simple) {
        check_vertices(vertices);
        const int maximum = maximum_generated_edges(vertices, simple);
        if (edge_count < vertices - 1 || edge_count > maximum)
            throw std::invalid_argument("Edge count must be between " + std::to_string(vertices - 1) +
                                        " and " + std::to_string(maximum) +
                                        (simple ? " for a simple graph." : " for a general graph."));
        std::mt19937 rng(seed);
        Graph graph{vertices, {}};
        std::vector<std::vector<bool>> used(vertices, std::vector<bool>(vertices));
        const int capacity_levels = decimal ? 2000 : 20;
        // Two half-range draws give a symmetric triangular distribution:
        // middle capacities are common, while both extremes remain possible.
        std::uniform_int_distribution<int> capacity_left(0, capacity_levels / 2 - 1);
        std::uniform_int_distribution<int> capacity_right(0, capacity_levels / 2);
        auto add = [&](int u, int v) {
            const int capacity = 1 + capacity_left(rng) + capacity_right(rng);
            graph.edges.push_back({u, v, capacity / (decimal ? 100.0f : 1.0f)});
            used[u][v] = true;
        };
        // A randomized spanning path ensures a useful source-to-sink exercise.
        std::vector<int> order(vertices);
        std::iota(order.begin(), order.end(), 0);
        std::shuffle(order.begin() + 1, order.end() - 1, rng);
        for (int i = 1; i < vertices; ++i) add(order[i - 1], order[i]);
        struct Candidate { int from, to; double priority; };
        std::vector<Candidate> candidates;
        std::exponential_distribution<double> priority(1.0);
        for (int u = 0; u < vertices - 1; ++u)
            for (int v = 1; v < vertices; ++v)
                if (u != v && !used[u][v] && (!simple || !used[v][u])) {
                    // Weighted sampling without replacement: terminal edges
                    // have four times the selection weight of internal edges.
                    const double weight = (u == 0 || v == vertices - 1) ? 4.0 : 1.0;
                    candidates.push_back({u, v, priority(rng) / weight});
                }
        std::sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) {
            return a.priority < b.priority;
        });
        for (const auto& candidate : candidates) {
            if (static_cast<int>(graph.edges.size()) == edge_count) break;
            const int u = candidate.from, v = candidate.to;
            if (simple && used[v][u]) continue;
            add(u, v);
        }
        return graph;
    }

    Result solve(const Graph& graph, int source, int sink, Algorithm algorithm) {
        check_vertices(graph.vertices);
        if (source < 0 || sink < 0 || source >= graph.vertices || sink >= graph.vertices || source == sink)
            throw std::invalid_argument("Source and sink must be distinct valid vertices.");
        Residual residual(graph.vertices);
        std::vector<std::pair<int, int>> original;
        for (const Edge& edge : graph.edges) {
            if (edge.from < 0 || edge.to < 0 || edge.from >= graph.vertices || edge.to >= graph.vertices || edge.from == edge.to)
                throw std::invalid_argument("Edges must connect distinct valid vertices.");
            if (!std::isfinite(edge.capacity) || edge.capacity < 0 || edge.capacity > 10000 ||
                std::fabs(edge.capacity - hundredths(edge.capacity)) > epsilon)
                throw std::invalid_argument("Capacity must be 0..10000 with at most two decimals.");
            const int forward = static_cast<int>(residual[edge.from].size());
            const int reverse = static_cast<int>(residual[edge.to].size());
            original.emplace_back(edge.from, forward);
            residual[edge.from].push_back({edge.to, reverse, edge.capacity});
            residual[edge.to].push_back({edge.from, forward, 0});
        }
        Result result;
        if (algorithm == Algorithm::EdmondsKarp) {
            while (true) {
                std::vector<int> parent(graph.vertices, -1), arc_index(graph.vertices, -1);
                std::queue<int> queue;
                queue.push(source);
                parent[source] = source;
                while (!queue.empty() && parent[sink] == -1) {
                    const int u = queue.front(); queue.pop();
                    for (int i = 0; i < static_cast<int>(residual[u].size()); ++i) {
                        const Arc& arc = residual[u][i];
                        if (arc.residual >  epsilon && parent[arc.to] == -1) {
                            parent[arc.to] = u;
                            arc_index[arc.to] = i;
                            queue.push(arc.to);
                        }
                    }
                }
                if (parent[sink] == -1) break;
                float amount = std::numeric_limits<float>::max();
                for (int v = sink; v != source; v = parent[v])
                    amount = std::min(amount, residual[parent[v]][arc_index[v]].residual);
                for (int v = sink; v != source; v = parent[v])
                    augment(residual, parent[v], arc_index[v], amount);
                result.value += amount;
                ++result.augmentations;
            }
        } else {
            std::vector<int> level(graph.vertices), next(graph.vertices);
            auto build_levels = [&]() {
                std::fill(level.begin(), level.end(), -1);
                std::queue<int> queue;
                queue.push(source); level[source] = 0;
                while (!queue.empty()) {
                    int u = queue.front(); queue.pop();
                    for (const Arc& arc : residual[u]) {
                        if (arc.residual > epsilon && level[arc.to] == -1) {
                            level[arc.to] = level[u] + 1;
                            queue.push(arc.to);
                        }
                    }
                }
                return level[sink] != -1;
            };
            std::function<float(int, float)> push = [&](int u, float available) -> float {
                if (u == sink) return available;
                for (int& i = next[u]; i < static_cast<int>(residual[u].size()); ++i) {
                    const Arc& arc = residual[u][i];
                    if (arc.residual <= epsilon || level[arc.to] != level[u] + 1) continue;
                    float sent = push(arc.to, std::min(available, arc.residual));
                    if (sent > epsilon) {
                        augment(residual, u, i, sent);
                        return sent;
                    }
                }
                return 0;
            };
            while (build_levels()) {
                std::fill(next.begin(), next.end(), 0);
                while (float sent = push(source, std::numeric_limits<float>::max())) {
                    result.value += sent;
                    ++result.augmentations;
                }
            }
        }
        result.value = std::round(result.value * 100.0) / 100.0;
        for (std::size_t i = 0; i < original.size(); ++i) {
            auto [u, index] = original[i];
            result.flow.push_back(hundredths(graph.edges[i].capacity - residual[u][index].residual));
        }
        result.source_side.assign(graph.vertices, false);
        std::queue<int> queue;
        queue.push(source); result.source_side[source] = true;
        while (!queue.empty()) {
            int u = queue.front(); queue.pop();
            for (const Arc& arc : residual[u]) {
                if (arc.residual > epsilon && !result.source_side[arc.to]) {
                    result.source_side[arc.to] = true;
                    queue.push(arc.to);
                }
            }
        }
        return result;
    }
} // namespace nf
