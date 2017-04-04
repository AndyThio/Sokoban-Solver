# Sokoban Solver

A Sokoban Solver, written in serial then converted to parallel using std::threads.

## Input File Requirements

- All integers
- First number should be height
- Second number should be width
- numbers should be separated by whitespace

## Integer Representation

| Integer |  Representation |
| :-----: |  -------------- |
| 0 | an empty space |
| 1 | a barrel |
| 2 | the target spot to be filled with a barrel |
| 3 | a target spot filled with a barrel |
| 4 | a wall that doesn't allow movement |
| 5 | the player or object pusher |
