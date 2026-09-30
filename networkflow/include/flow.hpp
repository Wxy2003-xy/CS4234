#pragma once

#include <cstdint>
#include <vector>

namespace nf {
constexpr float epsilon = 0.0001f;
enum class Algorithm { EdmondsKarp, Dinic };
struct Edge {
    int from;
    int to;
    float capacity;
};
struct Graph {
    int vertices = 0;
    std::vector<Edge> edges;
};
struct Result {
    double value = 0;
    std::vector<float> flow; // Indexed by original edge, including antiparallel edges.
    int augmentations = 0;
    std::vector<bool> source_side; // Vertices reachable in the final residual graph.
};

float hundredths(float value);
int edges_for_density(int vertices, float density, bool simple = false);
Graph generate(int vertices, int edge_count, bool decimal, std::uint32_t seed, bool simple = false);
Result solve(const Graph& graph, int source, int sink, Algorithm algorithm);
} // namespace nf
