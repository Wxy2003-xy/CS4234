from pathlib import Path

base = Path('/Users/wxy/Desktop/CS4234')
draft = base / 'tmp/pdfs/notes-edit'
old = (draft / 'CS4234-before.tex').read_text()

def between(start, end):
    return old[old.index(start):old.index(end)]

preamble = old[:old.index(r'\begin{document}')]
preamble = preamble.replace(r'\usepackage{tikz}', r'\usepackage{tikz}' + '\n' + r'\usetikzlibrary{arrows.meta,positioning}')
preamble += '\n' + r'\setcounter{tocdepth}{2}' + '\n' + r'\emergencystretch=2em' + '\n' + r'\allowdisplaybreaks[1]' + '\n'

knapsack = between(r'\subsection{Example: Greedy algorithms on 0/1 knapsack}', r'\subsection{Strategy: Pick the best among candidate algorithm results}')
knapsack = knapsack.replace(r'\subsection{Example: Greedy algorithms on 0/1 knapsack}', r'\subsection{Greedy algorithms for 0/1 knapsack}\label{gre:knapsack}')
knapsack = knapsack.replace('Counter example', 'Counterexample')
knapsack = knapsack.replace('unbounded loss into', 'unbounded loss into')
knapsack = knapsack.replace('Consider \n\\begin{align*}', 'For an integer capacity $W>2$, consider\n\\begin{align*}')
knapsack = knapsack.replace('Let the capacity be $W>2$', 'Let the capacity be an integer $W>2$')
knapsack = knapsack.replace('can give us arbitrarily bad result', 'can give an arbitrarily bad result')

setcover = between(r'\subsection{Set cover}', r'\subsection{Vertex Cover as a special case of Set cover}')
setcover = setcover.replace(r'\subsection{Set cover}', r'\subsection{Weighted set cover: greedy charging}\label{gre:setcover}', 1)
setcover += r'''
\paragraph{Running time.}
Each iteration covers at least one new element, so there are at most $n=|U|$ iterations.
If $m$ candidate sets are represented by their incidence matrix, recomputing all marginal
scores costs $O(mn)$ per iteration, for $O(mn^2)$ time. Faster updates are possible, but this
already establishes polynomial time. If $U$ is empty, return the empty cover.
'''

prereq = between(r'\section*{Introduction and Prerequisites}', r'\section{Formulae and Theorems}')
prereq = prereq.replace(r'\section*{Introduction and Prerequisites}', r'\section{Background and Prerequisites}\label{app:background}', 1)
formulas = between(r'\section{Formulae and Theorems}', r'\section*{Discrete Math Theorems Frequently Used in Competitive Programming}')
formulas = formulas.replace(r'\section{Formulae and Theorems}', r'\section{Extended Probability and Mathematical Reference}\label{app:formulas}', 1)
discrete = between(r'\section*{Discrete Math Theorems Frequently Used in Competitive Programming}', r'\section{Lecture 1: Approximation Algorithm}')
discrete = discrete.replace(r'\section*{Discrete Math Theorems Frequently Used in Competitive Programming}', r'\section{Discrete Mathematics Reference}\label{app:discrete}', 1)
discrete = discrete.replace("Eular's formula", "Euler's formula")
boolean = between(r'\section{Linear programming and integer programming}', r'\subsubsection*{Example: Vertex cover}')
boolean = boolean.replace(r'\section{Linear programming and integer programming}', r'\section{Boolean-to-ILP Encoding Reference}\label{app:boolean}', 1)
boolean = boolean.replace(r'\subsection{ILP formulation from finite space FOL}', r'\subsection{Binary encodings of logical constraints}', 1)
boolean += '\nAll variables in the table are binary. These encode constraints on assignments; an LP relaxation need not preserve logical equivalence.\n'

pathological = between(r'\subsection{Pathological example of naive FF}', r'\begin{lemma}[Integrality of max-flow]')
pathological = pathological.replace(r'\subsection{Pathological example of naive FF}', r'\section{Additional Algorithm Examples}\label{app:extra}' + '\n' + r'\subsection{An infinite Ford--Fulkerson augmentation sequence}', 1)
pathological = pathological.replace('FF may not terminate when edge capacity are real, when a particular pathological path choices are made. Consider the following graph:', r'''Ford--Fulkerson can fail to terminate with irrational capacities and adversarial path choices.
In this construction let $r=(\sqrt5-1)/2$, so $r^2=1-r$, and choose $M\ge6$.
The detailed infinite sequence is an extension of the core termination discussion.''')
pathological = pathological.replace(r'\usetikzlibrary{arrows.meta,positioning}', '')
pathological += r'''
\paragraph{Why the outer arcs never become the bottleneck.}
After the initial augmentation of size 1, the augmentation amounts are
$r,r,r^2,r^2,r^3,r^3,\ldots$. Their total is
$1+2r/(1-r)<5$. Thus every capacity-$M$ outer arc retains forward residual capacity
greater than 1 when $M\ge6$, while every prescribed bottleneck is at most 1.
This validates the entire infinite sequence, not just the recurrence on the three special arcs.
'''

flow = (draft/'flow.tex').read_text()
flow, flow_extension = flow.split(r'\subsection{Optional extension: lower bounds and vertex demands}', 1)
flow += r'''
\paragraph{Lower bounds and vertex demands.}
The variants $\ell_e\le f_e\le c_e$ and
$\operatorname{in}_f(v)-\operatorname{out}_f(v)=d(v)$ impose minimum arc flows
and net supply/demand at vertices. Their full feasibility reduction is in
Appendix~\ref{nf:demands}.
'''
flow_extension = r'\section{Flow Extensions: Lower Bounds and Demands}' + flow_extension

parts = [
    preamble, r'\begin{document}\maketitle\tableofcontents\clearpage',
    (draft/'front.tex').read_text(),
    r'\clearpage\section{Greedy Approximation and Scaling}\label{gre:main}' + '\n' +
    r'For each approximation below, feasibility and comparison with an optimal solution are separate proof obligations. Assume nonnegative values and costs, and positive item weights where density is used.',
    knapsack, (draft/'greedy-extra.tex').read_text(), setcover, (draft/'packing.tex').read_text(),
    r'\clearpage', (draft/'tsp.tex').read_text(),
    r'\clearpage', (draft/'random-lp.tex').read_text(),
    r'\clearpage', flow,
    r'\clearpage\appendix', (draft/'practice.tex').read_text(),
    r'\clearpage', prereq, r'\clearpage', formulas, r'\clearpage', discrete,
    r'\clearpage', boolean, r'\clearpage', flow_extension,
    r'\clearpage', pathological, (draft/'optional.tex').read_text(),
    r'\end{document}'
]
(base/'CS4234.tex').write_text('\n\n'.join(parts)+'\n')
print('Assembled self-contained CS4234.tex:', len((base/'CS4234.tex').read_text().splitlines()), 'lines')
