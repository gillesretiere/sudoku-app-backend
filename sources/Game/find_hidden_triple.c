#include <stdio.h>
#include <stdbool.h>
#include "find_hidden_triple.h"
#include "candidates.h"

static bool remove_candidate_bit(SudokuGame *game, int r, int c, int val) {
    if (game->player_grid[r][c] == 0 && (game->candidates[r][c] & (1 << val))) {
        game->candidates[r][c] &= ~(1 << val);
        game->highlight_red[r][c] |= (1 << val);
        return true;
    }
    return false;
}

bool find_hidden_triple(const SudokuGame *game) {
    bool changed = false;

    for (int unit_type = 0; unit_type < 3; unit_type++) {
        for (int u = 0; u < 9; u++) {
            int cells[9][2];
            for (int i = 0; i < 9; i++) {
                if (unit_type == 0)      { cells[i][0] = u; cells[i][1] = i; }
                else if (unit_type == 1) { cells[i][0] = i; cells[i][1] = u; }
                else                     { cells[i][0] = (u / 3) * 3 + i / 3; cells[i][1] = (u % 3) * 3 + i % 3; }
            }

            for (int v1 = 1; v1 <= 7; v1++) {
                for (int v2 = v1 + 1; v2 <= 8; v2++) {
                    for (int v3 = v2 + 1; v3 <= 9; v3++) {
                        
                        // 1. Compter les apparitions individuelles de chaque chiffre
                        int count_v1 = 0, count_v2 = 0, count_v3 = 0;
                        for (int i = 0; i < 9; i++) {
                            int r = cells[i][0], c = cells[i][1];
                            if (game->player_grid[r][c] == 0) {
                                if (has_candidate(game, r, c, v1)) count_v1++;
                                if (has_candidate(game, r, c, v2)) count_v2++;
                                if (has_candidate(game, r, c, v3)) count_v3++;
                            }
                        }

                        // Chaque chiffre doit être actif (au moins 1 fois) et présent au maximum 3 fois
                        if (count_v1 == 0 || count_v1 > 3 ||
                            count_v2 == 0 || count_v2 > 3 ||
                            count_v3 == 0 || count_v3 > 3) {
                            continue;
                        }

                        // 2. Trouver l'union des cases contenant v1, v2 ou v3
                        int union_cells[9];
                        int union_count = 0;

                        for (int i = 0; i < 9; i++) {
                            int r = cells[i][0], c = cells[i][1];
                            if (game->player_grid[r][c] == 0) {
                                if (has_candidate(game, r, c, v1) || 
                                    has_candidate(game, r, c, v2) || 
                                    has_candidate(game, r, c, v3)) {
                                    union_cells[union_count++] = i;
                                }
                            }
                        }

                        // 3. Si l'ensemble des 3 chiffres est exactement restreint à 3 cases
                        if (union_count == 3) {
                            for (int k = 0; k < 3; k++) {
                                int r = cells[union_cells[k]][0];
                                int c = cells[union_cells[k]][1];

                                for (int val = 1; val <= 9; val++) {
                                    if (val != v1 && val != v2 && val != v3) {
                                        if (remove_candidate_bit(game, r, c, val)) {
                                            changed = true;
                                        }
                                    }
                                }
                            }

                            if (changed) {
                                snprintf(game->hint_description, sizeof(game->hint_description),
                                         "[Niveau 2 — Triplet Caché]\nLes chiffres %d, %d, %d sont confinés dans 3 cases. Les autres candidats de ces cases sont éliminés.",
                                         v1, v2, v3);
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}