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

1. Click a field and type to replace its value. Enter commits the edit; Tab
   selects the next enabled field. Editing settings leaves the current exercise
   visible until you generate a graph.
2. Choose **Edge count** or **Density**, and **Integer** or **Decimal**.
3. Click **Generate graph** or press **G**. Trace your solution manually.
4. Press **Space** to compute and reveal the maximum flow. Positive-flow edges
   turn teal. Labels change from capacity to `flow / capacity`, including zero
   flow on unused edges. The total appears at the upper right.
5. Press **R** to hide the solution and practice again on the same graph.

| Control | Action |
| --- | --- |
| G / Enter | Generate using the current settings and seed (outside text fields) |
| N | Increment the seed and generate a new exercise |
| Space | Reveal the final flow |
| R | Clear the displayed solution, preserving the graph and layout |
| U | Toggle showing only edges with positive flow, after solving |
| L | Toggle edge labels; hovered edges still show their label |
| C | Restore the initial vertex layout |
| Drag a vertex | Rearrange the graph |
| Hover an edge | Highlight its direction and inspect its endpoints/capacity/flow |

Arrows indicate direction. Opposite directed edges bend to opposite sides.
Dense graphs naturally overlap: drag vertices, hide labels, and hover individual
edges to inspect them. Up to 40 vertices are supported in the visualizer;
small graphs are best for manual tracing.

## Graph rules

- Source is vertex **0**; sink is vertex **V−1**.
- Integer capacities use **Edmonds–Karp** (BFS augmenting paths).
- Decimal capacities use **Dinic / Dinitz** (BFS levels and DFS blocking flow).
- No self-loops or duplicate directed edges. Antiparallel edges are allowed.
- Edge count is between `V−1` and `V*(V−1)`, inclusive.
- Density is a normalized interpolation between those limits:
  `E = (V−1) + round(density * (V*(V−1) − (V−1)))`.
  This is the requested density convention, rather than the usual `E / (V*(V−1))`.
- Edge count and density are alternative inputs. In density mode, generating
  updates the edge-count field to the actual number of generated edges.
- A randomly ordered spanning path from source to sink is inserted first;
  remaining ordered pairs are sampled without replacement. Every graph therefore
  admits positive flow, including at density 0. This construction is deliberately
  biased toward usable exercises, rather than uniform over all directed graphs.
- Integer capacities range from **1 to 20**. Decimal capacities range from
  **0.01 to 20.00**, sampled in hundredths and stored as `float`.
- Float residual updates are rounded to hundredths, with a `0.0001` residual
  threshold. This preserves the two-decimal model despite binary float rounding.
- Reusing the same parameters and seed reproduces the exercise within the same
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
generator boundaries, invalid inputs, and reproducibility. Both algorithms are
compared with exhaustive minimum-cut enumeration on 840 small generated graphs,
with capacity, conservation, and residual/min-cut checks.

To build and test the algorithms without graphics dependencies:

```sh
cmake -S . -B build-core -DNETWORKFLOW_BUILD_APP=OFF
cmake --build build-core -j
ctest --test-dir build-core --output-on-failure
```

An optional graphics smoke test opens a window, exercises the real input
handlers, saves three PPM screenshots, and exits:

```sh
./build/networkflow --smoke-test /tmp/flow-preview
```

- `include/flow.hpp`: graph and algorithm interfaces.
- `src/flow.cpp`: generation, Edmonds–Karp, and Dinic.
- `src/main.cpp`: OpenGL rendering, SDL2 input, and parameter controls.
- `tests/flow_tests.cpp`: algorithm and generation checks.
