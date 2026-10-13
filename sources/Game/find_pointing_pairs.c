#include <stdio.h>
#include <stdbool.h>
#include "find_pointing_pairs.h"
#include "candidates.h"

bool find_pointing_pairs(const SudokuGame *game) {
    SudokuGame temp = *game;
    compute_autonote(&temp);

    SudokuGame *mutable_game = (SudokuGame *)game;

    for (int b = 0; b < 9; b++) {
        int start_r = (b / 3) * 3;
        int start_c = (b % 3) * 3;

        for (int val = 1; val <= 9; val++) {
            int same_r = -1;
            int same_c = -1;
            bool is_same_row = true;
            bool is_same_col = true;
            int count_in_box = 0;

            for (int r = 0; r < 3; r++) {
                for (int c = 0; c < 3; c++) {
                    int curr_r = start_r + r;
                    int curr_c = start_c + c;

                    if (temp.player_grid[curr_r][curr_c] == 0 && has_candidate(&temp, curr_r, curr_c, val)) {
                        count_in_box++;

                        if (same_r == -1) same_r = curr_r;
                        else if (same_r != curr_r) is_same_row = false;

                        if (same_c == -1) same_c = curr_c;
                        else if (same_c != curr_c) is_same_col = false;
                    }
                }
            }

            if (count_in_box >= 2 && count_in_box <= 3) {
                // Cas 1 : Alignés sur la même ligne
                if (is_same_row && same_r != -1) {
                    bool can_eliminate = false;
                    for (int col = 0; col < 9; col++) {
                        int box_of_col = (same_r / 3) * 3 + (col / 3);
                        if (box_of_col != b && temp.player_grid[same_r][col] == 0 && has_candidate(&temp, same_r, col, val)) {
                            can_eliminate = true;
                            break;
                        }
                    }

                    if (can_eliminate) {
                        clear_highlights(mutable_game);

                        // 1. Marquer en cyan les candidats alignés dans le bloc b
                        for (int r = 0; r < 3; r++) {
                            for (int c = 0; c < 3; c++) {
                                int curr_r = start_r + r;
                                int curr_c = start_c + c;
                                if (temp.player_grid[curr_r][curr_c] == 0 && has_candidate(&temp, curr_r, curr_c, val)) {
                                    mutable_game->highlight_green[curr_r][curr_c] |= (1 << val);
                                }
                            }
                        }

                        // 2. Marquer en rouge uniquement les candidats 'val' à éliminer sur la ligne hors du bloc b
                        for (int col = 0; col < 9; col++) {
                            int box_of_col = (same_r / 3) * 3 + (col / 3);
                            if (box_of_col != b && temp.player_grid[same_r][col] == 0 && has_candidate(&temp, same_r, col, val)) {
                                mutable_game->highlight_red[same_r][col] |= (1 << val);
                            }
                        }

                        printf("📖 THÉORIE — Paires/Triplets Pointés (Pointing Pairs/Triples) :\n");
                        printf("   Lorsque toutes les occurrences d'un candidat dans un bloc 3x3\n");
                        printf("   sont alignées sur une même ligne, ce chiffre doit nécessairement y être placé.\n");
                        printf("   Il peut donc être éliminé des autres cases de cette ligne.\n\n");

                        printf("🎯 APPLICATION :\n");
                        printf("   Dans le bloc %d, le chiffre %d n'apparaît que sur la ligne %d.\n", b + 1, val, same_r + 1);
                        printf("   👉 Action : Vous pouvez éliminer le candidat %d des autres cases de la ligne %d.\n", val, same_r + 1);
                        return true;
                    }
                }

                // Cas 2 : Alignés sur la même colonne
                if (is_same_col && same_c != -1) {
                    bool can_eliminate = false;
                    for (int row = 0; row < 9; row++) {
                        int box_of_row = (row / 3) * 3 + (same_c / 3);
                        if (box_of_row != b && temp.player_grid[row][same_c] == 0 && has_candidate(&temp, row, same_c, val)) {
                            can_eliminate = true;
                            break;
                        }
                    }

                    if (can_eliminate) {
                        clear_highlights(mutable_game);

                        // 1. Marquer en cyan les candidats alignés dans le bloc b
                        for (int r = 0; r < 3; r++) {
                            for (int c = 0; c < 3; c++) {
                                int curr_r = start_r + r;
                                int curr_c = start_c + c;
                                if (temp.player_grid[curr_r][curr_c] == 0 && has_candidate(&temp, curr_r, curr_c, val)) {
                                    mutable_game->highlight_green[curr_r][curr_c] |= (1 << val);
                                }
                            }
                        }

                        // 2. Marquer en rouge uniquement les candidats 'val' à éliminer sur la colonne hors du bloc b
                        for (int row = 0; row < 9; row++) {
                            int box_of_row = (row / 3) * 3 + (same_c / 3);
                            if (box_of_row != b && temp.player_grid[row][same_c] == 0 && has_candidate(&temp, row, same_c, val)) {
                                mutable_game->highlight_red[row][same_c] |= (1 << val);
                            }
                        }

                        printf("📖 THÉORIE — Paires/Triplets Pointés (Pointing Pairs/Triples) :\n");
                        printf("   Lorsque toutes les occurrences d'un candidat dans un bloc 3x3\n");
                        printf("   sont alignées sur une même colonne, ce chiffre doit nécessairement y être placé.\n");
                        printf("   Il peut donc être éliminé des autres cases de cette colonne.\n\n");

                        printf("🎯 APPLICATION :\n");
                        printf("   Dans le bloc %d, le chiffre %d n'apparaît que sur la colonne %d.\n", b + 1, val, same_c + 1);
                        printf("   👉 Action : Vous pouvez éliminer le candidat %d des autres cases de la colonne %d.\n", val, same_c + 1);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}