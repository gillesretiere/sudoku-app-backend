#include <stdio.h>
#include <stdbool.h>
#include "find_y_wing.h"
#include "candidates.h"

/* Vérifie si deux cases se voient (ligne, colonne ou bloc 3x3) */
static bool can_see(int r1, int c1, int r2, int c2) {
    if (r1 == r2 && c1 == c2) return false;
    return (r1 == r2 || c1 == c2 || (r1 / 3 == r2 / 3 && c1 / 3 == c2 / 3));
}

/* Extrait les 2 candidats d'une case bivalente */
static bool get_bivalent_values(const SudokuGame *game, int r, int c, int *v1, int *v2) {
    if (game->player_grid[r][c] != 0 || count_candidates(game, r, c) != 2) {
        return false;
    }
    int found = 0;
    for (int v = 1; v <= 9; v++) {
        if (has_candidate(game, r, c, v)) {
            if (found == 0) *v1 = v;
            else if (found == 1) *v2 = v;
            found++;
        }
    }
    return (found == 2);
}

bool find_y_wing(const SudokuGame *game) {
    for (int pr = 0; pr < 9; pr++) {
        for (int pc = 0; pc < 9; pc++) {
            int px, py;
            if (!get_bivalent_values(game, pr, pc, &px, &py)) continue;

            /* Pince 1 */
            for (int r1 = 0; r1 < 9; r1++) {
                for (int c1 = 0; c1 < 9; c1++) {
                    if (!can_see(pr, pc, r1, c1)) continue;

                    int p1_v1, p1_v2;
                    if (!get_bivalent_values(game, r1, c1, &p1_v1, &p1_v2)) continue;

                    int z = -1;
                    if ((p1_v1 == px && p1_v2 != py) || (p1_v2 == px && p1_v1 != py)) {
                        z = (p1_v1 == px) ? p1_v2 : p1_v1;
                    } else if ((p1_v1 == py && p1_v2 != px) || (p1_v2 == py && p1_v1 != px)) {
                        z = (p1_v1 == py) ? p1_v2 : p1_v1;
                    }
                    if (z == -1) continue;

                    /* Pince 2 */
                    for (int r2 = 0; r2 < 9; r2++) {
                        for (int c2 = 0; c2 < 9; c2++) {
                            if ((r2 == r1 && c2 == c1) || !can_see(pr, pc, r2, c2)) continue;

                            int p2_v1, p2_v2;
                            if (!get_bivalent_values(game, r2, c2, &p2_v1, &p2_v2)) continue;

                            int other_p_val = (p1_v1 == px || p1_v2 == px) ? py : px;
                            if (!((p2_v1 == z && p2_v2 == other_p_val) || (p2_v1 == other_p_val && p2_v2 == z))) {
                                continue;
                            }

                            /* Vérification des éliminations possibles (en lecture seule) */
                            bool eliminated = false;
                            for (int er = 0; er < 9; er++) {
                                for (int ec = 0; ec < 9; ec++) {
                                    if (game->player_grid[er][ec] == 0 && can_see(r1, c1, er, ec) && can_see(r2, c2, er, ec)) {
                                        if (has_candidate(game, er, ec, z)) {
                                            eliminated = true;
                                        }
                                    }
                                }
                            }

                            if (eliminated) {
                                printf("🎯 APPLICATION :\n");
                                printf("   Case Pivot : (%d, %d) [Candidats : %d, %d]\n", pr + 1, pc + 1, px, py);
                                printf("   Pince 1 : (%d, %d) | Pince 2 : (%d, %d)\n", r1 + 1, c1 + 1, r2 + 1, c2 + 1);
                                printf("   👉 Action : Élimination du candidat %d dans les cases communes aux deux pinces.\n", z);
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