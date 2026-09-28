# CS4234 midterm: notes review and targeted practice

Reviewed against the supplied lectures, Tutorials 1–4, both sample finals, and the current `CS4234.tex` / 36-page `CS4234.pdf`. Page references below are PDF page numbers; line references refer to the current TeX source. This is a separate review, not an edit of the notes.

## Scope and what the papers suggest

Network Flow III, slide 36, states that the midterm covers everything through Week 6 and is open book. That supports including all seven supplied lecture decks. This review uses that announcement as scope evidence; it does not import the final papers' exam instructions into the midterm.

Prioritize approximation, knapsack/FPTAS, set cover, vertex cover, metric TSP/Eulerian arguments, randomization and derandomization, MAX-SAT, LP relaxation/rounding, and network flow including applications. Tutorial 4 is present in the merged tutorial PDF, on PDF pp. 21–28; it should not be overlooked because of the file's name.

LP duality, simplex, k-center, and SDP/vector-program rounding are not developed in the supplied lecture decks. Treat those parts of the finals as lower priority unless the lecturer has separately included them. Bipartite matching via flow is in scope; matching–vertex-cover LP duality is a different topic.

The finals suggest question **styles**, not reliable probabilities:

| Evidence | Preparation implication |
|---|---|
| AY24/25 Q1: bounded-frequency weighted set cover | Know how to formulate an ILP, relax it, choose a threshold, and prove feasibility and cost separately. |
| AY24/25 Q2: residual-flow difference and a counterexample | Know the residual graph as an algebraic object, not just a picture. |
| AY24/25 Q4: deterministic MAX-3-CUT | A randomized expectation bound may be only the first half of an answer. |
| AY24/25 Q5: orient an even-degree connected graph | Eulerian structure can appear outside a TSP question. |
| AY25/26 Q1: true/false | Preconditions, quantifiers, and small counterexamples matter. |
| AY25/26 Q2–3: independent set and LCS | Be able to apply indicators or a best-of-candidates argument to an unfamiliar problem. |
| AY25/26 Q4: scheduling, flow, both directions, ILP | Practice translating several simultaneous restrictions into layers of a network. |

The 2024 paper says time complexity need not be analyzed **for that paper**. The current tutorials explicitly request it, so keep a short running-time argument ready.

## What to change first

Your modified-knapsack argument, weighted-set-cover charging proof, and shifted MAX-DICUT rounding calculation are useful foundations. The main issue is uneven coverage: course-specific notes start on p. 14, while several later core topics are absent or unfinished.

| Priority | Area | Current state | Concrete improvement |
|---|---|---|---|
| 1 | Incorrect formulas and algorithms | Several errors listed below | Correct these before adding more material or printing. |
| 1 | Flow applications | Notes stop after shortest-path pseudocode | Add matching, vertex splitting, disjoint paths, integrality, and a layered scheduling example with both reduction directions. |
| 1 | Conditional expectation | No worked method | Add computable conditional expectations for MAX-CUT and MAX-SAT, plus the invariant that expectation never decreases. |
| 1 | Tutorial 4 | Not incorporated | Add randomized set cover with the conditioning argument, Menger, cut tie-breaking, and min-cut saturation. |
| 1 | TSP/Eulerian structure | Missing as course topics | Add double-tree, Christofides, the matching-cost lemma, Held–Karp DP, and parity arguments. |
| 2 | Knapsack FPTAS | Scaling present, DP and proof incomplete | Replace the fragment with the value-indexed recurrence and complete error chain. |
| 2 | LP/MAX-SAT | Wrong exponent, unfinished bound, no combined algorithm | Add the full LP, the clause-length guarantee, and the 3/4 combination proof. |
| 2 | Threshold rounding | VC model present, rounding absent | Add weighted vertex cover and bounded-frequency set cover side by side. |
| 2 | Algorithm analysis | Scaling partial; shortest-path proof absent | Add phase count, distance monotonicity, and the repeated-saturation argument. |
| 3 | Reference organization | Long generic prerequisite/formula section | Move unrelated reference material to an appendix and add a one-page technique index. |

For every algorithm, use the same five fields: **when it applies; algorithm; feasibility; OPT comparison; runtime and failure example**. The first three lines should be enough to recognize and start using the method.

## Corrections to the current notes

### 1. Subset-sum pseudocode is not the algorithm analyzed

Notes p. 18; TeX around line 1056.

The line `B <- B + s_i` increases the capacity after an item is selected. As written, it can return an infeasible solution. Also, returning immediately on an oversized first item can give the empty answer despite later feasible items.

Use a fixed capacity `B` and running sum `A = 0`. Process positive items in descending order; if `A + s_i <= B`, select the item and update `A += s_i`; otherwise skip it. This matches Tutorial 2 Q3. The near-tight family is capacity `2k` and sizes `k+1, k, k`, giving ratio `(k+1)/(2k) -> 1/2`. The current `{100,51,51}`, capacity 102 example shows nonoptimality, not tightness of the factor 2.

### 2. The FPTAS needs the value-indexed DP

Notes pp. 22–23; TeX around line 1410.

Scaling values does not reduce the number of columns in a weight-indexed `O(nW)` DP. Use

\[
D[i,p]=\text{minimum weight of a subset of the first }i\text{ items having value exactly }p.
\]

The recurrence is

\[
D[i,p]=\min\{D[i-1,p],\ w_i+D[i-1,p-v_i]\},
\]

