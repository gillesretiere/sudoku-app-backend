#include <stdio.h>
#include <stdbool.h>
#include "find_box_line_reduction.h"
#include "candidates.h"

bool find_box_line_reduction(const SudokuGame *game) {
    SudokuGame temp = *game;
    compute_autonote(&temp);

    // 1. Analyse des lignes
    for (int r = 0; r < 9; r++) {
        for (int val = 1; val <= 9; val++) {
            int same_box = -1;
            bool is_same_box = true;
            int count = 0;

            for (int c = 0; c < 9; c++) {
                if (temp.player_grid[r][c] == 0 && has_candidate(&temp, r, c, val)) {
                    count++;
                    int b = (r / 3) * 3 + (c / 3);
                    if (same_box == -1) same_box = b;
                    else if (same_box != b) is_same_box = false;
                }
            }

            if (count >= 2 && count <= 3 && is_same_box && same_box != -1) {
                bool can_eliminate = false;
                int start_r = (same_box / 3) * 3;
                int start_c = (same_box % 3) * 3;

                for (int br = 0; br < 3; br++) {
                    for (int bc = 0; bc < 3; bc++) {
                        int curr_r = start_r + br;
                        int curr_c = start_c + bc;

                        if (curr_r != r && temp.player_grid[curr_r][curr_c] == 0 && has_candidate(&temp, curr_r, curr_c, val)) {
                            can_eliminate = true;
                            break;
                        }
                    }
                }

                if (can_eliminate) {
                    printf("📖 THÉORIE — Réduction Bloc-Ligne (Box-Line Reduction) :\n");
                    printf("   Lorsqu'un candidat sur une ligne n'apparaît que dans un seul bloc 3x3,\n");
                    printf("   ce chiffre doit obligatoirement appartenir à cette ligne dans ce bloc.\n");
                    printf("   Il peut donc être éliminé des autres cases de ce bloc 3x3.\n\n");

                    printf("🎯 APPLICATION :\n");
                    printf("   Sur la ligne %d, le chiffre %d est entièrement confiné au bloc %d.\n", r + 1, val, same_box + 1);
                    printf("   👉 Action : Vous pouvez éliminer le candidat %d des autres cases du bloc %d.\n", val, same_box + 1);
                    return true;
                }
            }
        }
    }

    // 2. Analyse des colonnes
    for (int c = 0; c < 9; c++) {
        for (int val = 1; val <= 9; val++) {
            int same_box = -1;
            bool is_same_box = true;
            int count = 0;

            for (int r = 0; r < 9; r++) {
                if (temp.player_grid[r][c] == 0 && has_candidate(&temp, r, c, val)) {
                    count++;
                    int b = (r / 3) * 3 + (c / 3);
                    if (same_box == -1) same_box = b;
                    else if (same_box != b) is_same_box = false;
                }
            }

            if (count >= 2 && count <= 3 && is_same_box && same_box != -1) {
                bool can_eliminate = false;
                int start_r = (same_box / 3) * 3;
                int start_c = (same_box % 3) * 3;

                for (int br = 0; br < 3; br++) {
                    for (int bc = 0; bc < 3; bc++) {
                        int curr_r = start_r + br;
                        int curr_c = start_c + bc;

                        if (curr_c != c && temp.player_grid[curr_r][curr_c] == 0 && has_candidate(&temp, curr_r, curr_c, val)) {
                            can_eliminate = true;
                            break;
                        }
                    }
                }

                if (can_eliminate) {
                    printf("📖 THÉORIE — Réduction Bloc-Ligne (Box-Line Reduction) :\n");
                    printf("   Lorsqu'un candidat sur une colonne n'apparaît que dans un seul bloc 3x3,\n");
                    printf("   ce chiffre doit obligatoirement appartenir à cette colonne dans ce bloc.\n");
                    printf("   Il peut donc être éliminé des autres cases de ce bloc 3x3.\n\n");

                    printf("🎯 APPLICATION :\n");
                    printf("   Dans la colonne %d, le chiffre %d est entièrement confiné au bloc %d.\n", c + 1, val, same_box + 1);
                    printf("   👉 Action : Vous pouvez éliminer le candidat %d des autres cases du bloc %d.\n", val, same_box + 1);
                    return true;
                }
            }
        }
    }
    return false;
}