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

bool find_hidden_triple(SudokuGame *game) {
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
                        int count = 0;
                        int target_indices[9];

                        for (int i = 0; i < 9; i++) {
                            int r = cells[i][0], c = cells[i][1];
                            if (game->player_grid[r][c] == 0) {
                                if (has_candidate(game, r, c, v1) || 
                                    has_candidate(game, r, c, v2) || 
                                    has_candidate(game, r, c, v3)) {
                                    target_indices[count++] = i;
                                }
                            }
                        }

                        if (count == 3) {
                            for (int k = 0; k < 3; k++) {
                                int r = cells[target_indices[k]][0];
                                int c = cells[target_indices[k]][1];

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