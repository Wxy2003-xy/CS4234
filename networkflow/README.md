# Flow Lab

A small C++17 network-flow practice app. SDL2 handles the window and input;
OpenGL 2.1 draws the graph, arrows, text, and controls. No external font assets
or UI toolkit are needed.

## Build and run

Requires CMake 3.20+, a C++17 compiler, SDL2 development files with a CMake
package, and desktop OpenGL. On macOS, `brew install cmake sdl2` supplies the
non-system dependencies. On Debian/Ubuntu, install `cmake`, `g++`,
`libsdl2-dev`, and `libgl1-mesa-dev`.

From this directory:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/networkflow
```

On Windows, supply SDL2 through your package manager's CMake toolchain or
`CMAKE_PREFIX_PATH`; the executable also needs the SDL2 runtime DLL.
The app was built and its OpenGL rendering verified on macOS.

## Practice

The initial exercise uses **6 vertices and 10 edges**.

1. Click a field and type to replace its value. Enter commits the edit; Tab
   selects the next enabled field. Editing settings leaves the current exercise
   visible until you generate a graph.
2. Choose **Edge count** or **Density**, **Integer** or **Decimal**, and
   **General** or **Simple** graph generation.
3. Click **Generate graph** or press **G**. Trace your solution manually.
4. Press **Space** to compute and reveal the maximum flow. Positive-flow edges
   turn teal. Labels change from capacity to `flow / capacity`, including zero
   flow on unused edges. The total appears at the upper right.
5. Press **R** to hide the solution and practice again on the same graph.
6. Click **High contrast** above the graph, or press **H**, to switch to a
   white background with black graph edges and red numeric labels. The theme
   changes immediately without regenerating the exercise.

| Control | Action |
| --- | --- |
| G / Enter | Generate using the current settings and seed (outside text fields) |
| N | Increment the seed and generate a new exercise |
| Space | Reveal the final flow |
| R | Clear the displayed solution, preserving the graph and layout |
| U | Toggle showing only edges with positive flow, after solving |
| L | Toggle edge labels; hovered edges still show their label |
| C | Restore the initial vertex layout |
| S | Toggle General/Simple for the next generated graph |
| H | Toggle the white/black/red high-contrast theme |
| Drag a vertex | Rearrange the graph |
| Hover an edge | Highlight its direction and inspect its endpoints/capacity/flow |

Arrows indicate direction. Opposite directed edges bend to opposite sides.
Dense graphs naturally overlap: drag vertices, hide labels, and hover individual
edges to inspect them. Up to 40 vertices are supported in the visualizer;
small graphs are best for manual tracing.

## Graph rules

- Source is vertex **0**; sink is vertex **V−1**.
- Generated edges never enter the source or leave the sink.
- Integer capacities use **Edmonds–Karp** (BFS augmenting paths).
- Decimal capacities use **Dinic / Dinitz** (BFS levels and DFS blocking flow).
- No self-loops or duplicate directed edges. **General** mode allows
  antiparallel edges. **Simple** mode forbids both `(x,y)` and `(y,x)` from
  appearing together for any pair of vertices.
- Edge count is between `V−1` and `(V−1)*(V−2)+1` in General mode, or between
  `V−1` and `V*(V−1)/2` in Simple mode, inclusive. An out-of-range edge
  count produces a validation message rather than silently changing the count.
- Density is a normalized interpolation between those limits:
  `E = (V−1) + round(density * (maximum − (V−1)))`, using the maximum for
  the selected graph mode.
  This is the requested density convention, rather than the usual `E / (V*(V−1))`.
- Edge count and density are alternative inputs. In density mode, generating
  updates the edge-count field to the actual number of generated edges.
- A randomly ordered spanning path from source to sink is inserted first;
  additional edges are sampled without replacement, giving edges out of the
  source or into the sink four times the selection weight of internal edges.
  This favors more routes leaving the source and entering the sink while
  retaining variety. Every graph therefore
  admits positive flow, including at density 0. This construction is deliberately
  biased toward usable exercises, rather than uniform over all directed graphs.
- Integer capacities range from **1 to 20**. Decimal capacities range from
  **0.01 to 20.00**, sampled in hundredths and stored as `float`.
  Both use a symmetric triangular distribution centered around 10: moderate
  values are more likely, while values near either endpoint remain possible.
- Float residual updates are rounded to hundredths, with a `0.0001` residual
  threshold. This preserves the two-decimal model despite binary float rounding.
- Reusing the same parameters, graph mode, and seed reproduces the exercise within the same
  build/toolchain. Changing pending settings does not change the algorithm used
  for the currently displayed graph.

The application reveals one valid maximum flow; other valid maximum flows may
distribute flow differently. The core gives each original edge its own residual
reverse edge, so cancellation remains correct even with antiparallel edges.

## Tests and source

```sh
ctest --test-dir build --output-on-failure
```

The tests check known networks, disconnected sinks, zero capacities, fractions,
parallel/antiparallel edges, an example requiring reverse-edge cancellation,
both generator modes and their boundaries, terminal-edge restrictions, the
moderate-capacity and terminal-edge biases, invalid inputs, and reproducibility. Both algorithms are
compared with exhaustive minimum-cut enumeration on 1,260 small generated graphs,
with capacity, conservation, and residual/min-cut checks.

To build and test the algorithms without graphics dependencies:

```sh
cmake -S . -B build-core -DNETWORKFLOW_BUILD_APP=OFF
cmake --build build-core -j
ctest --test-dir build-core --output-on-failure
```

An optional graphics smoke test opens a window, exercises the real input
handlers (including the high-contrast toggle), saves three PPM screenshots,
and exits:

```sh
./build/networkflow --smoke-test /tmp/flow-preview
```

- `include/flow.hpp`: graph and algorithm interfaces.
- `src/flow.cpp`: generation, Edmonds–Karp, and Dinic.
- `src/main.cpp`: OpenGL rendering, SDL2 input, and parameter controls.
- `tests/flow_tests.cpp`: algorithm and generation checks.
