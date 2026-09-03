#pragma once

#include <bits/stdc++.h>

namespace dp::graph {

using namespace std;

    class Graph {
    public:
        int V;
        int E;
        // Weighted adjacency matrix. For u != v, 0 and INT_MAX mean no edge.
        vector<vector<int>> adj;

        Graph(int V, int E, vector<vector<int>> adj) {
            this->V = V;
            this->E = E;
            this->adj = adj;
        }

        int SSSP_weight(int src, int dest) {
            validate_adjacency_matrix();
            if (src < 0 || src >= V || dest < 0 || dest >= V) {
                throw out_of_range("source and destination must be valid vertices");
            }
            for (int u = 0; u < V; ++u) {
                for (int v = 0; v < V; ++v) {
                    if (u != v && has_edge(u, v) && adj[u][v] < 0) {
                        throw invalid_argument(
                            "SSSP_weight requires non-negative edge weights"
                        );
                    }
                }
            }

            constexpr long long INF = numeric_limits<long long>::max() / 4;
            vector<long long> distance(V, INF);
            priority_queue<
                pair<long long, int>,
                vector<pair<long long, int>>,
                greater<pair<long long, int>>
            > pending;

            distance[src] = 0;
            pending.push({0, src});

            while (!pending.empty()) {
                auto [current_distance, u] = pending.top();
                pending.pop();

                if (current_distance != distance[u]) {
                    continue;
                }
                if (u == dest) {
                    break;
                }

                for (int v = 0; v < V; ++v) {
                    if (!has_edge(u, v)) {
                        continue;
                    }

                    const int weight = adj[u][v];
                    const long long candidate = current_distance + weight;
                    if (candidate < distance[v]) {
                        distance[v] = candidate;
                        pending.push({candidate, v});
                    }
                }
            }

            if (distance[dest] == INF) {
                return -1;
            }
            if (distance[dest] > numeric_limits<int>::max()) {
                throw overflow_error("shortest-path weight exceeds int range");
            }
            return static_cast<int>(distance[dest]);
        }

        bool is_connected() {
            validate_adjacency_matrix();
            if (V <= 1) {
                return true;
            }

            vector<bool> visited(V, false);
            queue<int> pending;
            visited[0] = true;
            pending.push(0);
            int visited_count = 1;

            while (!pending.empty()) {
                const int u = pending.front();
                pending.pop();

                for (int v = 0; v < V; ++v) {
                    if (!visited[v] && has_edge(u, v)) {
                        visited[v] = true;
                        ++visited_count;
                        pending.push(v);
                    }
                }
            }

            return visited_count == V;
        }

        bool is_complete() {
            validate_adjacency_matrix();

            for (int u = 0; u < V; ++u) {
                for (int v = 0; v < V; ++v) {
                    if (u != v && !has_edge(u, v)) {
                        return false;
                    }
                }
            }
            return true;
        }

        vector<int> rand_independent_set(int seed) {
            validate_undirected_graph();

            mt19937 generator(static_cast<uint32_t>(seed));
            bernoulli_distribution choose_vertex(0.5);
            vector<int> order(V);
            iota(order.begin(), order.end(), 0);
            shuffle(order.begin(), order.end(), generator);

            vector<bool> selected(V, false);
            vector<int> independent_set;

            for (const int vertex : order) {
                if (choose_vertex(generator)
                    && can_join_independent_set(vertex, selected)) {
                    selected[vertex] = true;
                    independent_set.push_back(vertex);
                }
            }

            sort(independent_set.begin(), independent_set.end());
            return independent_set;
        }

        vector<int> rand_maximal_independent_set(int seed) {
            validate_undirected_graph();

            mt19937 generator(static_cast<uint32_t>(seed));
            vector<int> order(V);
            iota(order.begin(), order.end(), 0);
            shuffle(order.begin(), order.end(), generator);

            vector<bool> selected(V, false);
            vector<int> independent_set;

            // Greedily scanning every vertex makes the result maximal: each
            // rejected vertex has a selected neighbour.
            for (const int vertex : order) {
                if (can_join_independent_set(vertex, selected)) {
                    selected[vertex] = true;
                    independent_set.push_back(vertex);
                }
            }

            sort(independent_set.begin(), independent_set.end());
            return independent_set;
        }

