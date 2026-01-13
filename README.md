# net-a46

## Description

This project is a **single-player puzzle game** inspired by *Net* from **Simon Tatham’s Portable Puzzle Collection**.  
The goal of the game is to connect the pieces of a grid to form a coherent network, where each piece must be correctly oriented to establish connections with its adjacent pieces. The player can rotate the pieces to solve the puzzle.

The project is divided into several parts:
- `game.h` and `game_aux.h` define the game interface, including functions to manipulate pieces, check the state of the grid, and control piece orientations.
- `game.c` and `game_aux.c` implement the functions declared in the header files.
- Several test files are provided to verify the correct behavior of each part of the game.

The project uses **CMake** for build configuration and **Make** to automate compilation and testing.

---

## Authors

- Eliane Chaal  
- Tran Minh Chau Do  
- Léa Drion  
