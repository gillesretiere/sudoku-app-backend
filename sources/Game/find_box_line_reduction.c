#include <stdio.h>
#include <stdbool.h>
#include "find_box_line_reduction.h"
#include "candidates.h"

bool find_box_line_reduction(const SudokuGame *game) {
    SudokuGame temp = *game;
    compute_autonote(&temp);

    SudokuGame *mutable_game = (SudokuGame *)game;

    for (int val = 1; val <= 9; val++) {
        // 1. Recherche par Lignes
        for (int r = 0; r < 9; r++) {
            int target_b = -1;
            int count_in_row = 0;
            bool same_box = true;

            for (int c = 0; c < 9; c++) {
                if (temp.player_grid[r][c] == 0 && has_candidate(&temp, r, c, val)) {
                    count_in_row++;
                    int b = (r / 3) * 3 + (c / 3);
                    if (target_b == -1) target_b = b;
                    else if (target_b != b) same_box = false;
                }
            }

            if (count_in_row >= 2 && same_box && target_b != -1) {
                bool can_eliminate = false;
                int start_r = (target_b / 3) * 3;
                int start_c = (target_b % 3) * 3;

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
                    clear_highlights(mutable_game);

                    // Candidats sources en VERT sur la ligne
                    for (int c = 0; c < 9; c++) {
                        if (temp.player_grid[r][c] == 0 && has_candidate(&temp, r, c, val)) {
                            mutable_game->highlight_green[r][c] |= (1 << val);
                        }
                    }

                    // Candidats à éliminer en ROUGE dans le reste du bloc
                    for (int br = 0; br < 3; br++) {
                        for (int bc = 0; bc < 3; bc++) {
                            int curr_r = start_r + br;
                            int curr_c = start_c + bc;
                            if (curr_r != r && temp.player_grid[curr_r][curr_c] == 0 && has_candidate(&temp, curr_r, curr_c, val)) {
                                mutable_game->highlight_red[curr_r][curr_c] |= (1 << val);
                            }
                        }
                    }

                    /* Format synthétique pour l'interface graphique (GUI) */
                    snprintf(mutable_game->hint_description, sizeof(mutable_game->hint_description),
                             "THEORIE - Reduction Bloc-Ligne :\n"
                             "Les candidats d'un chiffre sur une ligne sont confines dans un bloc. "
                             "Ce chiffre est elimine du reste du bloc.\n\n"
                             "APPLICATION :\n"
                             "Sur la ligne %d, le %d n'apparait que dans le bloc %d.\n"
                             "-> Eliminer le %d des autres cases du bloc %d.",
                             r + 1, val, target_b + 1, val, target_b + 1);

                    /* Format détaillé pour le terminal (CLI) */
                    printf("📖 THÉORIE — Réduction Bloc-Ligne (Box-Line Reduction) :\n");
                    printf("   Lorsque toutes les occurrences d'un candidat sur une ligne sont confinées dans un seul bloc 3x3,\n");
                    printf("   ce chiffre doit nécessairement être placé dans ce bloc sur cette ligne.\n");
                    printf("   Il peut donc être éliminé des autres cases du bloc.\n\n");

                    printf("🎯 APPLICATION :\n");
                    printf("   Sur la ligne %d, le chiffre %d n'apparaît que dans le bloc %d.\n", r + 1, val, target_b + 1);
                    printf("   👉 Action : Élimination du candidat %d des autres cases du bloc %d.\n", val, target_b + 1);
                    return true;
                }
            }
        }

        // 2. Recherche par Colonnes
        for (int c = 0; c < 9; c++) {
            int target_b = -1;
            int count_in_col = 0;
            bool same_box = true;

            for (int r = 0; r < 9; r++) {
                if (temp.player_grid[r][c] == 0 && has_candidate(&temp, r, c, val)) {
                    count_in_col++;
                    int b = (r / 3) * 3 + (c / 3);
                    if (target_b == -1) target_b = b;
                    else if (target_b != b) same_box = false;
                }
            }

            if (count_in_col >= 2 && same_box && target_b != -1) {
                bool can_eliminate = false;
                int start_r = (target_b / 3) * 3;
                int start_c = (target_b % 3) * 3;

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
                    clear_highlights(mutable_game);

                    // Candidats sources en VERT sur la colonne
                    for (int r = 0; r < 9; r++) {
                        if (temp.player_grid[r][c] == 0 && has_candidate(&temp, r, c, val)) {
                            mutable_game->highlight_green[r][c] |= (1 << val);
                        }
                    }

                    // Candidats à éliminer en ROUGE dans le reste du bloc
                    for (int br = 0; br < 3; br++) {
                        for (int bc = 0; bc < 3; bc++) {
                            int curr_r = start_r + br;
                            int curr_c = start_c + bc;
                            if (curr_c != c && temp.player_grid[curr_r][curr_c] == 0 && has_candidate(&temp, curr_r, curr_c, val)) {
                                mutable_game->highlight_red[curr_r][curr_c] |= (1 << val);
                            }
                        }
                    }

                    /* Format synthétique pour l'interface graphique (GUI) */
                    snprintf(mutable_game->hint_description, sizeof(mutable_game->hint_description),
                             "THEORIE - Reduction Bloc-Ligne :\n"
                             "Les candidats d'un chiffre sur une colonne sont confines dans un bloc. "
                             "Ce chiffre est elimine du reste du bloc.\n\n"
                             "APPLICATION :\n"
                             "Dans la colonne %d, le %d n'apparait que dans le bloc %d.\n"
                             "-> Eliminer le %d des autres cases du bloc %d.",
                             c + 1, val, target_b + 1, val, target_b + 1);

                    /* Format détaillé pour le terminal (CLI) */
                    printf("📖 THÉORIE — Réduction Bloc-Ligne (Box-Line Reduction) :\n");
                    printf("   Lorsque toutes les occurrences d'un candidat sur une colonne sont confinées dans un seul bloc 3x3,\n");
                    printf("   ce chiffre doit nécessairement être placé dans ce bloc sur cette colonne.\n");
                    printf("   Il peut donc être éliminé des autres cases du bloc.\n\n");

                    printf("🎯 APPLICATION :\n");
                    printf("   Dans la colonne %d, le chiffre %d n'apparaît que dans le bloc %d.\n", c + 1, val, target_b + 1);
                    printf("   👉 Action : Élimination du candidat %d des autres cases du bloc %d.\n", val, target_b + 1);
                    return true;
                }
            }
        }
    }
    return false;
}