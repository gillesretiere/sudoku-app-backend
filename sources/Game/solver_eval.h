#ifndef SOLVER_EVAL_H
#define SOLVER_EVAL_H

#include "game.h"

typedef struct {
    int level;         /* 0, 1, 2, 3 ou -1 (rejetée) */
    int x_wing_count;  /* Nombre de X-Wings réellement nécessaires */
} EvalResult;

/**
 * Évalue la difficulté exacte d'une grille et compte les X-Wings requis.
 */
EvalResult evaluate_puzzle_difficulty(const SudokuGame *game);

#endif