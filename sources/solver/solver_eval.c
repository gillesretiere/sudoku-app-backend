#include "solver_eval.h"
#include <string.h>

/* Scores de difficulté SE (Sudoku Explainer Rating) selon Denis Berthier */
#define SE_SINGLE        1.5f
#define SE_PAIR          2.4f
#define SE_POINTING      2.8f
#define SE_BOX_LINE      3.0f
#define SE_X_WING        3.8f
#define SE_Y_WING        4.2f
#define SE_SWORDFISH     5.2f

/* --- Helper : vérifie si deux cases se voient (ligne, colonne ou bloc) --- */
static inline bool cells_see_each_other(int r1, int c1, int r2, int c2) {
    if (r1 == r2 && c1 == c2) return false;
    return (r1 == r2) || (c1 == c2) || ((r1 / 3 == r2 / 3) && (c1 / 3 == c2 / 3));
}

/* --- Stratégies de base (Niveaux 1 et 2) --- */

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
        /* Lignes */
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

        /* Colonnes */
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

/* --- Stratégies Avancées (Niveau 2 et 3) --- */

static bool apply_naked_pairs(SudokuBoard *work_board) {
    bool progress = false;

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c1 = 0; c1 < BOARD_SIZE; c1++) {
            if (work_board->cells[r][c1].value != 0) continue;
            if (board_count_candidates(work_board, r, c1) == 2) {
                uint16_t mask1 = work_board->cells[r][c1].candidates;

                for (int c2 = c1 + 1; c2 < BOARD_SIZE; c2++) {
                    if (work_board->cells[r][c2].value == 0 && work_board->cells[r][c2].candidates == mask1) {
                        for (int c3 = 0; c3 < BOARD_SIZE; c3++) {
                            if (c3 != c1 && c3 != c2 && work_board->cells[r][c3].value == 0) {
                                for (int d = 1; d <= 9; d++) {
                                    if ((mask1 & (1U << d)) && board_has_candidate(work_board, r, c3, d)) {
                                        board_remove_candidate(work_board, r, c3, d);
                                        progress = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return progress;
}

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

            if (count > 1 && __builtin_popcount(row_mask) == 1) {
                int target_r = box_r + __builtin_ctz(row_mask);
                for (int c = 0; c < BOARD_SIZE; c++) {
                    if ((c < box_c || c >= box_c + 3) && board_has_candidate(work_board, target_r, c, d)) {
                        board_remove_candidate(work_board, target_r, c, d);
                        progress = true;
                    }
                }
            }

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

static bool apply_box_line_reduction(SudokuBoard *work_board) {
    bool progress = false;

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int d = 1; d <= 9; d++) {
            int box_mask = 0, count = 0;
            for (int c = 0; c < BOARD_SIZE; c++) {
                if (work_board->cells[r][c].value == 0 && board_has_candidate(work_board, r, c, d)) {
                    int b = (r / 3) * 3 + (c / 3);
                    box_mask |= (1 << b);
                    count++;
                }
            }

            if (count > 1 && __builtin_popcount(box_mask) == 1) {
                int target_box = __builtin_ctz(box_mask);
                int box_r = (target_box / 3) * 3;
                int box_c = (target_box % 3) * 3;

                for (int br = 0; br < 3; br++) {
                    for (int bc = 0; bc < 3; bc++) {
                        int curr_r = box_r + br;
                        int curr_c = box_c + bc;
                        if (curr_r != r && work_board->cells[curr_r][curr_c].value == 0 &&
                            board_has_candidate(work_board, curr_r, curr_c, d)) {
                            board_remove_candidate(work_board, curr_r, curr_c, d);
                            progress = true;
                        }
                    }
                }
            }
        }
    }
    return progress;
}

/* --- Stratégie Expert (X-Wing SE 3.8) --- */

static bool apply_x_wing(SudokuBoard *work_board) {
    bool progress = false;

    for (int d = 1; d <= 9; d++) {
        for (int r1 = 0; r1 < BOARD_SIZE; r1++) {
            int c1_1 = -1, c1_2 = -1, count1 = 0;
            for (int c = 0; c < BOARD_SIZE; c++) {
                if (work_board->cells[r1][c].value == 0 && board_has_candidate(work_board, r1, c, d)) {
                    count1++;
                    if (c1_1 == -1) c1_1 = c;
                    else if (c1_2 == -1) c1_2 = c;
                }
            }

            if (count1 == 2) {
                for (int r2 = r1 + 1; r2 < BOARD_SIZE; r2++) {
                    int c2_1 = -1, c2_2 = -1, count2 = 0;
                    for (int c = 0; c < BOARD_SIZE; c++) {
                        if (work_board->cells[r2][c].value == 0 && board_has_candidate(work_board, r2, c, d)) {
                            count2++;
                            if (c2_1 == -1) c2_1 = c;
                            else if (c2_2 == -1) c2_2 = c;
                        }
                    }

                    if (count2 == 2 && c1_1 == c2_1 && c1_2 == c2_2) {
                        for (int r = 0; r < BOARD_SIZE; r++) {
                            if (r != r1 && r != r2) {
                                if (work_board->cells[r][c1_1].value == 0 && board_has_candidate(work_board, r, c1_1, d)) {
                                    board_remove_candidate(work_board, r, c1_1, d);
                                    progress = true;
                                }
                                if (work_board->cells[r][c1_2].value == 0 && board_has_candidate(work_board, r, c1_2, d)) {
                                    board_remove_candidate(work_board, r, c1_2, d);
                                    progress = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return progress;
}

/* --- Stratégie Expert (Y-Wing SE 4.2) --- */

static bool apply_y_wing(SudokuBoard *work_board) {
    bool progress = false;

    for (int r1 = 0; r1 < BOARD_SIZE; r1++) {
        for (int c1 = 0; c1 < BOARD_SIZE; c1++) {
            if (work_board->cells[r1][c1].value != 0) continue;
            if (board_count_candidates(work_board, r1, c1) != 2) continue;

            uint16_t mask1 = work_board->cells[r1][c1].candidates;
            int a = __builtin_ctz(mask1);
            int b = __builtin_ctz(mask1 & ~(1U << a));

            /* Pincer 1 */
            for (int r2 = 0; r2 < BOARD_SIZE; r2++) {
                for (int c2 = 0; c2 < BOARD_SIZE; c2++) {
                    if (!cells_see_each_other(r1, c1, r2, c2)) continue;
                    if (work_board->cells[r2][c2].value != 0) continue;
                    if (board_count_candidates(work_board, r2, c2) != 2) continue;

                    uint16_t mask2 = work_board->cells[r2][c2].candidates;
                    if (mask1 == mask2) continue;

                    bool has_a = (mask2 & (1U << a)) != 0;
                    bool has_b = (mask2 & (1U << b)) != 0;
                    if (!has_a && !has_b) continue;

                    int shared = has_a ? a : b;
                    int c_val = __builtin_ctz(mask2 & ~(1U << shared));

                    /* Pincer 2 */
                    for (int r3 = 0; r3 < BOARD_SIZE; r3++) {
                        for (int c3 = 0; c3 < BOARD_SIZE; c3++) {
                            if ((r3 == r2 && c3 == c2) || !cells_see_each_other(r1, c1, r3, c3)) continue;
                            if (work_board->cells[r3][c3].value != 0) continue;
                            if (board_count_candidates(work_board, r3, c3) != 2) continue;

                            uint16_t mask3 = work_board->cells[r3][c3].candidates;
                            int other_val = (shared == a) ? b : a;

                            if ((mask3 & (1U << other_val)) && (mask3 & (1U << c_val))) {
                                /* Y-Wing identifié avec pivot (r1,c1) et pinces (r2,c2) et (r3,c3) */
                                for (int rx = 0; rx < BOARD_SIZE; rx++) {
                                    for (int cx = 0; cx < BOARD_SIZE; cx++) {
                                        if (work_board->cells[rx][cx].value == 0 &&
                                            cells_see_each_other(r2, c2, rx, cx) &&
                                            cells_see_each_other(r3, c3, rx, cx) &&
                                            board_has_candidate(work_board, rx, cx, c_val)) {
                                            
                                            board_remove_candidate(work_board, rx, cx, c_val);
                                            progress = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return progress;
}

/* --- Stratégie Maître (Swordfish SE 5.2) --- */

static bool apply_swordfish(SudokuBoard *work_board) {
    bool progress = false;

    for (int d = 1; d <= 9; d++) {
        int row_masks[BOARD_SIZE] = {0};
        int row_counts[BOARD_SIZE] = {0};

        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                if (work_board->cells[r][c].value == 0 && board_has_candidate(work_board, r, c, d)) {
                    row_masks[r] |= (1 << c);
                    row_counts[r]++;
                }
            }
        }

        for (int r1 = 0; r1 < BOARD_SIZE; r1++) {
            if (row_counts[r1] < 2 || row_counts[r1] > 3) continue;
            for (int r2 = r1 + 1; r2 < BOARD_SIZE; r2++) {
                if (row_counts[r2] < 2 || row_counts[r2] > 3) continue;
                for (int r3 = r2 + 1; r3 < BOARD_SIZE; r3++) {
                    if (row_counts[r3] < 2 || row_counts[r3] > 3) continue;

                    int combined_cols = row_masks[r1] | row_masks[r2] | row_masks[r3];
                    if (__builtin_popcount(combined_cols) == 3) {
                        /* Swordfish identifié sur les lignes r1, r2, r3 */
                        for (int c = 0; c < BOARD_SIZE; c++) {
                            if (combined_cols & (1 << c)) {
                                for (int r = 0; r < BOARD_SIZE; r++) {
                                    if (r != r1 && r != r2 && r != r3) {
                                        if (work_board->cells[r][c].value == 0 && board_has_candidate(work_board, r, c, d)) {
                                            board_remove_candidate(work_board, r, c, d);
                                            progress = true;
                                        }
                                    }
                                }
                            }
                        }
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

    SudokuBoard work_board = *board;
    board_recompute_all_candidates(&work_board);

    bool changed = true;
    while (changed) {
        changed = false;

        /* 1. Singles (SE 1.0 - 1.5) */
        if (apply_naked_singles(&work_board) || apply_hidden_singles(&work_board)) {
            result->total_steps++;
            if (result->max_se_rating < SE_SINGLE) result->max_se_rating = SE_SINGLE;
            changed = true;
            continue;
        }

        /* 2. Naked Pairs (SE 2.4) */
        if (apply_naked_pairs(&work_board)) {
            result->total_steps++;
            result->complex_steps++;
            if (result->max_se_rating < SE_PAIR) result->max_se_rating = SE_PAIR;
            changed = true;
            continue;
        }

        /* 3. Pointing Pairs (SE 2.8) */
        if (apply_pointing_pairs(&work_board)) {
            result->total_steps++;
            result->complex_steps++;
            if (result->max_se_rating < SE_POINTING) result->max_se_rating = SE_POINTING;
            changed = true;
            continue;
        }

        /* 4. Box-Line Reduction (SE 3.0) */
        if (apply_box_line_reduction(&work_board)) {
            result->total_steps++;
            result->complex_steps++;
            if (result->max_se_rating < SE_BOX_LINE) result->max_se_rating = SE_BOX_LINE;
            changed = true;
            continue;
        }

        /* 5. X-Wing (SE 3.8) */
        if (apply_x_wing(&work_board)) {
            result->total_steps++;
            result->complex_steps++;
            if (result->max_se_rating < SE_X_WING) result->max_se_rating = SE_X_WING;
            changed = true;
            continue;
        }

        /* 6. Y-Wing (SE 4.2) */
        if (apply_y_wing(&work_board)) {
            result->total_steps++;
            result->complex_steps++;
            if (result->max_se_rating < SE_Y_WING) result->max_se_rating = SE_Y_WING;
            changed = true;
            continue;
        }

        /* 7. Swordfish (SE 5.2) */
        if (apply_swordfish(&work_board)) {
            result->total_steps++;
            result->complex_steps++;
            if (result->max_se_rating < SE_SWORDFISH) result->max_se_rating = SE_SWORDFISH;
            changed = true;
            continue;
        }
    }

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