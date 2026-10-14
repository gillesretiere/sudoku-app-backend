#include <stdio.h>
#include <stdbool.h>
#include "find_naked_triple.h"
#include "candidates.h"

static int get_candidates_mask(const SudokuGame *game, int r, int c) {
    return game->candidates[r][c];
}

bool find_naked_triple(const SudokuGame *game) {
    SudokuGame temp = *game;
    compute_autonote(&temp);

    SudokuGame *mutable_game = (SudokuGame *)game;

    for (int unit_type = 0; unit_type < 3; unit_type++) {
        for (int u = 0; u < 9; u++) {
            int cells[9][2];
            for (int i = 0; i < 9; i++) {
                if (unit_type == 0) {        // Ligne
                    cells[i][0] = u; cells[i][1] = i;
                } else if (unit_type == 1) { // Colonne
                    cells[i][0] = i; cells[i][1] = u;
                } else {                     // Bloc 3x3
                    cells[i][0] = (u / 3) * 3 + i / 3;
                    cells[i][1] = (u % 3) * 3 + i % 3;
                }
            }

            for (int i = 0; i < 7; i++) {
                int r1 = cells[i][0], c1 = cells[i][1];
                if (temp.player_grid[r1][c1] != 0) continue;
                int m1 = get_candidates_mask(&temp, r1, c1);
                if (m1 == 0 || __builtin_popcount(m1) > 3) continue;

                for (int j = i + 1; j < 8; j++) {
                    int r2 = cells[j][0], c2 = cells[j][1];
                    if (temp.player_grid[r2][c2] != 0) continue;
                    int m2 = get_candidates_mask(&temp, r2, c2);
                    if (m2 == 0 || __builtin_popcount(m2) > 3) continue;

                    for (int k = j + 1; k < 9; k++) {
                        int r3 = cells[k][0], c3 = cells[k][1];
                        if (temp.player_grid[r3][c3] != 0) continue;
                        int m3 = get_candidates_mask(&temp, r3, c3);
                        if (m3 == 0 || __builtin_popcount(m3) > 3) continue;

                        int combined_mask = m1 | m2 | m3;
                        if (__builtin_popcount(combined_mask) == 3) {
                            bool can_eliminate = false;
                            for (int idx = 0; idx < 9; idx++) {
                                if (idx == i || idx == j || idx == k) continue;
                                int r = cells[idx][0], c = cells[idx][1];
                                if (temp.player_grid[r][c] == 0 && (get_candidates_mask(&temp, r, c) & combined_mask) != 0) {
                                    can_eliminate = true;
                                    break;
                                }
                            }

                            if (can_eliminate) {
                                clear_highlights(mutable_game);

                                // 1. Triplet nu en VERT dans les 3 cases
                                mutable_game->highlight_green[r1][c1] |= m1;
                                mutable_game->highlight_green[r2][c2] |= m2;
                                mutable_game->highlight_green[r3][c3] |= m3;

                                // 2. Éliminations en ROUGE dans les autres cases de l'unité
                                for (int idx = 0; idx < 9; idx++) {
                                    if (idx == i || idx == j || idx == k) continue;
                                    int r = cells[idx][0], c = cells[idx][1];
                                    if (temp.player_grid[r][c] == 0) {
                                        mutable_game->highlight_red[r][c] |= (get_candidates_mask(&temp, r, c) & combined_mask);
                                    }
                                }

                                const char *unit_names[] = {"la ligne", "la colonne", "le bloc 3x3"};

                                printf("📖 THÉORIE — Triplet Nu (Naked Triple) :\n");
                                printf("   Lorsque trois cases d'une même unité ne contiennent, à elles trois,\n");
                                printf("   qu'un ensemble de trois candidats au total, ces trois chiffres doivent y être placés.\n");
                                printf("   Ils peuvent donc être éliminés des autres cases de l'unité.\n\n");

                                printf("🎯 APPLICATION :\n");
                                printf("   Dans %s %d, les cases (%d, %d), (%d, %d) et (%d, %d) forment un triplet nu.\n",
                                       unit_names[unit_type], u + 1, r1 + 1, c1 + 1, r2 + 1, c2 + 1, r3 + 1, c3 + 1);
                                printf("   👉 Action : Élimination des candidats du triplet dans les autres cases de %s.\n", unit_names[unit_type]);
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