        vector<int> rand_vertex_cover(int seed) {
            validate_undirected_graph();

            const vector<int> independent_set =
                rand_maximal_independent_set(seed);
            vector<bool> is_independent(V, false);
            for (const int vertex : independent_set) {
                is_independent[vertex] = true;
            }

            vector<int> vertex_cover;
            for (int vertex = 0; vertex < V; ++vertex) {
                if (!is_independent[vertex]) {
                    vertex_cover.push_back(vertex);
                }
            }
            return vertex_cover;
        }

        vector<int> greedy_approx_min_vertex_cover() {
            validate_undirected_graph();

            vector<bool> matched(V, false);
            vector<int> vertex_cover;

            for (int u = 0; u < V; ++u) {
                if (matched[u]) {
                    continue;
                }

                for (int v = u + 1; v < V; ++v) {
                    if (!matched[v] && has_edge(u, v)) {
                        matched[u] = true;
                        matched[v] = true;
                        vertex_cover.push_back(u);
                        vertex_cover.push_back(v);
                        break;
                    }
                }
            }

            sort(vertex_cover.begin(), vertex_cover.end());
            return vertex_cover;
        }

        vector<vector<int>> MST(int seed) {
            validate_undirected_graph();

            vector<vector<int>> tree(V, vector<int>(V, 0));
            if (V == 0) {
                return tree;
            }
            if (seed < 0 || seed >= V) {
                throw out_of_range("MST seed must be a valid start vertex");
            }

            vector<int> best_weight(V, numeric_limits<int>::max());
            vector<int> parent(V, -1);
            vector<bool> in_tree(V, false);
            best_weight[seed] = 0;

            for (int iteration = 0; iteration < V; ++iteration) {
                int u = -1;
                for (int vertex = 0; vertex < V; ++vertex) {
                    if (!in_tree[vertex]
                        && (u == -1
                            || best_weight[vertex] < best_weight[u])) {
                        u = vertex;
                    }
                }

                if (u == -1
                    || best_weight[u] == numeric_limits<int>::max()) {
                    throw invalid_argument(
                        "MST requires a connected undirected graph"
                    );
                }

                in_tree[u] = true;
                if (parent[u] != -1) {
                    const int weight = adj[parent[u]][u];
                    tree[parent[u]][u] = weight;
                    tree[u][parent[u]] = weight;
                }

                for (int v = 0; v < V; ++v) {
                    if (!in_tree[v]
                        && has_edge(u, v)
                        && adj[u][v] < best_weight[v]) {
                        best_weight[v] = adj[u][v];
                        parent[v] = u;
                    }
                }
            }

            return tree;
        }


    private:
        bool can_join_independent_set(
            int vertex,
            const vector<bool>& selected
        ) const {
            for (int neighbour = 0; neighbour < V; ++neighbour) {
                if (selected[neighbour] && has_edge(vertex, neighbour)) {
                    return false;
                }
            }
            return true;
        }

        bool has_edge(int src, int dest) const {
            if (src == dest) {
                return false;
            }

            const int weight = adj[src][dest];
            return weight != 0 && weight != numeric_limits<int>::max();
        }

        void validate_adjacency_matrix() const {
            if (V < 0 || static_cast<int>(adj.size()) != V) {
                throw invalid_argument(
                    "adjacency matrix must contain exactly V rows"
                );
            }

            for (const auto& row : adj) {
                if (static_cast<int>(row.size()) != V) {
                    throw invalid_argument(
                        "each adjacency-matrix row must contain exactly V entries"
                    );
                }
            }
        }

        void validate_undirected_graph() const {
            validate_adjacency_matrix();

            for (int u = 0; u < V; ++u) {
                for (int v = u + 1; v < V; ++v) {
                    const bool forward_edge = has_edge(u, v);
                    const bool reverse_edge = has_edge(v, u);

                    if (forward_edge != reverse_edge
                        || (forward_edge && adj[u][v] != adj[v][u])) {
                        throw invalid_argument(
                            "operation requires a symmetric undirected graph"
                        );
                    }
                }
            }
        }
    };

} // namespace dp::graph