omitting the second alternative if `p < v_i`. Set `D[0,0]=0` and all other initial entries to infinity. Return the largest `p` with `D[n,p] <= W` and reconstruct its items. Since `p <= nV`, this takes `O(n²V)`, where `V=max_i v_i`.

Discard items with `w_i>W` first. Handle an empty set or `V=0` separately. Set

\[
K=\epsilon V/n,\qquad \hat v_i=\lfloor v_i/K\rfloor.
\]

It is **the scaled value** that satisfies `hat v_i <= n/epsilon`; the original `v_i` need not. `V` is an input value, not an asymptotic constant. The scaled DP takes `O(n³/epsilon)` arithmetic operations. For its output `S` and an original optimum `S*`,

\[
v(S)\ge K\hat v(S)\ge K\hat v(S^*)\ge v(S^*)-nK
=OPT-\epsilon V\ge(1-\epsilon)OPT.
\]

The final step uses `V <= OPT`, which is why filtering overweight items matters. Keep the original weights, so feasibility is preserved.

### 3. MAX-SAT LP rounding: exponent, objective, and bound direction

Notes p. 26; TeX around lines 1568–1597.

Fill in the objective `maximize sum_i w_i y_i` and bounds `0 <= x_j,y_i <= 1` for the relaxation. Its optimum is `LP*`, with `LP* >= OPT`; it is not generally equal to the integral optimum.

After AM–GM, the exponent is **the clause length** `ell_i`, not its reciprocal. The correct chain is

\[
\Pr[C_i\text{ satisfied}]
\ge 1-(1-y_i^*/\ell_i)^{\ell_i}
\ge [1-(1-1/\ell_i)^{\ell_i}]y_i^*
\ge(1-1/e)y_i^*.
\]

The middle inequality follows from concavity on `[0,1]`. Summing gives

\[
\mathbb E[ALG]\ge(1-1/e)LP^*\ge(1-1/e)OPT.
\]

For the product formula, remove duplicate literals and handle tautological clauses separately; independent variable choices do not make repeated occurrences independent.

Lecture L4-LP slide 21 itself has two typos: its first equality should generally be a lower bound, and its last `<=` should be `>=`. Slide 20 has the correct structure. Do not copy slide 21 verbatim.

### 4. Biased MAX-SAT requires preprocessing

Notes p. 25; TeX around line 1532.

The bound `min(p,1-p²)` requires dealing with negative singleton clauses. Merge repeated unit clauses by summing their weights. Rename variables consistently throughout the formula so that, for each variable, the positive unit-clause weight is at least the negative one. Let `N` be the sum of the negative unit-clause weights and `W` the total clause weight. Every assignment loses at least `N`, so `OPT <= W-N`.

Ignore those negative singleton contributions in the lower bound. Every remaining singleton is positive; every other remaining clause has at least two distinct literals. Thus

\[
\mathbb E[ALG]\ge \min(p,1-p^2)(W-N)\ge\min(p,1-p^2)OPT.
\]

Choose `p=(sqrt(5)-1)/2`. Also change the text saying the unbiased satisfaction probability decreases with clause length: it **increases**.

### 5. MAX-DICUT: your unfinished argument yields another 1/2 guarantee

Notes pp. 29–30; TeX around lines 1754–1817.

The existing derivation for direct rounding `p_i=x_i*` reaches

\[
\mathbb E[ALG]\ge L^2/W,
\qquad L=LP^*,\quad W=\sum_{ij}w_{ij}.
\]

Complete it by observing that `x_i=1/2` for every vertex and `z_ij=1/2` for every arc is LP-feasible. Therefore `L >= W/2`, and for `W>0`,

\[
\mathbb E[ALG]\ge L^2/W\ge L/2\ge OPT/2.
\]

If `W=0`, every cut is optimal. Thus the claim that direct rounding cannot give as good an approximation is incorrect for this LP. What differs is the proof: shifted rounding gives a **per-arc** `z_ij/2` lower bound, whereas direct rounding uses a **global** bound and LP optimality.

In the affine-parameter calculation, assume `a,b >= 0` and `a+b <= 1` before multiplying lower bounds. Correct

\[
\alpha=2b(1-b)=\tfrac12-2(b-\tfrac12)^2\le\tfrac12.
\]

You maximize this retained-value fraction, achieving `1/2` at `b=1/2, a=1/4`. The current notes have the opposite inequality and say to minimize it. The square argument optimizes this particular lower bound; by itself it does not establish an impossibility result for every affine-rounding analysis.

### 6. Define the reachable set in the max-flow/min-cut proof

Notes pp. 32–33; TeX around lines 1940–2003.

At termination define

\[
S=\{v:v\text{ is reachable from }s\text{ in }G_f\},\qquad T=V\setminus S.
\]

This is essential. An arbitrary partition containing `s` on one side and `t` on the other need not be a minimum cut. An outgoing residual edge would make its head reachable, contradicting membership in `T`; it does not automatically make `t` reachable.

Write the cut identity correctly:

\[
|f|=\sum_{u\in S,v\notin S}f(u,v)-\sum_{u\in S,v\notin S}f(v,u).
\]

The notes' `f_in(A)` currently sums `f(u,v)` while indexing incoming arcs `(v,u)`; change the summand to `f(v,u)`. In general, define flow value as net outflow from `s`, or explicitly assume no arcs enter `s`. Include `0 <= f(e) <= c(e)`.

