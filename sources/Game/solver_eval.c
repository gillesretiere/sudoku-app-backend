#include <stdio.h>
#include <stdbool.h>
#include "solver_eval.h"
#include "candidates.h"

static bool remove_candidate_bit(SudokuGame *game, int r, int c, int val) {
    if (game->player_grid[r][c] == 0 && (game->candidates[r][c] & (1 << val))) {
        game->candidates[r][c] &= ~(1 << val);
        return true;
    }
    return false;
}

static int count_bits(uint16_t mask) {
    int count = 0;
    for (int v = 1; v <= 9; v++) {
        if (mask & (1 << v)) count++;
    }
    return count;
}

static void place_digit(SudokuGame *game, int r, int c, int val) {
    game->player_grid[r][c] = (char)val;
    game->candidates[r][c] = 0;

    for (int i = 0; i < 9; i++) {
        game->candidates[r][i] &= ~(1 << val);
        game->candidates[i][c] &= ~(1 << val);
    }

    int start_r = (r / 3) * 3;
    int start_c = (c / 3) * 3;
    for (int br = 0; br < 3; br++) {
        for (int bc = 0; bc < 3; bc++) {
            game->candidates[start_r + br][start_c + bc] &= ~(1 << val);
        }
    }
}

/* --- Niveau 0 : Singles --- */
static bool silent_singles(SudokuGame *game) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (game->player_grid[r][c] == 0 && count_candidates(game, r, c) == 1) {
                for (int v = 1; v <= 9; v++) {
                    if (has_candidate(game, r, c, v)) {
                        place_digit(game, r, c, v);
                        return true;
                    }
                }
            }
        }
    }

    for (int unit_type = 0; unit_type < 3; unit_type++) {
        for (int u = 0; u < 9; u++) {
            for (int val = 1; val <= 9; val++) {
                int count = 0, target_r = -1, target_c = -1;
                for (int i = 0; i < 9; i++) {
                    int r = (unit_type == 0) ? u : (unit_type == 1) ? i : (u / 3) * 3 + i / 3;
                    int c = (unit_type == 0) ? i : (unit_type == 1) ? u : (u % 3) * 3 + i % 3;

                    if (game->player_grid[r][c] == 0 && has_candidate(game, r, c, val)) {
                        count++;
                        target_r = r;
                        target_c = c;
                    }
                }
                if (count == 1) {
                    place_digit(game, target_r, target_c, val);
                    return true;
                }
            }
        }
    }
    return false;
}

/* --- Niveau 1 : Paires Nues --- */
static bool silent_naked_pairs(SudokuGame *game) {
    bool changed = false;
    for (int unit_type = 0; unit_type < 3; unit_type++) {
        for (int u = 0; u < 9; u++) {
            for (int i = 0; i < 8; i++) {
                int r1 = (unit_type == 0) ? u : (unit_type == 1) ? i : (u / 3) * 3 + i / 3;
                int c1 = (unit_type == 0) ? i : (unit_type == 1) ? u : (u % 3) * 3 + i % 3;

                if (game->player_grid[r1][c1] != 0 || count_candidates(game, r1, c1) != 2) continue;

                for (int j = i + 1; j < 9; j++) {
                    int r2 = (unit_type == 0) ? u : (unit_type == 1) ? j : (u / 3) * 3 + j / 3;
                    int c2 = (unit_type == 0) ? j : (unit_type == 1) ? u : (u % 3) * 3 + j % 3;

                    if (game->player_grid[r2][c2] != 0 || count_candidates(game, r2, c2) != 2) continue;

                    if (game->candidates[r1][c1] == game->candidates[r2][c2]) {
                        uint16_t mask = game->candidates[r1][c1];
                        for (int k = 0; k < 9; k++) {
                            if (k == i || k == j) continue;
                            int r = (unit_type == 0) ? u : (unit_type == 1) ? k : (u / 3) * 3 + k / 3;
                            int c = (unit_type == 0) ? k : (unit_type == 1) ? u : (u % 3) * 3 + k % 3;

                            for (int v = 1; v <= 9; v++) {
                                if (mask & (1 << v)) {
                                    if (remove_candidate_bit(game, r, c, v)) changed = true;
                                }
                            }
                        }
                        if (changed) return true;
                    }
                }
            }
        }
    }
    return false;
}

