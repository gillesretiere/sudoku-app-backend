#include <stdio.h>
#include <stdbool.h>
#include "solver_eval.h"
#include "candidates.h"

/* Supprime un bit de candidat pour une case donnée */
static bool remove_candidate_bit(SudokuGame *game, int r, int c, int val) {
    if (game->player_grid[r][c] == 0 && (game->candidates[r][c] & (1 << val))) {
        game->candidates[r][c] &= ~(1 << val);
        return true;
    }
    return false;
}

/* --- Niveau 0 : Singles (placement direct) --- */
static bool silent_singles(SudokuGame *game) {
    /* 1. Naked Single */
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (game->player_grid[r][c] == 0 && count_candidates(game, r, c) == 1) {
                for (int v = 1; v <= 9; v++) {
                    if (has_candidate(game, r, c, v)) {
                        game->player_grid[r][c] = (char)v;
                        compute_autonote(game);
                        return true;
                    }
                }
            }
        }
    }

    /* 2. Hidden Single */
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
                    game->player_grid[target_r][target_c] = (char)val;
                    compute_autonote(game);
                    return true;
                }
            }
        }
    }
    return false;
}

/* --- Niveau 1 : Paires Nues (Élimination réelle) --- */
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

                    /* Détection de deux cases partageant les mêmes 2 candidats */
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

/* --- Niveau 3 : X-Wing (Élimination réelle) --- */
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
    return false;
}

int evaluate_puzzle_difficulty(const SudokuGame *game) {
    SudokuGame temp = *game;
    int max_level_used = 0;
    bool progress = true;

    compute_autonote(&temp);

    while (!check_victory(&temp) && progress) {
        progress = false;

        /* 1. Essai Niveau 0 (Singles) */
        if (silent_singles(&temp)) {
            progress = true;
            continue;
        }

        /* 2. Essai Niveau 1 (Paires) */
        if (silent_naked_pairs(&temp)) {
            if (max_level_used < 1) max_level_used = 1;
            progress = true;
            continue;
        }

        /* 3. Essai Niveau 3 (X-Wing) */
        if (silent_x_wing(&temp)) {
            if (max_level_used < 3) max_level_used = 3;
            progress = true;
            continue;
        }
    }

    if (check_victory(&temp)) {
        return max_level_used;
    }

    return -1; /* Grille bloquée ou nécessitant une stratégie non implémentée */
}