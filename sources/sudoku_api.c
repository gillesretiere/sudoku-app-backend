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
        /* Niveau 1 : Débutant */
        {.min_se = 1.0f, .max_se = 2.0f, .max_clues = 40, .max_attempts = 100},
        /* Niveau 2 : Intermédiaire */
        {.min_se = 1.6f, .max_se = 2.6f, .max_clues = 34, .max_attempts = 300},
        /* Niveau 3 : Avancé */
        {.min_se = 2.4f, .max_se = 3.6f, .max_clues = 28, .max_attempts = 600},
        /* Niveau 4 : Expert */
        {.min_se = 3.2f, .max_se = 4.6f, .max_clues = 24, .max_attempts = 1200},
        /* Niveau 5 : Maître */
        {.min_se = 4.0f, .max_se = 9.0f, .max_clues = 22, .max_attempts = 2000}
    };

    SudokuBoard board;
    GeneratorTarget target = targets[level - 1];

    // 1. Tentative avec les exigences strictes
    bool ok = generator_royle_generate_puzzle(&target, &board);

    // 2. Fallback sécurisé : conserve un SE minimum proportionnel au niveau
    if (!ok) {
        float min_fallback_se = (level >= 4) ? 3.0f : ((level == 3) ? 2.0f : 1.0f);
        GeneratorTarget fallback = {
            .min_se = min_fallback_se, 
            .max_se = 10.0f, 
            .max_clues = (level >= 4) ? 25 : 81, 
            .max_attempts = 500
        };
        ok = generator_royle_generate_puzzle(&fallback, &board);
        if (!ok) return false;
    }

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