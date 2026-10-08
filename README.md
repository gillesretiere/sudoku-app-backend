# Apress Source Code

This repository accompanies [*Sudoku Programming with C*](http://www.apress.com/9781484209967) by Giulio Zambon (Apress, 2015).

![Cover image](9781484209967.jpg)

Download the files as a zip using the green button, or clone the repository to your machine using Git.

## Releases

Release v1.0 corresponds to the code in the published book, without corrections or updates.

## Contributions

See the file Contributing.md for more information on how you can contribute to this repository.

## Adaptation
### Compilation des sources
#### Game
gcc -Wall -Wextra -std=c11 -DGAME_MODE game.c main.c find_naked_single.c find_hidden_single.c ../Generator/*.c -I../Generator -o sudoku_game