### 7. Integrality means integral values on every arc

Notes p. 34; TeX line 2159.

Replace “there exists a max-flow that is of integer value” with:

> With integer capacities, there exists a maximum flow satisfying `f(e) in Z` for every arc `e`.

This does not say that every feasible flow, or every maximum flow, is integral. Edgewise integrality is what makes matching and disjoint-path reductions work. Starting from zero, augmenting by an integer bottleneck preserves it.

### 8. Capacity scaling can terminate too early as written

Notes p. 35; TeX around line 2161; also present in the lecture pseudocode.

Starting at `C=max_e c(e)`, repeatedly doing real division `k <- k/2`, and stopping below 1 may skip threshold 1. For example, `s -> a` has capacity 3 and `a -> t` has capacity 1. Thresholds 3 and 1.5 find no admissible path; the code stops with flow 0 although the maximum is 1.

For integer capacities use

\[
k=2^{\lfloor\log_2 C\rfloor},
\]

then halve through the phase `k=1`. Handle `C=0` separately. Every selected path must have **all residual capacities at least k**; the inner line “choose any path” must preserve that restriction.

Define `S` using reachability through residual arcs of capacity at least `k`. At a phase end, `f* - |f| <= mk`. At the next phase's start, the gap is at most `2mk`, so there are at most `2m` augmentations of size at least `k`. This bounds augmentations **in a phase**; it does not assert that that phase reaches the final optimum. There are `O(1+log C)` phases and `O(m²(1+log C))` time under the lecture's graph-operation model.

### 9. Complete shortest-augmenting-path flow

Notes p. 36; TeX around line 2221.

The body currently says “choose any path” after referring to a shortest path. Replace both with one BFS choice of a shortest residual `s–t` path, measured by **number of arcs**, and augment by its bottleneck.

Add the two proof facts from Network Flow III:

1. Residual BFS distances from `s` never decrease.
2. If `(u,v)` is saturated on a shortest path and later reappears via use of `(v,u)`, the distance to `u` has risen by at least 2.

Each residual arc can be critical only `O(n)` times, giving `O(nm)` augmentations and `O(nm²)` time. This operation count does not depend on capacity magnitudes. The lecture labels the scaling and shortest-path variants differently from some references; write the path-selection rule and runtime beside each name to avoid ambiguity.

### 10. Smaller but worthwhile repairs

- The maximal-matching vertex-cover pseudocode on p. 21 returns `M`, a set of edges. Return **all endpoints of M** as the cover.
- The best-of-candidates proof on p. 17 needs nonnegative component functions and candidates feasible for the same domain. An optimizer of each component must be global, not merely a local optimum.
- The random-endpoint vertex-cover proof on pp. 23–24 is unfinished and its indicator does not describe the randomly chosen endpoint. Either remove this optional variant from the main reference or prove it using iteration indicators: conditional on an active iteration, the probability of selecting a vertex in a fixed optimal cover is at least 1/2; selected vertices are distinct; hence expected iterations are at most twice the optimal-cover size. This is an expectation guarantee, not a per-run bound.
- Residual arcs must retain their original-edge identity if the input has antiparallel arcs. Testing only whether `(u,v)` belongs to the original edge set cannot distinguish a forward residual arc from a reverse residual arc of `(v,u)`. Use labelled residual arcs, or state a simplifying assumption that the original network has no antiparallel pairs.
- Replace empty “MAX-CUT,” “Inapproximability,” and generic FPTAS fragments with finished reference entries. Do not let an unfinished fragment look like an exam-ready proof.

## Compact material to add

### A. One-page method index

| Problem cue | Method | Key inequality / certificate |
|---|---|---|
| Cost per newly covered object | Weighted greedy set cover | `ALG = sum_e price(e) <= H_n OPT` |
| Every element has at most k covering choices | LP threshold `1/k` | Feasibility from a sum of at most k variables; `cost <= k LP* <= k OPT` |
| Destroy fixed-size obstructions | Maximal disjoint obstruction packing | Each packed obstruction forces a distinct OPT action |
| Maximize nonnegative local rewards | Independent random labels | Indicators; `OPT <= total weight` |
| Need a deterministic version | Conditional expectation | Current expectation is a weighted average of branch expectations |
| Fractional values plus weighted rewards | LP rounding | Bound each reward against its LP variable, then sum |
| Long clauses vs short clauses | Combine complementary algorithms | `max(A,B) >= (A+B)/2` |
| Capacity and compatibility constraints | Layered flow | A path represents one assignment; layer capacities enforce quotas |
| Vertex throughput / vertex-disjointness | Vertex splitting | `v_in -> v_out` carries the vertex capacity |
| Lexicographic cut objective | Integer capacity perturbation | `(m+1)c(e)+1` |
| Metric travel / parity repair | MST, matching, Euler tour, shortcut | `MST <= OPT`, matching on odd vertices `<= OPT/2` |
| Large numeric DP coordinate | Scale values | At most `nK` additive rounding loss |

### B. TSP/Eulerian reference

Double-tree: compute MST `T`, double its edges, take an Euler tour, and shortcut repeated vertices. Completeness makes shortcut edges available; triangle inequality ensures they do not increase cost. Removing an edge of an optimal tour gives a spanning tree, so `w(T) <= OPT` and the returned tour costs at most `2OPT`.

