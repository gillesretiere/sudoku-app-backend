#include "sudoku_api.h"
#include "core/core_board.h"
#include "solver/solver_dlx.h"
#include "solver/solver_eval.h"
#include "generator/generator_royle.h"
#include <string.h>

bool sudoku_api_generate_level(int level, SudokuPuzzleDTO *out_puzzle) {
    if (!out_puzzle || level < 1 || level > 5) return false;

    memset(out_puzzle, 0, sizeof(SudokuPuzzleDTO));

    GeneratorTarget targets[5] = {
        {.min_se = 1.0f, .max_se = 1.5f, .max_clues = 40, .max_attempts = 200},
        {.min_se = 2.0f, .max_se = 3.2f, .max_clues = 35, .max_attempts = 300},
        {.min_se = 3.3f, .max_se = 4.1f, .max_clues = 31, .max_attempts = 1000},
        {.min_se = 4.2f, .max_se = 5.1f, .max_clues = 28, .max_attempts = 3000},
        {.min_se = 5.2f, .max_se = 8.0f, .max_clues = 26, .max_attempts = 6000}
    };

    SudokuBoard board;
    bool ok = generator_royle_generate_puzzle(&targets[level - 1], &board);
    if (!ok) return false;

    /* Extraire la solution unique via DLX */
    uint8_t sol[BOARD_SIZE][BOARD_SIZE];
    solver_dlx_has_unique_solution(&board, sol);

    /* Évaluer le score SE final */
    SudokuEvaluation eval;
    solver_eval_analyze(&board, &eval);

    /* Remplir le DTO de retour */
    int clues = 0;
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            out_puzzle->cells[r][c] = board.cells[r][c].value;
            out_puzzle->solution[r][c] = sol[r][c];
            if (board.cells[r][c].value != 0) clues++;
        }
    }

    out_puzzle->se_rating = eval.max_se_rating;
    out_puzzle->clues_count = clues;
    out_puzzle->is_valid = true;

    return true;
}