/* --- Niveau 1 : Paires Cachées --- */
static bool silent_hidden_pairs(SudokuGame *game) {
    bool changed = false;
    for (int unit_type = 0; unit_type < 3; unit_type++) {
        for (int u = 0; u < 9; u++) {
            for (int v1 = 1; v1 < 9; v1++) {
                for (int v2 = v1 + 1; v2 <= 9; v2++) {
                    int count = 0;
                    int pos1 = -1, pos2 = -1;
                    bool valid = true;

                    for (int i = 0; i < 9; i++) {
                        int r = (unit_type == 0) ? u : (unit_type == 1) ? i : (u / 3) * 3 + i / 3;
                        int c = (unit_type == 0) ? i : (unit_type == 1) ? u : (u % 3) * 3 + i % 3;

                        if (game->player_grid[r][c] == 0) {
                            bool h1 = has_candidate(game, r, c, v1);
                            bool h2 = has_candidate(game, r, c, v2);

                            if (h1 || h2) {
                                if (h1 && h2) {
                                    count++;
                                    if (pos1 == -1) pos1 = i;
                                    else if (pos2 == -1) pos2 = i;
                                } else {
                                    valid = false;
                                    break;
                                }
                            }
                        }
                    }

                    if (valid && count == 2) {
                        int r1 = (unit_type == 0) ? u : (unit_type == 1) ? pos1 : (u / 3) * 3 + pos1 / 3;
                        int c1 = (unit_type == 0) ? pos1 : (unit_type == 1) ? u : (u % 3) * 3 + pos1 % 3;
                        int r2 = (unit_type == 0) ? u : (unit_type == 1) ? pos2 : (u / 3) * 3 + pos2 / 3;
                        int c2 = (unit_type == 0) ? pos2 : (unit_type == 1) ? u : (u % 3) * 3 + pos2 % 3;

                        for (int v = 1; v <= 9; v++) {
                            if (v != v1 && v != v2) {
                                if (remove_candidate_bit(game, r1, c1, v)) changed = true;
                                if (remove_candidate_bit(game, r2, c2, v)) changed = true;
                            }
                        }
                        if (changed) return true;
                    }
                }
            }
        }
    }
    return false;
}

/* --- Niveau 2 : Pointing Pairs --- */
static bool silent_pointing_pairs(SudokuGame *game) {
    bool changed = false;
    for (int b = 0; b < 9; b++) {
        int start_r = (b / 3) * 3;
        int start_c = (b % 3) * 3;

        for (int val = 1; val <= 9; val++) {
            int same_r = -1, same_c = -1;
            bool is_same_row = true, is_same_col = true;
            int count = 0;

            for (int r = 0; r < 3; r++) {
                for (int c = 0; c < 3; c++) {
                    int curr_r = start_r + r, curr_c = start_c + c;
                    if (game->player_grid[curr_r][curr_c] == 0 && has_candidate(game, curr_r, curr_c, val)) {
                        count++;
                        if (same_r == -1) same_r = curr_r;
                        else if (same_r != curr_r) is_same_row = false;

                        if (same_c == -1) same_c = curr_c;
                        else if (same_c != curr_c) is_same_col = false;
                    }
                }
            }

            if (count >= 2 && count <= 3) {
                if (is_same_row && same_r != -1) {
                    for (int col = 0; col < 9; col++) {
                        if ((col / 3) != (b % 3)) {
                            if (remove_candidate_bit(game, same_r, col, val)) changed = true;
                        }
                    }
                }
                if (is_same_col && same_c != -1) {
                    for (int row = 0; row < 9; row++) {
                        if ((row / 3) != (b / 3)) {
                            if (remove_candidate_bit(game, row, same_c, val)) changed = true;
                        }
                    }
                }
                if (changed) return true;
            }
        }
    }
    return false;
}

