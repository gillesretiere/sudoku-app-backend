#include <stdio.h>
#include <stdbool.h>
#include "find_swordfish.h"
#include "candidates.h"

static bool remove_candidate_bit(SudokuGame *game, int r, int c, int val) {
    if (game->player_grid[r][c] == 0 && (game->candidates[r][c] & (1 << val))) {
        game->candidates[r][c] &= ~(1 << val);
        game->highlight_red[r][c] |= (1 << val);
        return true;
    }
    return false;
}

bool find_swordfish(SudokuGame *game) {
    bool changed = false;

    // 1. Recherche basée sur les lignes (Swordfish horizontal)
    for (int val = 1; val <= 9; val++) {
        for (int r1 = 0; r1 < 7; r1++) {
            for (int r2 = r1 + 1; r2 < 8; r2++) {
                for (int r3 = r2 + 1; r3 < 9; r3++) {

                    int cols_mask = 0;
                    int rows[3] = {r1, r2, r3};
                    bool valid = true;

                    for (int i = 0; i < 3; i++) {
                        int r = rows[i];
                        int count = 0;
                        for (int c = 0; c < 9; c++) {
                            if (game->player_grid[r][c] == 0 && has_candidate(game, r, c, val)) {
                                cols_mask |= (1 << c);
                                count++;
                            }
                        }
                        if (count < 2 || count > 3) {
                            valid = false;
                            break;
                        }
                    }

                    int total_cols = 0;
                    for (int c = 0; c < 9; c++) {
                        if (cols_mask & (1 << c)) total_cols++;
                    }

                    if (valid && total_cols == 3) {
                        for (int c = 0; c < 9; c++) {
                            if (cols_mask & (1 << c)) {
                                for (int r = 0; r < 9; r++) {
                                    if (r != r1 && r != r2 && r != r3) {
                                        if (remove_candidate_bit(game, r, c, val)) {
                                            changed = true;
                                        }
                                    }
                                }
                            }
                        }

                        if (changed) {
                            snprintf(game->hint_description, sizeof(game->hint_description),
                                     "[Niveau 4 — Swordfish]\nUn motif Swordfish sur le chiffre %d a été détecté sur les lignes %d, %d et %d.",
                                     val, r1 + 1, r2 + 1, r3 + 1);
                            return true;
                        }
                    }
                }
            }
        }
    }

    // 2. Recherche basée sur les colonnes (Swordfish vertical)
    for (int val = 1; val <= 9; val++) {
        for (int c1 = 0; c1 < 7; c1++) {
            for (int c2 = c1 + 1; c2 < 8; c2++) {
                for (int c3 = c2 + 1; c3 < 9; c3++) {

                    int rows_mask = 0;
                    int cols[3] = {c1, c2, c3};
                    bool valid = true;

                    for (int i = 0; i < 3; i++) {
                        int c = cols[i];
                        int count = 0;
                        for (int r = 0; r < 9; r++) {
                            if (game->player_grid[r][c] == 0 && has_candidate(game, r, c, val)) {
                                rows_mask |= (1 << r);
                                count++;
                            }
                        }
                        if (count < 2 || count > 3) {
                            valid = false;
                            break;
                        }
                    }

                    int total_rows = 0;
                    for (int r = 0; r < 9; r++) {
                        if (rows_mask & (1 << r)) total_rows++;
                    }

                    if (valid && total_rows == 3) {
                        for (int r = 0; r < 9; r++) {
                            if (rows_mask & (1 << r)) {
                                for (int c = 0; c < 9; c++) {
                                    if (c != c1 && c != c2 && c != c3) {
                                        if (remove_candidate_bit(game, r, c, val)) {
                                            changed = true;
                                        }
                                    }
                                }
                            }
                        }

                        if (changed) {
                            snprintf(game->hint_description, sizeof(game->hint_description),
                                     "[Niveau 4 — Swordfish]\nUn motif Swordfish sur le chiffre %d a été détecté sur les colonnes %d, %d et %d.",
                                     val, c1 + 1, c2 + 1, c3 + 1);
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}