Christofides: let `O` be the odd-degree vertices of the MST. Their number is even. Add a minimum-weight perfect matching `M` on `O`, then Euler-tour and shortcut. Shortcut the optimal tour to `O`; alternating edges give two perfect matchings, so the minimum matching costs at most half this restricted tour, at most `OPT/2`. Therefore `ALG <= w(T)+w(M) <= 3OPT/2`. Allow parallel edges in the intermediate multigraph.

For a connected undirected multigraph, an Euler circuit exists iff every degree is even. An Euler trail between distinct specified endpoints exists iff precisely those endpoints have odd degree. Adding an edge toggles both endpoint parities. Orienting an Euler circuit consistently gives a strongly connected directed graph on its nonisolated vertices.

Held–Karp: fix root `r`; `D[S,v]` is the minimum cost of a path from `r` to `v` visiting exactly `S`, once each. Base `D[{r},r]=0`; other impossible states are infinity. For `v != r`,

\[
D[S,v]=\min_{u\in S\setminus\{v\}}D[S\setminus\{v\},u]+c(u,v).
\]

Answer `min_{v != r} D[V,v]+c(v,r)`, time `O(n²2^n)`, space `O(n2^n)`.

### C. Conditional expectation in usable form

Let `F` be the expected final objective given current fixed decisions. For the next random decision `X`,

\[
F=\sum_a\Pr[X=a]\mathbb E[\text{objective}\mid\text{fixed decisions},X=a].
\]

Choose a branch with value at least `F`. Compute these expectations in polynomial time, repeat, and finish with a deterministic objective at least the original expectation.

For a fair random `k`-cut: an edge with both endpoints fixed contributes 0 or its full weight; an edge with at least one endpoint unfixed contributes `(1-1/k)` times its weight. When fixing a vertex, choose a label maximizing its cut weight to already fixed neighbors. Summing these local updates yields the conditional-expectation algorithm.

For fair MAX-SAT: a clause already satisfied contributes its weight; a clause already falsified contributes 0; an unsatisfied clause with `r` distinct unfixed variables contributes `w(1-2^{-r})`. With biased variables use the corresponding product of literal-failure probabilities instead.

### D. MAX-SAT's combined 3/4 guarantee

For length `ell`, let `a_ell=1-2^{-ell}` and `b_ell=1-(1-1/ell)^ell`. Fair rounding contributes at least `a_ell y_i*`, since `y_i*<=1`; LP rounding contributes at least `b_ell y_i*`.

`a_ell+b_ell >= 3/2`: equality for lengths 1 and 2; for `ell>=3`, use `a_ell>=7/8` and `b_ell>=1-1/e>5/8`. Run both algorithms and retain the better assignment:

\[
\mathbb E[\max(A,B)]\ge\tfrac12(\mathbb E[A]+\mathbb E[B])
\ge\tfrac34 LP^*\ge\tfrac34 OPT.
\]

Independence between the two algorithm outputs is unnecessary for this inequality. Their clause-length bounds are essential: simply averaging the global guarantees `1/2` and `1-1/e` does not give `3/4`.

### E. Flow reduction checklist

Specify all vertices, arcs, capacities, source/sink, and the target flow value. Then prove:

1. Every feasible original solution induces a flow meeting the target.
2. An **integral** flow meeting the target can be decoded into a feasible original solution.

For bipartite matching: `s -> left -> right -> t`, all capacities 1. For edge-disjoint directed paths: all original arcs capacity 1, then decompose integral flow into unit paths and discard cycles. For vertex capacities: replace `v` by `v_in -> v_out` with that capacity and redirect incoming/outgoing arcs appropriately. Multiple sources/sinks use a super-source/sink with sufficiently large capacities; a finite upper bound on total possible flow suffices in place of infinity.

Lower bounds and vertex demands are mentioned in Network Flow III slide 40 but not fully developed there. A useful additional reference, if needed: for `in(f)-out(f)=d(v)` and lower bounds `l(e)`, substitute `f=l+g`, give `g` capacity `c-l`, and define

\[
d'(v)=d(v)-\sum_{e\text{ into }v}l(e)+\sum_{e\text{ out of }v}l(e).
\]

Check `sum_v d'(v)=0`. For `d'<0`, add an arc from a super-source to `v` of capacity `-d'`; for `d'>0`, add `v -> super-sink` of capacity `d'`. Feasibility is equivalent to saturating every super-source arc. Recover `f=l+g`. This is supporting material, not evidence that a detailed circulation reduction must appear.

## Practice questions I would prioritize

These are predictions of useful question families, not claims about the actual paper. “High priority” reflects overlap between lectures, current tutorials, and exam style. Marks below are suggested practice weights only. Answer sketches follow the questions so you can attempt them first.

### Q1. Scheduling with two levels of quotas — high priority, 10 marks

There are workers `i`, disjoint periods `j`, and tasks, each belonging to one period. Worker `i` can perform tasks in `A_i`, may perform at most `b_i` tasks overall, and at most one task per period. Every task must be performed by exactly one worker.

(a) Construct a flow network and specify the target flow value. (b) Prove both directions of the reduction. (c) Give an ILP. (d) Explain why a fractional feasible flow is not itself a schedule, and why this does not prevent a flow-based algorithm.

Basis: AY25/26 Q4; Network Flow III applications.

