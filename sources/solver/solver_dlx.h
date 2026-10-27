#ifndef SOLVER_DLX_H
#define SOLVER_DLX_H

#include "core_board.h"
#include <stdbool.h>

/**
 * @brief Vérifie si la grille possède exactement une solution unique.
 * 
 * @param board Grille de Sudoku à analyser
 * @param solution_out Matrice 9x9 pour recevoir la solution (si unique, peut être NULL)
 * @return true si la grille a une solution unique, false sinon (0 ou >1 solutions)
 */
bool solver_dlx_has_unique_solution(const SudokuBoard *board, uint8_t solution_out[BOARD_SIZE][BOARD_SIZE]);

/**
 * @brief Compte le nombre de solutions d'une grille jusqu'à un nombre maximum.
 * 
 * @param board Grille de Sudoku à analyser
 * @param max_count Limite d'arrêt du comptage (ex: 2 pour s'arrêter dès que non-unique)
 * @return Nombre de solutions trouvées (<= max_count)
 */
int solver_dlx_count_solutions(const SudokuBoard *board, int max_count);

#endif /* SOLVER_DLX_H */