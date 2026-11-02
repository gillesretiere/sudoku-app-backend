#include "sudoku_api.h"
#include "core/core_board.h"
#include "solver/solver_dlx.h"
#include "solver/solver_eval.h"
#include "generator/generator_royle.h"
#include <string.h>

bool sudoku_api_generate_level(int level, SudokuPuzzleDTO *out_puzzle) {
    if (!out_puzzle || level < 1 || level > 5) return false;

    memset(out_puzzle, 0, sizeof(SudokuPuzzleDTO));

    /* 
     * Recalibrage des objectifs de génération par niveau (SE Rating & Nombre d'indices max)
     * Pour les niveaux 4 et 5, le nombre d'indices est fortement réduit afin d'éliminer
     * les résolutions trop simples par paires orientées et forcer des motifs complexes (X-Wing, etc.).
     */
    GeneratorTarget targets[5] = {
        /* Niveau 1 : Débutant (Singles Nus) */
        {.min_se = 1.0f, .max_se = 1.5f, .max_clues = 38, .max_attempts = 300},
        
        /* Niveau 2 : Intermédiaire (Singles Cachés) */
        {.min_se = 1.6f, .max_se = 2.2f, .max_clues = 32, .max_attempts = 500},
        
        /* Niveau 3 : Avancé (Paires orientées / Paires nues) */
        {.min_se = 2.3f, .max_se = 3.5f, .max_clues = 27, .max_attempts = 1500},
        
        /* Niveau 4 : Expert (Force l'apparition de X-Wing / Triplets) */
        {.min_se = 3.6f, .max_se = 4.8f, .max_clues = 23, .max_attempts = 4000},
        
        /* Niveau 5 : Maître (Grilles minimales à haute technicité) */
        {.min_se = 4.9f, .max_se = 8.0f, .max_clues = 20, .max_attempts = 8000}
    };

    SudokuBoard board;
    bool ok = generator_royle_generate_puzzle(&targets[level - 1], &board);
    if (!ok) return false;

    /* Extraire la solution unique via l'algorithme DLX */
    uint8_t sol[BOARD_SIZE][BOARD_SIZE];
    solver_dlx_has_unique_solution(&board, sol);

    /* Évaluer le score SE (Sudoku Explainer) final de la grille */
    SudokuEvaluation eval;
    solver_eval_analyze(&board, &eval);

    /* Remplir le DTO d'exportation pour le frontend */
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