#include <stdio.h>
#include <stdbool.h>
#include "find_unique_rectangle.h"
#include "candidates.h"

static int count_bits(uint16_t mask) {
    int count = 0;
    for (int v = 1; v <= 9; v++) {
        if (mask & (1 << v)) count++;
    }
    return count;
}

bool find_unique_rectangle(SudokuGame *game) {
    for (int r1 = 0; r1 < 8; r1++) {
        for (int r2 = r1 + 1; r2 < 9; r2++) {
            // Un Rectangle Unique doit s'étendre sur deux blocs différents
            if (r1 / 3 == r2 / 3) continue;

            for (int c1 = 0; c1 < 8; c1++) {
                for (int c2 = c1 + 1; c2 < 9; c2++) {
                    if (c1 / 3 == c2 / 3) continue;

                    // Coordonnées des 4 coins
                    int rows[4] = {r1, r1, r2, r2};
                    int cols[4] = {c1, c2, c1, c2};

                    // Vérification que les 4 cases sont non résolues
                    bool all_empty = true;
                    for (int i = 0; i < 4; i++) {
                        if (game->player_grid[rows[i]][cols[i]] != 0) {
                            all_empty = false;
                            break;
                        }
                    }
                    if (!all_empty) continue;

                    // Test de toutes les paires possibles {v1, v2}
                    for (int v1 = 1; v1 < 9; v1++) {
                        for (int v2 = v1 + 1; v2 <= 9; v2++) {
                            uint16_t pair_mask = (1 << v1) | (1 << v2);

                            int bivalue_count = 0;
                            int target_idx = -1;

                            for (int i = 0; i < 4; i++) {
                                uint16_t cand = game->candidates[rows[i]][cols[i]];
                                if (cand == pair_mask) {
                                    bivalue_count++;
                                } else if ((cand & pair_mask) == pair_mask) {
                                    target_idx = i;
                                }
                            }

                            // Type 1 : Exactement 3 cases bivaluées {v1, v2} et 1 case contenant {v1, v2} + d'autres candidats
                            if (bivalue_count == 3 && target_idx != -1) {
                                int tr = rows[target_idx];
                                int tc = cols[target_idx];

                                bool changed = false;
                                if (game->candidates[tr][tc] & (1 << v1)) {
                                    game->candidates[tr][tc] &= ~(1 << v1);
                                    game->highlight_red[tr][tc] |= (1 << v1);
                                    changed = true;
                                }
                                if (game->candidates[tr][tc] & (1 << v2)) {
                                    game->candidates[tr][tc] &= ~(1 << v2);
                                    game->highlight_red[tr][tc] |= (1 << v2);
                                    changed = true;
                                }

                                if (changed) {
                                    snprintf(game->hint_description, sizeof(game->hint_description),
                                             "[Niveau 4 — Rectangle Unique]\nÉlimination de %d et %d en case (%d, %d) pour empêcher un piège à solution multiple.",
                                             v1, v2, tr + 1, tc + 1);
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}