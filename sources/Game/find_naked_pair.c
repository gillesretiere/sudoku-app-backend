#include <stdio.h>
#include <stdbool.h>
#include "find_naked_pair.h"
#include "candidates.h"

static bool eliminates_candidates_in_unit(const SudokuGame *game, int r1, int c1, int r2, int c2, int val1, int val2, const int cells[9][2]) {
    for (int i = 0; i < 9; i++) {
        int r = cells[i][0];
        int c = cells[i][1];

        if ((r == r1 && c == c1) || (r == r2 && c == c2) || game->player_grid[r][c] != 0) {
            continue;
        }

        if (has_candidate(game, r, c, val1) || has_candidate(game, r, c, val2)) {
            return true;
        }
    }
    return false;
}

bool find_naked_pair(const SudokuGame *game) {
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

            for (int i = 0; i < 8; i++) {
                int r1 = cells[i][0], c1 = cells[i][1];
                if (temp.player_grid[r1][c1] != 0 || count_candidates(&temp, r1, c1) != 2) continue;

                for (int j = i + 1; j < 9; j++) {
                    int r2 = cells[j][0], c2 = cells[j][1];
                    if (temp.player_grid[r2][c2] != 0 || count_candidates(&temp, r2, c2) != 2) continue;

                    if (temp.candidates[r1][c1] == temp.candidates[r2][c2]) {
                        int val1 = 0, val2 = 0;
                        for (int v = 1; v <= 9; v++) {
                            if (has_candidate(&temp, r1, c1, v)) {
                                if (val1 == 0) val1 = v;
                                else val2 = v;
                            }
                        }

                        if (eliminates_candidates_in_unit(&temp, r1, c1, r2, c2, val1, val2, cells)) {
                            clear_highlights(mutable_game);

                            // 1. Surligner la Paire Nue en VERT
                            mutable_game->highlight_green[r1][c1] |= (1 << val1) | (1 << val2);
                            mutable_game->highlight_green[r2][c2] |= (1 << val1) | (1 << val2);

                            // 2. Surligner les candidats à éliminer en ROUGE dans le reste de l'unité
                            for (int k = 0; k < 9; k++) {
                                int r = cells[k][0], c = cells[k][1];
                                if ((r == r1 && c == c1) || (r == r2 && c == c2) || temp.player_grid[r][c] != 0) {
                                    continue;
                                }
                                if (has_candidate(&temp, r, c, val1)) {
                                    mutable_game->highlight_red[r][c] |= (1 << val1);
                                }
                                if (has_candidate(&temp, r, c, val2)) {
                                    mutable_game->highlight_red[r][c] |= (1 << val2);
                                }
                            }

                            const char *unit_names[] = {"la ligne", "la colonne", "le bloc 3x3"};

                            printf("📖 THÉORIE — Paire Nue (Naked Pair) :\n");
                            printf("   Lorsque deux cases d'une même unité (ligne, colonne ou bloc)\n");
                            printf("   contiennent exactement les deux mêmes candidats (et aucun autre),\n");
                            printf("   ces deux chiffres doivent obligatoirement occuper ces deux cases.\n");
                            printf("   Ils peuvent donc être éliminés de toutes les autres cases de l'unité.\n\n");

                            printf("🎯 APPLICATION :\n");
                            printf("   Dans %s %d, les cases (%d, %d) et (%d, %d) contiennent uniquement\n", 
                                   unit_names[unit_type], u + 1, r1 + 1, c1 + 1, r2 + 1, c2 + 1);
                            printf("   la paire de candidats {%d, %d}.\n", val1, val2);
                            printf("   👉 Action : Vous pouvez éliminer les chiffres %d et %d des autres cases vides de %s.\n", 
                                   val1, val2, unit_names[unit_type]);
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}