### Q2. Bounded-frequency weighted covering — high priority, 8 marks

Each element belongs to at most `k` candidate sets with nonnegative costs. Give an ILP and a deterministic `k`-approximation using an LP. Prove feasibility and cost separately. Does the same threshold proof work if instead every **set contains at most k elements**?

Basis: AY24/25 Q1; L4-LP vertex-cover rounding.

### Q3. Randomized cut, then remove the randomness — high priority, 8 marks

For weighted MAX-`k`-CUT, design an algorithm obtaining at least `(1-1/k)OPT` in expectation. Convert it to a deterministic polynomial-time algorithm. State precisely how to compute the conditional expectation after a partial labelling. Explain why independence of the edge indicators is unnecessary.

Basis: Tutorial 3 Q1; AY24/25 Q4; L3 slides 37–44.

### Q4. Randomized set cover with a stopping rule — high priority, 10 marks

Solve the natural weighted-set-cover LP. One trial takes `r=ceil(ln(2n))` independent rounds, selecting set `j` with probability `x_j*` in each round and taking their union. Discard unsuccessful trials and repeat until a cover is obtained.

Prove that the algorithm returns a feasible cover almost surely, runs in expected polynomial time, and has expected returned cost at most `2r OPT`. Explain why “one trial has expected cost at most r LP*, so the returned cover does too” is not justified.

Basis: Tutorial 4 Q1. This is an especially useful synthesis of LP, probability, and correctness.

### Q5. Minimum-cut structure and tie-breaking — high priority, 8 marks

(a) Fix any maximum flow `f`. Prove that for **every** minimum cut, outgoing arcs are saturated and incoming arcs carry zero flow. (b) Deduce that a positive-capacity arc cannot cross two minimum cuts in opposite directions. (c) For integer capacities, find a minimum cut with the fewest outgoing arcs.

Basis: Tutorial 4 Q3–4; Network Flow II.

### Q6. MAX-DICUT rounding and a changed probability — high priority, 8 marks

Use `z_ij<=x_i`, `z_ij<=1-x_j`, and variables in `[0,1]`, maximizing `sum w_ij z_ij`. Prove that independent rounding with `p_i=1/4+x_i/2` gives at least half the LP value. As an extension, determine whether direct rounding `p_i=x_i` can also guarantee half the optimum, assuming the LP solution is optimal.

Basis: Tutorial 3 Q3; the extension completes an argument already in your notes.

### Q7. MAX-SAT clause lengths and combination — high priority, 10 marks

Write the weighted MAX-SAT ILP and LP. Prove the LP-rounding bound for a clause of length `ell`, including the AM–GM and concavity steps. Then show why taking the better of LP rounding and fair random assignment obtains `3/4 OPT` in expectation.

Basis: L4-LP slides 11–23; L3 probability analysis.

### Q8. Metric TSP and the role of assumptions — core coverage, 8 marks

Prove the double-tree factor 2 and Christofides factor 3/2. Explain exactly where triangle inequality, even parity, and minimum-weight perfect matching are used. Give a four-vertex metric instance where a valid double-tree execution is nonoptimal. Explain why arbitrary nonmetric costs invalidate the shortcut argument.

Basis: L2; Tutorial 2 Q1–2. It is a substantial omission in the current notes even though the two finals do not directly test the full Christofides proof.

### Q9. Knapsack scaling — core coverage, 8 marks

Derive the value-indexed `O(n²V)` DP. Use it to obtain an FPTAS with `O(n³/epsilon)` arithmetic-operation complexity. Prove the guarantee and explain why overweight items must be removed before defining `V`.

Basis: Tutorial 1 Q2; L1.

### Q10. Vertex-disjoint routes — high priority, 8 marks

For distinct nonadjacent vertices `x,y` of an undirected graph, prove that the maximum number of internally vertex-disjoint `x–y` paths equals the minimum size of an `x–y` vertex separator. Give the flow construction and justify both correspondences. Why do capacity-1 splitting arcs suffice, and why are the other arcs assigned capacity larger than `n`?

Basis: Tutorial 4 Q2; Network Flow III vertex capacities and flow decomposition.

### Q11. Repair a flow algorithm — high priority, 8 marks

(a) A scaling algorithm starts at `k=C`, halves by real division, and stops below 1. Find an integer-capacity counterexample and repair the algorithm. (b) Prove that BFS augmentations give `O(nm²)` time independent of capacity magnitudes. (c) Explain why arbitrary Ford–Fulkerson is only pseudo-polynomial for integer capacities and may fail to terminate for irrational capacities.

Basis: Network Flow II–III; AY25/26 Q1D. The bug-finding part is an original practice variation.

### Q12. An unfamiliar objective with a familiar proof — medium priority, 6 marks

You are given `n` strings of length `L` over an alphabet of `q` symbols. Give a deterministic `1/q` retained-value approximation for the longest common subsequence of all strings, in `O(nL+q)` time. Give a second proof using a sum of per-symbol upper bounds on OPT.

Basis: AY25/26 Q3; L1's best-of-candidates technique. Focus on recognizing the proof pattern rather than memorizing the story.

### Quick true/false drill

For each, provide a proof or a small counterexample.

