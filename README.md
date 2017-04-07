# Sokoban Solver

A Sokoban Solver, written in serial then converted to parallel using std::threads.

## Input File Requirements

- All integers
- First number should be height
- Second number should be width
- Numbers should be separated by whitespace
- The puzzle is a rectangular/square shape
- The border wall of the puzzle is not inputed (going out of bounds would be considered hitting a wall)

## Integer Representation

| Integer |  Representation |
| :-----: |  -------------- |
| 0 | An empty space |
| 1 | A barrel |
| 2 | The target spot to be filled with a barrel |
| 3 | A target spot filled with a barrel |
| 4 | A wall that doesn't allow movement |
| 5 | The player or object pusher |
| 6 | Player standing on a target |
