#ifndef SOLVER_EVAL_H
#define SOLVER_EVAL_H

#include <stdbool.h>
#include "game.h"

typedef struct {
    int level;
    int x_wing_count;
    int y_wing_count;
    int swordfish_count;
    int unique_rectangle_count;
    bool is_solved;
} EvalResult;

EvalResult evaluate_puzzle_difficulty(const SudokuGame *game);

#endif /* SOLVER_EVAL_H */