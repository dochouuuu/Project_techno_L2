# net-a46

## Description

This project is a **single-player puzzle game** inspired by *Net* from **Simon Tatham’s Portable Puzzle Collection**.  
It was developed as part of a **second-year (L2) Project Techno course**, carried out over **two academic semesters**.

The goal of the game is to connect all the pieces of a grid to form a coherent network, where each piece must be correctly oriented to establish connections with its adjacent pieces. The player can rotate pieces to solve the puzzle.

The project is divided into several files:
- `game.h`, `game_aux.h`, and `game_ext.h` define the game interface, including functions to manipulate pieces, check the grid state, and control orientations.
- `game_struct.h` defines the internal game data structures.
- `queue.h` and `queue.c` handle the **undo** and **redo** stacks.
- `game.c`, `game_aux.c`, and `game_ext.c` implement the functions declared in the header files.
- `game_text.c` allows the game to be played in **text mode** via the terminal.
- Several test files verify the correctness of each part of the game.

The project uses **CMake** for build configuration, as well as **Make** and **CTest** to automate compilation and testing.

---

## Authors

- Eliane Chaal  
- Tran Minh Chau Do  
- Léa Drion  

---

## Commands

To play the game, enter one of the following commands in the terminal:

- **Rotate the piece at (i, j) clockwise**: `c i j`
- **Rotate the piece at (i, j) counterclockwise**: `a i j`
- **Shuffle the game**: `r`
- **Undo the last move**: `z`
- **Redo the last move**: `y`
- **Display help**: `h`
- **Quit the game**: `q`

---

## Customizable Parameters

The following parameters can be customized:

- **Grid width and height**  
  Size of the grid in number of cells.

- **Connected borders (wrapping)**  
  When enabled, connections can wrap around the grid, allowing links between the left/right and top/bottom edges.
