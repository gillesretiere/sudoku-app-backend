#include <stdio.h>
#include <stdbool.h>
#include "find_x_wing.h"
#include "candidates.h"

bool find_x_wing(const SudokuGame *game) {
    SudokuGame temp = *game;
    compute_autonote(&temp);

    // 1. X-Wing basé sur les LIGNES (Éliminations sur les COLONNES)
    for (int val = 1; val <= 9; val++) {
        for (int r1 = 0; r1 < 8; r1++) {
            int cols_r1[9];
            int count_r1 = 0;

            for (int c = 0; c < 9; c++) {
                if (temp.player_grid[r1][c] == 0 && has_candidate(&temp, r1, c, val)) {
                    cols_r1[count_r1++] = c;
                }
            }

            if (count_r1 != 2) continue; // Le candidat doit apparaître exactement 2 fois sur la ligne r1

            for (int r2 = r1 + 1; r2 < 9; r2++) {
                int cols_r2[9];
                int count_r2 = 0;

                for (int c = 0; c < 9; c++) {
                    if (temp.player_grid[r2][c] == 0 && has_candidate(&temp, r2, c, val)) {
                        cols_r2[count_r2++] = c;
                    }
                }

                if (count_r2 != 2) continue; // Doit aussi apparaître exactement 2 fois sur r2

                // Alignement parfait sur les deux mêmes colonnes
                if (cols_r1[0] == cols_r2[0] && cols_r1[1] == cols_r2[1]) {
                    int c1 = cols_r1[0];
                    int c2 = cols_r1[1];

                    // Vérification : existe-t-il au moins un candidat à éliminer dans c1 ou c2 ?
                    bool can_eliminate = false;
                    for (int r = 0; r < 9; r++) {
                        if (r == r1 || r == r2) continue;
                        if (temp.player_grid[r][c1] == 0 && has_candidate(&temp, r, c1, val)) can_eliminate = true;
                        if (temp.player_grid[r][c2] == 0 && has_candidate(&temp, r, c2, val)) can_eliminate = true;
                    }

                    if (can_eliminate) {
                        printf("📖 THÉORIE — X-Wing (par Lignes) :\n");
                        printf("   Le candidat %d n'apparaît que dans deux cases de la ligne %d (col. %d et %d)\n", val, r1 + 1, c1 + 1, c2 + 1);
                        printf("   et dans deux cases de la ligne %d, exactement alignées sur les MÊMES colonnes.\n", r2 + 1);
                        printf("   Ces quatre cases forment un rectangle d'interverrouillage.\n\n");

                        printf("🎯 APPLICATION :\n");
                        printf("   👉 Action : Vous pouvez éliminer le candidat %d des autres cases des colonnes %d et %d.\n", val, c1 + 1, c2 + 1);
                        return true;
                    }
                }
            }
        }
    }

    // 2. X-Wing basé sur les COLONNES (Éliminations sur les LIGNES)
    for (int val = 1; val <= 9; val++) {
        for (int c1 = 0; c1 < 8; c1++) {
            int rows_c1[9];
            int count_c1 = 0;

            for (int r = 0; r < 9; r++) {
                if (temp.player_grid[r][c1] == 0 && has_candidate(&temp, r, c1, val)) {
                    rows_c1[count_c1++] = r;
                }
            }

            if (count_c1 != 2) continue;

            for (int c2 = c1 + 1; c2 < 9; c2++) {
                int rows_c2[9];
                int count_c2 = 0;

                for (int r = 0; r < 9; r++) {
                    if (temp.player_grid[r][c2] == 0 && has_candidate(&temp, r, c2, val)) {
                        rows_c2[count_c2++] = r;
                    }
                }

                if (count_c2 != 2) continue;

                if (rows_c1[0] == rows_c2[0] && rows_c1[1] == rows_c2[1]) {
                    int r1 = rows_c1[0];
                    int r2 = rows_c1[1];

                    bool can_eliminate = false;
                    for (int c = 0; c < 9; c++) {
                        if (c == c1 || c == c2) continue;
                        if (temp.player_grid[r1][c] == 0 && has_candidate(&temp, r1, c, val)) can_eliminate = true;
                        if (temp.player_grid[r2][c] == 0 && has_candidate(&temp, r2, c, val)) can_eliminate = true;
                    }

                    if (can_eliminate) {
                        printf("📖 THÉORIE — X-Wing (par Colonnes) :\n");
                        printf("   Le candidat %d n'apparaît que dans deux cases de la colonne %d (lignes %d et %d)\n", val, c1 + 1, r1 + 1, r2 + 1);
                        printf("   et dans deux cases de la colonne %d, alignées sur les MÊMES lignes.\n", c2 + 1);
                        printf("   Ces quatre cases forment un rectangle d'interverrouillage.\n\n");

                        printf("🎯 APPLICATION :\n");
                        printf("   👉 Action : Vous pouvez éliminer le candidat %d des autres cases des lignes %d et %d.\n", val, r1 + 1, r2 + 1);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}