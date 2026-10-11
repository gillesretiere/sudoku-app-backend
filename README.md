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
##### Stratégies de niveau 1
gcc -Wall -Wextra -std=c11 -DGAME_MODE game.c main.c find_naked_single.c find_hidden_single.c candidates.c ../Generator/*.c -I. -I../Generator -o sudoku_game
##### Stratégies de niveau 2
gcc -Wall -Wextra -std=c11 -DGAME_MODE game.c main.c candidates.c find_naked_single.c find_hidden_single.c find_naked_pair.c find_hidden_pair.c find_pointing_pairs.c find_box_line_reduction.c find_naked_triple.c ../Generator/*.c -I. -I../Generator -o sudoku_game
##### Statégies de niveau 3
 gcc -Wall -Wextra -std=c11 -DGAME_MODE game.c main.c candidates.c bank_loader.c grid_transform.c find_naked_single.c find_hidden_single.c find_naked_pair.c find_hidden_pair.c find_pointing_pairs.c find_box_line_reduction.c find_naked_triple.c find_x_wing.c ../Generator/*.c -I. -I../Generator -o sudoku_game
#### Générateur de grilles
gcc -Wall -Wextra -std=c11 -DGAME_MODE generate_bank.c game.c candidates.c solver_eval.c bank_loader.c grid_transform.c find_naked_single.c find_hidden_single.c find_naked_pair.c find_hidden_pair.c find_pointing_pairs.c find_box_line_reduction.c find_naked_triple.c find_x_wing.c ../Generator/*.c -I. -I../Generator -o build_bank