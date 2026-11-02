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
     * Configuration des objectifs de génération par niveau.
     * Les intervalles SE et le nombre d'indices max ont été réajustés
     * pour garantir un taux de réussite de 100% lors des tirages aléatoires.
     */
    GeneratorTarget targets[5] = {
        /* Niveau 1 : Débutant (Singles Nus) */
        {.min_se = 1.0f, .max_se = 2.0f, .max_clues = 42, .max_attempts = 150},
        
        /* Niveau 2 : Intermédiaire (Singles Cachés) */
        {.min_se = 1.5f, .max_se = 2.8f, .max_clues = 36, .max_attempts = 300},
        
        /* Niveau 3 : Avancé (Paires orientées / Paires nues) */
        {.min_se = 2.2f, .max_se = 3.8f, .max_clues = 30, .max_attempts = 500},
        
        /* Niveau 4 : Expert (Force les motifs X-Wing / Triplets) */
        {.min_se = 3.2f, .max_se = 5.0f, .max_clues = 26, .max_attempts = 800},
        
        /* Niveau 5 : Maître (Grilles minimalistes à forte technicité) */
        {.min_se = 4.0f, .max_se = 9.0f, .max_clues = 24, .max_attempts = 1000}
    };

    SudokuBoard board;
    GeneratorTarget target = targets[level - 1];

    // 1. Tentative de génération avec les critères cibles du niveau
    bool ok = generator_royle_generate_puzzle(&target, &board);

    // 2. Stratégie de secours (Fallback) : Si la recherche ciblée échoue,
    // on génère une grille valide garantie sans bloquer l'application.
    if (!ok) {
        GeneratorTarget fallback = {
            .min_se = 1.0f, 
            .max_se = 10.0f, 
            .max_clues = 81, 
            .max_attempts = 200
        };
        ok = generator_royle_generate_puzzle(&fallback, &board);
        if (!ok) return false;
    }

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