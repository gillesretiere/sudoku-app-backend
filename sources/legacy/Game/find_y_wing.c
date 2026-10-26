#include <stdio.h>
#include <stdbool.h>
#include "find_y_wing.h"
#include "candidates.h"

// Définitions des couleurs ANSI pour l'affichage de l'indice
#ifndef COLOR_RESET
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_MAGENTA "\033[1;38;2;190;70;255m"
#endif

/* Vérifie si deux cases partagent une ligne, une colonne ou un bloc 3x3 */
static bool can_see(int r1, int c1, int r2, int c2) {
    if (r1 == r2 && c1 == c2) return false;
    return (r1 == r2 || c1 == c2 || (r1 / 3 == r2 / 3 && c1 / 3 == c2 / 3));
}

/* Extrait les deux candidats d'une case bivalente */
static bool get_bivalent_values(SudokuGame *game, int r, int c, int *v1, int *v2) {
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

bool find_y_wing(SudokuGame *game) {
    compute_autonote(game);

    SudokuGame *mutable_game = (SudokuGame *)game;

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

                    int z = -1, p1_shared = -1;
                    if (p1_v1 == px || p1_v2 == px) {
                        p1_shared = px;
                        z = (p1_v1 == px) ? p1_v2 : p1_v1;
                    } else if (p1_v1 == py || p1_v2 == py) {
                        p1_shared = py;
                        z = (p1_v1 == py) ? p1_v2 : p1_v1;
                    }
                    if (z == -1 || z == px || z == py) continue;

                    /* Pince 2 */
                    for (int r2 = 0; r2 < 9; r2++) {
                        for (int c2 = 0; c2 < 9; c2++) {
                            if ((r2 == r1 && c2 == c1) || !can_see(pr, pc, r2, c2)) continue;

                            int p2_v1, p2_v2;
                            if (!get_bivalent_values(game, r2, c2, &p2_v1, &p2_v2)) continue;

                            int p2_shared = (p1_shared == px) ? py : px;
                            if (!((p2_v1 == z && p2_v2 == p2_shared) || (p2_v1 == p2_shared && p2_v2 == z))) {
                                continue;
                            }

                            /* Éliminations possibles : cases qui voient Pince 1 et Pince 2 */
                            bool can_eliminate = false;
                            for (int er = 0; er < 9; er++) {
                                for (int ec = 0; ec < 9; ec++) {
                                    if ((er == pr && ec == pc) || (er == r1 && ec == c1) || (er == r2 && ec == c2)) {
                                        continue;
                                    }
                                    if (game->player_grid[er][ec] == 0 && can_see(r1, c1, er, ec) && can_see(r2, c2, er, ec)) {
                                        if (has_candidate(game, er, ec, z)) {
                                            can_eliminate = true;
                                        }
                                    }
                                }
                            }

                            if (can_eliminate) {
                                clear_highlights(mutable_game);

                                /* 1. Coloration du Pivot : px en Cyan, py en Magenta */
                                mutable_game->highlight_cyan[pr][pc] |= (1 << px);
                                mutable_game->highlight_magenta[pr][pc] |= (1 << py);

                                /* 2. Coloration de la Pince 1 */
                                if (p1_shared == px) {
                                    mutable_game->highlight_cyan[r1][c1] |= (1 << px);
                                } else {
                                    mutable_game->highlight_magenta[r1][c1] |= (1 << py);
                                }
                                mutable_game->highlight_green[r1][c1] |= (1 << z); /* Candidat z en Vert */

                                /* 3. Coloration de la Pince 2 */
                                if (p2_shared == px) {
                                    mutable_game->highlight_cyan[r2][c2] |= (1 << px);
                                } else {
                                    mutable_game->highlight_magenta[r2][c2] |= (1 << py);
                                }
                                mutable_game->highlight_green[r2][c2] |= (1 << z); /* Candidat z en Vert */

                                /* 4. Coloration en Rouge des candidats z éliminés */
                                for (int er = 0; er < 9; er++) {
                                    for (int ec = 0; ec < 9; ec++) {
                                        if ((er == pr && ec == pc) || (er == r1 && ec == c1) || (er == r2 && ec == c2)) {
                                            continue;
                                        }
                                        if (game->player_grid[er][ec] == 0 && can_see(r1, c1, er, ec) && can_see(r2, c2, er, ec)) {
                                            if (has_candidate(game, er, ec, z)) {
                                                mutable_game->highlight_red[er][ec] |= (1 << z);
                                            }
                                        }
                                    }
                                }

                                /* 5. Format synthétique pour l'interface graphique (GUI) */
                                snprintf(mutable_game->hint_description, sizeof(mutable_game->hint_description),
                                         "THEORIE - Y-Wing :\n"
                                         "Un pivot (%d, %d) avec pinces (%d, %d) et (%d, %d) "
                                         "impose la présence du %d dans l'une des pinces.\n\n"
                                         "APPLICATION :\n"
                                         "Pivot : (%d, %d) {%d, %d} | Pinces : (%d, %d), (%d, %d)\n"
                                         "-> Eliminer le candidat %d des cases voyant les 2 pinces.",
                                         pr + 1, pc + 1, r1 + 1, c1 + 1, r2 + 1, c2 + 1, z,
                                         pr + 1, pc + 1, px, py, r1 + 1, c1 + 1, r2 + 1, c2 + 1, z);

                                /* 6. Format détaillé pour le terminal (CLI) */
                                printf("📖 THÉORIE — Y-Wing (XY-Wing) :\n");
                                printf("   Le Y-Wing utilise 3 cases bivalentes : un pivot (%d, %d) et deux pinces.\n", pr + 1, pc + 1);
                                printf("   Si le pivot vaut %d, la pince 1 impose %d. S'il vaut %d, la pince 2 impose %d.\n", px, z, py, z);
                                printf("   Le candidat %d est donc obligatoirement présent dans l'une des deux pinces.\n\n", z);

                                printf("🎯 APPLICATION :\n");
                                printf("   Case Pivot : (%d, %d) [Candidats : " COLOR_CYAN "%d" COLOR_RESET ", " COLOR_MAGENTA "%d" COLOR_RESET "]\n", pr + 1, pc + 1, px, py);
                                printf("   Pince 1 : (%d, %d) | Pince 2 : (%d, %d)\n", r1 + 1, c1 + 1, r2 + 1, c2 + 1);
                                printf("   👉 Action : Élimination du " COLOR_RED "candidat %d" COLOR_RESET " dans les cases communes aux deux pinces.\n", z);
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