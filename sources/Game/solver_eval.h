#ifndef SOLVER_EVAL_H
#define SOLVER_EVAL_H

#include <stdbool.h>
#include "game.h"

typedef struct {
    int level;
    int x_wing_count;
    int y_wing_count;
    bool is_solved; /* Vrai si la grille est entièrement résolue par la logique */
} EvalResult;

EvalResult evaluate_puzzle_difficulty(const SudokuGame *game);

#endif /* SOLVER_EVAL_H */