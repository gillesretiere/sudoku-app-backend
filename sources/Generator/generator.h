#ifndef GENERATOR_H
#define GENERATOR_H

/**
 * Génère une grille de Sudoku unique ainsi que sa solution complète.
 * 
 * @param puzzle_out Tableau 9x9 récepteur pour la grille de jeu (avec cases vides à 0)
 * @param solution_out Tableau 9x9 récepteur pour la solution finale
 */
void generate_sudoku(char puzzle_out[9][9], char solution_out[9][9]);

#endif /* GENERATOR_H */