1. Complementing any 2-approximate vertex cover gives a constant-factor approximate maximum independent set.
2. Integer capacities imply every maximum flow is integral on every arc.
3. A feasible flow whose value equals the capacity of some cut is maximum.
4. An integer-capacity flow algorithm taking `O(mC)` time is polynomial in the binary input length.
5. A constant-factor approximation for MAX-SAT automatically gives one for minimizing unsatisfied clauses.
6. Shortest residual distances never decrease under arbitrary Ford–Fulkerson path choices.
7. A feasible solution of a minimization LP relaxation upper-bounds the integer optimum.
8. Every graph on `n` vertices has vertex-cover LP gap at least `2-2/n`.
9. Endpoints of a maximal matching always form a vertex cover; endpoints of a maximum matching always form a minimum cover.
10. The indicators of two edges sharing a vertex must be independent in order to sum their expectations.

## Answer sketches

### Q1

Use `s -> worker_i -> worker_period_(i,j) -> task_d -> t`. Capacities are respectively `b_i`, 1, 1, 1. Include a worker-period-to-task arc exactly when task `d` is in period `j` and in `A_i`. The target is the number of tasks `D`.

A schedule sends one unit along each corresponding path; all capacities and conservation constraints follow from the quotas. Conversely, choose an integral maximum flow. If its value is `D`, all `D` unit-capacity task-to-sink arcs are saturated. Read each unit on `(worker_period_(i,j),task_d)` as an assignment. The preceding capacity-1 arc enforces the per-period quota, and `s -> worker_i` enforces the total quota. Integrality guarantees indivisible assignments.

ILP: binary `x_id` for available pairs; minimize 0, with `sum_i x_id=1` for each task, `sum_d x_id<=b_i` for each worker, and `sum_{d in period j} x_id<=1` for each worker-period. Unavailable variables are absent or fixed to zero. The graph has `O(nk+D)` vertices and `O(n+nk+D+sum_i |A_i|)` arcs; one polynomial-time max-flow computation suffices. If the original question asks for at least one worker per task and all other constraints are upper bounds, remove excess assignments to obtain exactly one first.

### Q2

Minimize `sum_j c_j x_j` subject to `sum_{j:e in S_j}x_j>=1`, with binary variables. Relax to `[0,1]`, solve, and select all `x_j*>=1/k`. Each covering constraint has at most `k` terms, so one reaches the threshold. The cost is at most `sum_j k c_j x_j*=k LP*<=k OPT`.

Bounded **set size** restricts columns, not the number of terms in an element's covering constraint. Therefore the threshold feasibility proof does not follow. For example, a single element with two identical unit-cost singleton sets has a valid optimal LP solution `(1/2,1/2)`; set size is at most 1, but threshold 1 would select neither.

### Q3

Each edge crosses with probability `1-1/k`, so expected value is `(1-1/k)sum_e w_e >= (1-1/k)OPT`. Use the conditional-expectation rule described above. For a vertex being fixed, only edges to already fixed neighbors have label-dependent expected contributions. Pick the label maximizing the total weight of those neighbors assigned a different label. This preserves conditional expectation and yields the same guarantee deterministically. A direct implementation is polynomial, e.g. `O(k(n+m))`; more efficient bookkeeping is possible. Linearity of expectation does not require edge-indicator independence.

### Q4

For one element, one round's failure probability is `prod(1-x_j*) <= exp(-sum x_j*) <= e^-1`. After `r` independent rounds it is at most `e^-r<=1/(2n)`. By the union bound a trial succeeds with probability `q>=1/2`.

Expected cost of a trial is at most `r LP*`, because each set is included with probability `1-(1-x_j*)^r <= r x_j*`. The returned trial is distributed as a trial conditioned on success `A`, so for its nonnegative cost `C`,

\[
\mathbb E[C\mid A]=\frac{\mathbb E[C\mathbf1_A]}{q}
\le\frac{\mathbb E[C]}q\le2rLP^*\le2rOPT.
\]

Independent fresh trials have geometric stopping time of mean `1/q<=2`, terminate almost surely, and return a checked feasible cover. Trial cost and success can be correlated; that is why the conditioning factor is necessary.

### Q5

For any minimum cut with source side `S`,

\[
0=c(\delta^+(S))-|f|
=\sum_{e\in\delta^+(S)}(c_e-f_e)+\sum_{e\in\delta^-(S)}f_e.
\]

Every term is nonnegative, hence each is zero. A positive-capacity arc crossing outwards requires `f_e=c_e>0`; crossing inwards across another minimum cut would require `f_e=0`, impossible for the same fixed maximum flow.

For tie-breaking, let `m=|E|` and use `c'_e=(m+1)c_e+1`. A cut has transformed capacity `(m+1)c(S)+|delta^+(S)|`. Integer capacity differences of at least 1 dominate every possible cardinality difference (at most `m`), so original capacity is minimized first, then cardinality. Use one polynomial-time max-flow/min-cut computation; capacity encoding length increases by `O(log m)` bits.

### Q6

For each arc,

\[
\Pr[i\in U,j\notin U]=(1/4+x_i/2)(1/4+(1-x_j)/2)
\ge(1/4+z_{ij}/2)^2
=z_{ij}/2+(2z_{ij}-1)^2/16\ge z_{ij}/2.
\]

Sum against nonnegative weights. For direct rounding use the Cauchy–Schwarz argument and the feasible half-valued LP point from Correction 5. Optimality of the LP solution supplies `L>=W/2`; an arbitrary low-value feasible LP point need not have that property.

### Q7

