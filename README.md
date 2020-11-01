# Nonogram Solver - C++

A Nonogram solver in C++. See the Wikipeida [article][nonogram_wiki].

[nonogram_wiki]: https://en.wikipedia.org/wiki/Nonogram 

Run "main.exe --help" on the cmd line to see options.


## Algorithm 
### Complete solver
Based on ideas by [Rosetta Code][rosetta_code].

Algorithm
- generate all possible rows and columns. This is done using unique integr partition permutations. This is very slow and memory intenstive.
- do row and column sweeps
	- discard possible rows/columns which do not fit in the current puzzle.
	- determine if there are any cells that are the same colour in all possibilities. 
- if there are more than 1 possible row/column after constraint propagation, try place possible rows, and see if all possible columns fit. This can be very slow and memory intenstive. 

### Fast solver
Based on ideas by [Jan Wolter][Wolter_survey] and [Steve Simpson][lancaster_solver].
Algorithm
- do constraint propagation with row and column sweeps
	- find the left-most and right-most match for each row/column. This is done using a Nondeterministic Finite State Machine and Thompsons algorithm. This is an O(n^2) algorithm.
	- find intersections. This is done by finding a "changer sequence". See the example below. It is made by replacing each symbol with a counter which increments everytime the symbols change e.g. from BLACK to WHITE. Counter values which are in the same index in both left and right are overlaps.
	- a simple filler which adds very simple clues that the left-right algorithm sometimes misses e.g. a white after ending a black sequence.
- if this fails, guess and then go back to constraint propagation
	- first try to find find contradictions -> guesses which are obviously wrong. This will happen if there are no matches for the line matcher. The opposite guess is therefore true.
	- otherwise save the current grid and make a binary guess. Backtrack if it is wrong. This is O(2^n), so it can very easily lead down a never ending spiral of guesses. 

Example by [Steve Simpson][lancaster_fast]	
Length:   24
Rule:     1,1,5
Original: >---#---------#-----#####<
Broken:   >---#--         -      # <
      left>000122344444444444555556<
     right>000122222222222223455555<
      fast>---#--+++++++++-+++####+<


[Wolter_survey]: https://webpbn.com/survey/
[lancaster_solver]: http://scc-forge.lancaster.ac.uk/open/nonogram/
[lancaster_fast]: http://scc-forge.lancaster.ac.uk/open/nonogram/ls-fast
[rosetta_code]: https://rosettacode.org/wiki/Nonogram_solver

## Input files
### Solve collection
Solve a collection of puzzles in a single file.

Format is:
---
General description (this line is skipped by the file reader)
-- puzzle title (this line is skipped by the file reader)
[run rows]
[run cols]
-- next puzzle title (this line is skipped by the file reader)
...
---

The runs are given by sets of unicode characters, with A=1, B=2 and so on. Spaces represent a line break.
While this can work for a numbers greater than 26, these will be encoded as non-Basic Latin unicode symbols.

### Solve puzzle
Based on Steve Simpsons .non format.

Format is:

---
text    (this line is skipped by the file reader)
title  (this line is skipped by the file reader)

width w (this line is skipped by the file reader)
height h (this line is skipped by the file reader)

rows 
x, x, x
....

columns
x, x, x
...

---








