#include <stdio.h>
#include <stdbool.h>
#include "find_hidden_pair.h"
#include "candidates.h"

bool find_hidden_pair(const SudokuGame *game) {
    SudokuGame temp = *game;
    compute_autonote(&temp);

    for (int unit_type = 0; unit_type < 3; unit_type++) {
        for (int u = 0; u < 9; u++) {
            int cells[9][2];
            for (int i = 0; i < 9; i++) {
                if (unit_type == 0) {        // Ligne
                    cells[i][0] = u; cells[i][1] = i;
                } else if (unit_type == 1) { // Colonne
                    cells[i][0] = i; cells[i][1] = u;
                } else {                     // Bloc
                    cells[i][0] = (u / 3) * 3 + i / 3;
                    cells[i][1] = (u % 3) * 3 + i % 3;
                }
            }

            // Test de chaque paire de chiffres possible (d1, d2) entre 1 et 9
            for (int d1 = 1; d1 <= 8; d1++) {
                for (int d2 = d1 + 1; d2 <= 9; d2++) {
                    int cell1_idx = -1, cell2_idx = -1;
                    int count_d1 = 0, count_d2 = 0;

                    for (int i = 0; i < 9; i++) {
                        int r = cells[i][0], c = cells[i][1];
                        if (temp.player_grid[r][c] != 0) continue;

                        bool candidate_d1 = has_candidate(&temp, r, c, d1);
                        bool candidate_d2 = has_candidate(&temp, r, c, d2);

                        if (candidate_d1) count_d1++;
                        if (candidate_d2) count_d2++;

                        if (candidate_d1 || candidate_d2) {
                            if (cell1_idx == -1) cell1_idx = i;
                            else if (cell2_idx == -1) cell2_idx = i;
                        }
                    }

                    // La paire cachée existe si d1 et d2 apparaissent EXACTEMENT 2 fois et dans les deux MÊMES cases
                    if (count_d1 == 2 && count_d2 == 2 && cell1_idx != -1 && cell2_idx != -1) {
                        int r1 = cells[cell1_idx][0], c1 = cells[cell1_idx][1];
                        int r2 = cells[cell2_idx][0], c2 = cells[cell2_idx][1];

                        // S'assurer que d1 et d2 sont tous les deux présents dans les deux cases
                        if (has_candidate(&temp, r1, c1, d1) && has_candidate(&temp, r1, c1, d2) &&
                            has_candidate(&temp, r2, c2, d1) && has_candidate(&temp, r2, c2, d2)) {

                            // La paire est "cachée" si au moins une de ces deux cases contient d'AUTRES candidats à éliminer
                            if (count_candidates(&temp, r1, c1) > 2 || count_candidates(&temp, r2, c2) > 2) {
                                const char *unit_names[] = {"la ligne", "la colonne", "le bloc 3x3"};

                                /* --- EXPLICATION THÉORIQUE --- */
                                printf("📖 THÉORIE — Paire Cachée (Hidden Pair) :\n");
                                printf("   Lorsque deux chiffres apparaissent comme candidats uniquement dans deux cases\n");
                                printf("   d'une même unité, ces deux chiffres doivent nécessairement aller dans ces deux cases.\n");
                                printf("   Tous les autres candidats présents dans ces deux cases peuvent donc être éliminés.\n\n");

                                /* --- APPLICATION À LA GRILLE --- */
                                printf("🎯 APPLICATION :\n");
                                printf("   Dans %s %d, les chiffres %d et %d n'apparaissent que dans les cases (%d, %d) et (%d, %d).\n", 
                                       unit_names[unit_type], u + 1, d1, d2, r1 + 1, c1 + 1, r2 + 1, c2 + 1);
                                printf("   👉 Action : Vous pouvez éliminer tous les AUTRES candidats de ces deux cases\n");
                                printf("      pour ne conserver que la paire {%d, %d}.\n", d1, d2);
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