Maximize `sum_i w_i y_i`, subject to `sum_{positive j in C_i}x_j + sum_{negative j in C_i}(1-x_j) >= y_i`, and binary truth/satisfaction variables. Relax their domains to `[0,1]`.

Let the literal-success probabilities in a non-tautological clause be `a_1,...,a_ell`, with sum at least `y_i*`. Independence and AM–GM give `prod_h(1-a_h) <= (1-sum_h a_h/ell)^ell <= (1-y_i*/ell)^ell`. The function `g(y)=1-(1-y/ell)^ell` is concave, `g(0)=0`, and hence `g(y)>=y g(1)` on `[0,1]`. Finish using the chain in Correction 3 and the clause-by-clause combination in Section D.

### Q8

Use the proofs in the TSP reference. For a nonoptimal double-tree execution, take cycle `A-B-D-C-A` with side costs 1 and both diagonal costs 2 (shortest-path metric of this cycle). Choose MST edges `AB,AC,CD`. Euler tour `A,B,A,C,D,C,A` shortcuts to `A,B,C,D,A`, cost 6. The original cycle costs 4 and is optimal because every tour uses four edges each of cost at least 1.

Without triangle inequality, a shortcut can be arbitrarily more expensive than the detour it replaces. For general-TSP gap reductions, give original graph edges cost 1 and nonedges cost `B>rho n`: a Hamiltonian cycle has cost `n`, whereas any tour in a no-instance uses a cost-`B` edge. An assumed fixed-factor `rho` approximation would distinguish these cases in polynomial time, contrary to `P != NP`.

### Q9

Use Correction 2. Include state meaning, base cases, recurrence, final-state selection, and reconstruction. The count is `n` rows times `O(nV)` values before scaling and `O(n²/epsilon)` values afterward. The error is additive at most `nK`, converted to a multiplicative error using `V<=OPT`. This uses the true input bit length plus the accuracy parameter; it is not justified by treating `V` as a constant.

### Q10

Split each internal vertex `v` into `v_in -> v_out` with capacity 1. Keep `x,y` unsplit. Replace each undirected edge `{u,v}` with arcs `u_out -> v_in` and `v_out -> u_in`, each capacity `M=n+1`, interpreting unsplit endpoints as themselves.

Internally disjoint paths give integral unit flows. Conversely, decompose an integral maximum flow into unit `x–y` paths and cycles; discard cycles. Capacity-1 splitting arcs ensure no internal vertex belongs to two paths.

Deleting a separator's splitting arcs destroys all flow paths; the reachable-set cut after deletion has capacity at most the separator size. Since `x,y` are nonadjacent, deleting all other vertices is a separator, so minimum cut capacity is at most `n-2<M`. Consequently a minimum cut cannot use an `M`-capacity arc. Its outgoing splitting arcs identify a separator of exactly the cut capacity. The two inequalities plus max-flow/min-cut prove the theorem. Adjacency matters: a direct `x–y` edge cannot be destroyed by deleting internal vertices.

### Q11

Use the two-arc `3,1` path counterexample and power-of-two repair from Correction 8. For shortest paths, new residual arcs reverse edges on the old shortest augmenting path and cannot create a shorter source distance; a shortest alleged violating path gives a contradiction at its first vertex with decreased distance. If `(u,v)` is saturated with distances `d(v)=d(u)+1`, its reappearance requires a later shortest path using `(v,u)`. Then `d_new(u)=d_new(v)+1>=d_old(v)+1=d_old(u)+2`. Thus each directed residual arc is saturated critically `O(n)` times.

With integer capacities, arbitrary augmentation increases flow by at least 1, giving `O(m|f*|)` time; `|f*|` may be exponential in its bit length. Irrational capacities can produce an infinite sequence of positive augmentations with decreasing amounts. Knowing this limitation and the integer bad example is higher priority than reproducing the long irrational construction.

### Q12

For each symbol `a`, compute `f_a=min_i count_a(string_i)`. Output `a` repeated `f_a` times for a symbol maximizing `f_a`. This is a common subsequence by construction. Some symbol occurs at least `OPT/q` times in an optimal common subsequence, so the returned length is at least that large. Alternatively, `OPT<=sum_a f_a<=q max_a f_a`.

For a fixed alphabet use frequency arrays. For variable `q`, maintain per-symbol string-occurrence counts and minima, touching only symbols occurring in each string; absent-in-some-string symbols have `f_a=0`. This avoids an `O(nq)` full-array reset and gives `O(nL+q)` time.

### True/false answers

1. **False.** `q` disjoint edges plus one isolated vertex: maximal-matching VC selects all edge endpoints, so its complement has size 1 while maximum IS has size `q+1`.
2. **False.** Use unit arcs `s->a`, `s->b`, `a->c`, `b->c`, `c->t`. A maximum flow sends `1/2` down each branch and 1 on `c->t`; an integral maximum also exists.
3. **True.** Any flow value is at most any cut capacity, so equality certifies both optima.
4. **False.** `C` requires only `O(log C)` bits.
5. **False.** A satisfiable two-literal clause has optimum unsatisfied count zero, but fair assignment leaves it unsatisfied with probability 1/4. An expected finite multiplicative approximation at optimum zero would require expected cost zero.
6. **False.** The monotonicity proof depends on choosing shortest augmenting paths. For an explicit example, take unit arcs along `s->a->b->c->d->t`, together with `s->d`. Initially `dist(s,c)=3`. Augment on the long path. The unused arc `s->d` and the new reverse residual arc `d->c` give distance 2 to `c`, so its distance decreases.
7. **False.** The **optimal** minimization LP value lower-bounds the integer optimum. An arbitrary fractional feasible objective is neither automatically an upper nor a lower bound on it. An integral feasible solution does give an upper bound.
8. **False.** `2-2/n` is a lower bound on the worst-case gap over instances, witnessed by `K_n`, not a claim about every graph. On `K_n`, integral optimum is `n-1`, and LP optimum is `n/2` (sum all edge constraints). Bipartite examples can have gap 1.
9. **First true, second false.** Maximality proves coverage. Even one edge has a maximum matching whose two endpoints are a cover of size 2, while minimum cover size is 1.
10. **False.** Linearity of expectation holds without independence.

