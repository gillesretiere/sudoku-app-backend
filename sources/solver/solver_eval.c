#include "solver_eval.h"
#include <string.h>

/* Scores de difficulté SE (Sudoku Explainer Rating) selon Denis Berthier */
#define SE_SINGLE        1.5f
#define SE_PAIR          2.4f
#define SE_POINTING      2.8f
#define SE_BOX_LINE      3.0f
#define SE_TRIPLE        3.6f
#define SE_X_WING        3.8f
#define SE_Y_WING        4.2f

/* --- Stratégies de base (Singles) --- */

static bool apply_naked_singles(SudokuBoard *work_board) {
    bool progress = false;
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (work_board->cells[r][c].value == 0) {
                uint8_t val = board_get_single_candidate(work_board, r, c);
                if (val != 0) {
                    board_set_value(work_board, r, c, val);
                    board_recompute_all_candidates(work_board);
                    progress = true;
                }
            }
        }
    }
    return progress;
}

static bool apply_hidden_singles(SudokuBoard *work_board) {
    bool progress = false;

    for (int d = 1; d <= 9; d++) {
        /* Vérification par ligne */
        for (int r = 0; r < BOARD_SIZE; r++) {
            int count = 0, last_c = -1;
            for (int c = 0; c < BOARD_SIZE; c++) {
                if (work_board->cells[r][c].value == 0 && board_has_candidate(work_board, r, c, d)) {
                    count++;
                    last_c = c;
                }
            }
            if (count == 1) {
                board_set_value(work_board, r, last_c, d);
                board_recompute_all_candidates(work_board);
                progress = true;
            }
        }

        /* Vérification par colonne */
        for (int c = 0; c < BOARD_SIZE; c++) {
            int count = 0, last_r = -1;
            for (int r = 0; r < BOARD_SIZE; r++) {
                if (work_board->cells[r][c].value == 0 && board_has_candidate(work_board, r, c, d)) {
                    count++;
                    last_r = r;
                }
            }
            if (count == 1) {
                board_set_value(work_board, last_r, c, d);
                board_recompute_all_candidates(work_board);
                progress = true;
            }
        }
    }
    return progress;
}

/* --- Stratégies d'intersections et de sous-ensembles --- */

static bool apply_pointing_pairs(SudokuBoard *work_board) {
    bool progress = false;

    for (int box = 0; box < BOARD_SIZE; box++) {
        int box_r = (box / 3) * 3;
        int box_c = (box % 3) * 3;

        for (int d = 1; d <= 9; d++) {
            int row_mask = 0, col_mask = 0, count = 0;

            for (int br = 0; br < 3; br++) {
                for (int bc = 0; bc < 3; bc++) {
                    int r = box_r + br;
                    int c = box_c + bc;
                    if (work_board->cells[r][c].value == 0 && board_has_candidate(work_board, r, c, d)) {
                        row_mask |= (1 << br);
                        col_mask |= (1 << bc);
                        count++;
                    }
                }
            }

            /* Si tous les candidats sont alignés sur une seule ligne du bloc */
            if (count > 1 && __builtin_popcount(row_mask) == 1) {
                int target_r = box_r + __builtin_ctz(row_mask);
                for (int c = 0; c < BOARD_SIZE; c++) {
                    if ((c < box_c || c >= box_c + 3) && board_has_candidate(work_board, target_r, c, d)) {
                        board_remove_candidate(work_board, target_r, c, d);
                        progress = true;
                    }
                }
            }

            /* Si tous les candidats sont alignés sur une seule colonne du bloc */
            if (count > 1 && __builtin_popcount(col_mask) == 1) {
                int target_c = box_c + __builtin_ctz(col_mask);
                for (int r = 0; r < BOARD_SIZE; r++) {
                    if ((r < box_r || r >= box_r + 3) && board_has_candidate(work_board, r, target_c, d)) {
                        board_remove_candidate(work_board, r, target_c, d);
                        progress = true;
                    }
                }
            }
        }
    }
    return progress;
}

/* --- Moteur d'évaluation principal --- */

void solver_eval_analyze(const SudokuBoard *board, SudokuEvaluation *result) {
    if (!board || !result) return;

    memset(result, 0, sizeof(SudokuEvaluation));
    result->max_se_rating = 1.0f;

    /* Copie de travail pour ne pas altérer la grille d'origine */
    SudokuBoard work_board = *board;
    board_recompute_all_candidates(&work_board);

    bool changed = true;
    while (changed) {
        changed = false;

        /* 1. Essayer les règles de niveau 1 (Singles) */
        if (apply_naked_singles(&work_board) || apply_hidden_singles(&work_board)) {
            result->total_steps++;
            if (result->max_se_rating < SE_SINGLE) result->max_se_rating = SE_SINGLE;
            changed = true;
            continue;
        }

        /* 2. Essayer les règles d'intersections (Pointing Pairs) */
        if (apply_pointing_pairs(&work_board)) {
            result->total_steps++;
            result->complex_steps++;
            if (result->max_se_rating < SE_POINTING) result->max_se_rating = SE_POINTING;
            changed = true;
            continue;
        }
    }

    /* Vérifier si la grille est totalement résolue */
    bool filled = true;
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (work_board.cells[r][c].value == 0) {
                filled = false;
                break;
            }
        }
    }

    result->is_solvable = filled;
}