/* --- Niveau 2 : Box-Line Reduction --- */
static bool silent_box_line_reduction(SudokuGame *game) {
    bool changed = false;

    for (int r = 0; r < 9; r++) {
        for (int val = 1; val <= 9; val++) {
            int same_box = -1;
            bool is_same_box = true;
            int count = 0;

            for (int c = 0; c < 9; c++) {
                if (game->player_grid[r][c] == 0 && has_candidate(game, r, c, val)) {
                    count++;
                    int b = (r / 3) * 3 + (c / 3);
                    if (same_box == -1) same_box = b;
                    else if (same_box != b) is_same_box = false;
                }
            }

            if (count >= 2 && count <= 3 && is_same_box && same_box != -1) {
                int start_r = (same_box / 3) * 3;
                int start_c = (same_box % 3) * 3;

                for (int br = 0; br < 3; br++) {
                    for (int bc = 0; bc < 3; bc++) {
                        int curr_r = start_r + br;
                        int curr_c = start_c + bc;
                        if (curr_r != r) {
                            if (remove_candidate_bit(game, curr_r, curr_c, val)) changed = true;
                        }
                    }
                }
                if (changed) return true;
            }
        }
    }

    for (int c = 0; c < 9; c++) {
        for (int val = 1; val <= 9; val++) {
            int same_box = -1;
            bool is_same_box = true;
            int count = 0;

            for (int r = 0; r < 9; r++) {
                if (game->player_grid[r][c] == 0 && has_candidate(game, r, c, val)) {
                    count++;
                    int b = (r / 3) * 3 + (c / 3);
                    if (same_box == -1) same_box = b;
                    else if (same_box != b) is_same_box = false;
                }
            }

            if (count >= 2 && count <= 3 && is_same_box && same_box != -1) {
                int start_r = (same_box / 3) * 3;
                int start_c = (same_box % 3) * 3;

                for (int br = 0; br < 3; br++) {
                    for (int bc = 0; bc < 3; bc++) {
                        int curr_r = start_r + br;
                        int curr_c = start_c + bc;
                        if (curr_c != c) {
                            if (remove_candidate_bit(game, curr_r, curr_c, val)) changed = true;
                        }
                    }
                }
                if (changed) return true;
            }
        }
    }
    return false;
}

