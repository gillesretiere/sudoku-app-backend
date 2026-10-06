#ifndef GENERATOR_H
#define GENERATOR_H

/**
 * Génère une grille de Sudoku et sa solution en réutilisant le moteur de sudoku_gen.c
 * 
 * @param puzzle_out Tableau 9x9 recevant la grille de départ (0 = case vide)
 * @param solution_out Tableau 9x9 recevant la solution complète
 */
void generate_sudoku(char puzzle_out[9][9], char solution_out[9][9]);

#endif /* GENERATOR_H */