## Organize the open-book packet

Put a one-page technique index first, followed by complete core entries grouped by course topic. Place a compact counterexample sheet near the front: oversized-item handling, greedy knapsack failures, disjoint-edge VC/IS complement, zero-optimum unsatisfied SAT, nonmetric shortcuts, fractional maximum flows, and capacity-scaling threshold failure.

Move the long trigonometric Taylor series, number theory, CRT, primitive roots, sieves, coupon collector, and planar-minor material to an appendix. Retain the inequalities that the supplied material actually uses: linearity, union bound, `1-x<=e^-x`, AM–GM, concavity/Jensen, Cauchy–Schwarz, geometric stopping times, and `H_n<=1+ln n`.

Replace “trivial” or unfinished proofs with short, reproducible arguments. For a flow reduction, draw and label the layers and write the target flow value. For an approximation, put the complete inequality chain on one line if possible. State the guarantee explicitly as `ALG<=rho OPT` or `E[ALG]>=alpha OPT` so the two ratio conventions cannot be confused.

Suggested work order: correct the existing errors; add flow reductions and cut identities; add conditional expectation and Tutorial 4; add TSP; finish FPTAS and MAX-SAT; then attempt Q1–Q7 and the true/false drill using only the packet.

## Source issues to keep separate from your notes

The source material itself is not error-free. In addition to the L4-LP slide 21 typos and scaling stopping condition already described:

- L1 slide 23 prints the knapsack constraint with `>= W`; the intended capacity constraint is `<= W`, as the surrounding slides show.
- AY25/26 solution Q4C refers to an arc `(u_i,v)` that is absent from its construction; use `(u_{i,j},v)`. If a schedule covers days redundantly, first discard excess assignments before constructing the unit flow per day.
- AY25/26 solution Q5A's triangle-only feasibility test is insufficient. Five vertices constrained to opposite sides around a 5-cycle violate 2-colourability but contain no forbidden triangle. Correct feasibility uses parity propagation/BFS over all same/different constraints. Q5's SDP parts are outside the supplied lecture scope, so do not spend core revision time on that solution.
- AY24/25 Q6's dual variables are fractional cover variables; obtaining an integral minimum vertex cover also needs the bipartite integrality property, not strong duality alone. This is another reason to distinguish in-scope matching-by-flow from later LP-duality material.

## Source map

All page numbers are PDF page numbers. Only the supplied local materials were needed for this review.

- [Current notes](/Users/wxy/Desktop/CS4234/CS4234.pdf), especially pp. 14–36; [editable source](/Users/wxy/Desktop/CS4234/CS4234.tex).
- [AY24/25 final and solutions](</Users/wxy/Downloads/CS4234_AY24_25_final-with-solution (2).pdf>): questions pp. 1–2; solutions pp. 3–8.
- [AY25/26 final and solutions](/Users/wxy/Downloads/CS4234_AY25_26_final-with-solution.pdf): questions pp. 3–6; solutions pp. 9–16.
- [Merged tutorials](</Users/wxy/Downloads/tutorial1 (1)_merged.pdf>): T1 pp. 1–6; T2 pp. 7–14; T3 pp. 15–20; T4 pp. 21–28.
- [L1](/Users/wxy/Desktop/CS4234/references/L1-intro-to-approx.pdf): approximation conventions, knapsack, FPTAS motivation, best-of-candidates.
- [L2](/Users/wxy/Desktop/CS4234/references/L2-VC-TSP.pdf): set cover pp. 2–19; VC pp. 21–32; TSP pp. 33–72.
- [L3](/Users/wxy/Desktop/CS4234/references/L3-MAX-SAT.pdf): MAX-CUT pp. 3–15; MAX-SAT pp. 16–36; derandomization pp. 37–44.
- [L4-LP](/Users/wxy/Desktop/CS4234/references/L4-LP-MAX-SAT.pdf): VC threshold rounding p. 8; MAX-SAT LP/rounding pp. 11–23.
- [Network Flow I](/Users/wxy/Downloads/L4-NetworkFlow1.pdf): definitions and the failure of forward-only greedy augmentation.
- [Network Flow II](/Users/wxy/Downloads/L5-NetworkFlow2.pdf): residuals pp. 11–21; flow/cut identities pp. 23–39; Ford–Fulkerson limits pp. 40–48.
- [Network Flow III](/Users/wxy/Downloads/L6-NetworkFlow3.pdf): scaling pp. 6–15; shortest augmenting paths pp. 16–33; scope p. 36; applications, integrality and decomposition pp. 37–61.