/* --- Niveau 2 : Triplets Nus --- */
static bool silent_naked_triples(SudokuGame *game) {
    bool changed = false;

    for (int unit_type = 0; unit_type < 3; unit_type++) {
        for (int u = 0; u < 9; u++) {
            int cells[9][2];
            for (int i = 0; i < 9; i++) {
                if (unit_type == 0)      { cells[i][0] = u; cells[i][1] = i; }
                else if (unit_type == 1) { cells[i][0] = i; cells[i][1] = u; }
                else                     { cells[i][0] = (u / 3) * 3 + i / 3; cells[i][1] = (u % 3) * 3 + i % 3; }
            }

            for (int i = 0; i < 7; i++) {
                int r1 = cells[i][0], c1 = cells[i][1];
                if (game->player_grid[r1][c1] != 0) continue;
                uint16_t m1 = game->candidates[r1][c1];
                if (count_bits(m1) > 3 || count_bits(m1) < 2) continue;

                for (int j = i + 1; j < 8; j++) {
                    int r2 = cells[j][0], c2 = cells[j][1];
                    if (game->player_grid[r2][c2] != 0) continue;
                    uint16_t m2 = game->candidates[r2][c2];
                    if (count_bits(m2) > 3 || count_bits(m2) < 2) continue;

                    for (int k = j + 1; k < 9; k++) {
                        int r3 = cells[k][0], c3 = cells[k][1];
                        if (game->player_grid[r3][c3] != 0) continue;
                        uint16_t m3 = game->candidates[r3][c3];
                        if (count_bits(m3) > 3 || count_bits(m3) < 2) continue;

                        uint16_t combined = m1 | m2 | m3;

                        if (count_bits(combined) == 3) {
                            for (int idx = 0; idx < 9; idx++) {
                                if (idx == i || idx == j || idx == k) continue;
                                int r = cells[idx][0], c = cells[idx][1];
                                for (int v = 1; v <= 9; v++) {
                                    if (combined & (1 << v)) {
                                        if (remove_candidate_bit(game, r, c, v)) changed = true;
                                    }
                                }
                            }
                            if (changed) return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

/* --- Niveau 3 : X-Wing --- */
static bool silent_x_wing(SudokuGame *game) {
    bool changed = false;

    for (int val = 1; val <= 9; val++) {
        for (int r1 = 0; r1 < 8; r1++) {
            int cols_r1[9], count_r1 = 0;
            for (int c = 0; c < 9; c++) {
                if (game->player_grid[r1][c] == 0 && has_candidate(game, r1, c, val)) {
                    cols_r1[count_r1++] = c;
                }
            }
            if (count_r1 != 2) continue;

            for (int r2 = r1 + 1; r2 < 9; r2++) {
                int cols_r2[9], count_r2 = 0;
                for (int c = 0; c < 9; c++) {
                    if (game->player_grid[r2][c] == 0 && has_candidate(game, r2, c, val)) {
                        cols_r2[count_r2++] = c;
                    }
                }
                if (count_r2 != 2) continue;

                if (cols_r1[0] == cols_r2[0] && cols_r1[1] == cols_r2[1]) {
                    int c1 = cols_r1[0], c2 = cols_r1[1];
                    for (int r = 0; r < 9; r++) {
                        if (r == r1 || r == r2) continue;
                        if (remove_candidate_bit(game, r, c1, val)) changed = true;
                        if (remove_candidate_bit(game, r, c2, val)) changed = true;
                    }
                    if (changed) return true;
                }
            }
        }
    }

    for (int val = 1; val <= 9; val++) {
        for (int c1 = 0; c1 < 8; c1++) {
            int rows_c1[9], count_c1 = 0;
            for (int r = 0; r < 9; r++) {
                if (game->player_grid[r][c1] == 0 && has_candidate(game, r, c1, val)) {
                    rows_c1[count_c1++] = r;
                }
            }
            if (count_c1 != 2) continue;

            for (int c2 = c1 + 1; c2 < 9; c2++) {
                int rows_c2[9], count_c2 = 0;
                for (int r = 0; r < 9; r++) {
                    if (game->player_grid[r][c2] == 0 && has_candidate(game, r, c2, val)) {
                        rows_c2[count_c2++] = r;
                    }
                }
                if (count_c2 != 2) continue;

                if (rows_c1[0] == rows_c2[0] && rows_c1[1] == rows_c2[1]) {
                    int r1 = rows_c1[0], r2 = rows_c1[1];
                    for (int c = 0; c < 9; c++) {
                        if (c == c1 || c == c2) continue;
                        if (remove_candidate_bit(game, r1, c, val)) changed = true;
                        if (remove_candidate_bit(game, r2, c, val)) changed = true;
                    }
                    if (changed) return true;
                }
            }
        }
    }

    return false;
}

/* --- Niveau 3 : Y-Wing (Silencieux) --- */
static bool silent_y_wing(SudokuGame *game) {
    bool changed = false;

    for (int pr = 0; pr < 9; pr++) {
        for (int pc = 0; pc < 9; pc++) {
            if (game->player_grid[pr][pc] != 0 || count_candidates(game, pr, pc) != 2) continue;

            int px = -1, py = -1;
            for (int v = 1; v <= 9; v++) {
                if (has_candidate(game, pr, pc, v)) {
                    if (px == -1) px = v;
                    else py = v;
                }
            }

            for (int r1 = 0; r1 < 9; r1++) {
                for (int c1 = 0; c1 < 9; c1++) {
                    if ((r1 == pr && c1 == pc) || !(r1 == pr || c1 == pc || (r1 / 3 == pr / 3 && c1 / 3 == pc / 3))) continue;
                    if (game->player_grid[r1][c1] != 0 || count_candidates(game, r1, c1) != 2) continue;

                    int p1_v1 = -1, p1_v2 = -1;
                    for (int v = 1; v <= 9; v++) {
                        if (has_candidate(game, r1, c1, v)) {
                            if (p1_v1 == -1) p1_v1 = v;
                            else p1_v2 = v;
                        }
                    }

                    int z = -1;
                    if ((p1_v1 == px && p1_v2 != py) || (p1_v2 == px && p1_v1 != py)) z = (p1_v1 == px) ? p1_v2 : p1_v1;
                    else if ((p1_v1 == py && p1_v2 != px) || (p1_v2 == py && p1_v1 != px)) z = (p1_v1 == py) ? p1_v2 : p1_v1;
                    if (z == -1) continue;

                    for (int r2 = 0; r2 < 9; r2++) {
                        for (int c2 = 0; c2 < 9; c2++) {
                            if ((r2 == r1 && c2 == c1) || (r2 == pr && c2 == pc)) continue;
                            if (!(r2 == pr || c2 == pc || (r2 / 3 == pr / 3 && c2 / 3 == pc / 3))) continue;
                            if (game->player_grid[r2][c2] != 0 || count_candidates(game, r2, c2) != 2) continue;

                            int p2_v1 = -1, p2_v2 = -1;
                            for (int v = 1; v <= 9; v++) {
                                if (has_candidate(game, r2, c2, v)) {
                                    if (p2_v1 == -1) p2_v1 = v;
                                    else p2_v2 = v;
                                }
                            }

                            int other_p = (p1_v1 == px || p1_v2 == px) ? py : px;
                            if (!((p2_v1 == z && p2_v2 == other_p) || (p2_v1 == other_p && p2_v2 == z))) continue;

                            for (int er = 0; er < 9; er++) {
                                for (int ec = 0; ec < 9; ec++) {
                                    if (game->player_grid[er][ec] == 0) {
                                        bool see1 = (er == r1 || ec == c1 || (er / 3 == r1 / 3 && ec / 3 == c1 / 3));
                                        bool see2 = (er == r2 || ec == c2 || (er / 3 == r2 / 3 && ec / 3 == c2 / 3));
                                        if (see1 && see2 && !(er == r1 && ec == c1) && !(er == r2 && ec == c2)) {
                                            if (remove_candidate_bit(game, er, ec, z)) changed = true;
                                        }
                                    }
                                }
                            }
                            if (changed) return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

EvalResult evaluate_puzzle_difficulty(const SudokuGame *game) {
    SudokuGame temp = *game;
    EvalResult res = { .level = 0, .x_wing_count = 0 };
    bool progress = true;

    compute_autonote(&temp);

    while (!check_victory(&temp) && progress) {
        progress = false;

        /* 1. Singles */
        if (silent_singles(&temp)) {
            progress = true;
            continue;
        }

        /* 2. Paires (Nues et Cachées) */
        if (silent_naked_pairs(&temp) || silent_hidden_pairs(&temp)) {
            if (res.level < 1) res.level = 1;
            progress = true;
            continue;
        }

        /* 3. Intersections & Triplets */
        if (silent_pointing_pairs(&temp) || silent_box_line_reduction(&temp) || silent_naked_triples(&temp)) {
            if (res.level < 2) res.level = 2;
            progress = true;
            continue;
        }

        /* 4. Niveau 3 : Évaluation indépendante de X-Wing et Y-Wing (sans court-circuit) */
        bool found_x = silent_x_wing(&temp);
        bool found_y = silent_y_wing(&temp);

        if (found_x || found_y) {
            if (res.level < 3) res.level = 3;
            if (found_x) res.x_wing_count++;
            if (found_y) res.y_wing_count++;
            progress = true;
            continue;
        }
    }

    if (check_victory(&temp)) {
        return res;
    }

    res.level = -1;
    return res;
}