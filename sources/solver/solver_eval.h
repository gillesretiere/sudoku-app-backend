#ifndef SOLVER_EVAL_H
#define SOLVER_EVAL_H

#include "core_board.h"
#include <stdbool.h>

/**
 * @brief Structure décrivant le bilan d'évaluation d'une grille.
 */
typedef struct {
    float max_se_rating;   /* Score SE maximal de la règle requise (ex: 1.5, 3.2, 4.2...) */
    int total_steps;       /* Nombre d'étapes logiques exécutées */
    int complex_steps;     /* Nombre d'étapes nécessitant une règle SE >= 3.0 */
    bool is_solvable;      /* Vrai si la grille se résout entièrement par la logique */
} SudokuEvaluation;

/**
 * @brief Analyser et évaluer la difficulté d'une grille selon les règles de Denis Berthier.
 * 
 * @param board Grille de Sudoku à analyser
 * @param result Structure recevant le rapport complet d'évaluation
 */
void solver_eval_analyze(const SudokuBoard *board, SudokuEvaluation *result);

#endif /* SOLVER_EVAL_H */