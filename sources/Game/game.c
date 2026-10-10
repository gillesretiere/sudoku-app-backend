#include <stdio.h>
#include "game.h"
#include "candidates.h"
#include "../Generator/generator.h"
#include "find_naked_single.h"
#include "find_hidden_single.h"
#include "find_naked_pair.h"
#include "find_hidden_pair.h"

// Séquences de couleurs ANSI pour le terminal
#define COLOR_RESET "\033[0m"
#define COLOR_GRAY  "\033[90m"
#define COLOR_WHITE "\033[1;37m"

typedef bool (*StrategyFn)(const SudokuGame *);

typedef struct {
    const char *level_name;
    StrategyFn *strategies;
    const char **strategy_names;
    int count;
} StrategyLevel;

// --- Niveau 0 : Stratégies triviales ---
static StrategyFn strat0[] = {
    find_naked_single,
    find_hidden_single
};

static const char *strat0_names[] = {
    "Candidat Unique (Naked Single)",
    "Chiffre Caché (Hidden Single)"
};

// --- Niveau 1 : Stratégies intermédiaires ---
static StrategyFn strat1[] = {
    find_naked_pair,
    find_hidden_pair
};

static const char *strat1_names[] = {
    "Paire Nue (Naked Pair)",
    "Paire Cachée (Hidden Pair)"
};

static StrategyLevel levels[] = {
    {
        .level_name = "Niveau 0 (Trivial)",
        .strategies = strat0,
        .strategy_names = strat0_names,
        .count = sizeof(strat0) / sizeof(strat0[0])
    },
    {
        .level_name = "Niveau 1 (Intermédiaire)",
        .strategies = strat1,
        .strategy_names = strat1_names,
        .count = sizeof(strat1) / sizeof(strat1[0])
    }
};

static const int N_LEVELS = sizeof(levels) / sizeof(levels[0]);

/**
 * Simule la résolution avec TOUTES les stratégies du Niveau 0 (Naked + Hidden Singles).
 * Renvoie true si la grille se bloque et nécessite au moins le Niveau 1 (Paires).
 */
static bool requires_pairs_strategy(const SudokuGame *game) {
    SudokuGame temp = *game;
    bool progress = true;

    while (progress) {
        progress = false;
        compute_autonote(&temp);

        // 1. Recherche de Naked Single
        for (int r = 0; r < 9 && !progress; r++) {
            for (int c = 0; c < 9 && !progress; c++) {
                if (temp.player_grid[r][c] == 0 && count_candidates(&temp, r, c) == 1) {
                    for (int val = 1; val <= 9; val++) {
                        if (has_candidate(&temp, r, c, val)) {
                            temp.player_grid[r][c] = val;
                            progress = true;
                            break;
                        }
                    }
                }
            }
        }

        if (progress) continue;

        // 2. Recherche de Hidden Single
        for (int unit_type = 0; unit_type < 3 && !progress; unit_type++) {
            for (int u = 0; u < 9 && !progress; u++) {
                for (int val = 1; val <= 9 && !progress; val++) {
                    int count = 0;
                    int target_r = -1, target_c = -1;

                    for (int i = 0; i < 9; i++) {
                        int r = (unit_type == 0) ? u : (unit_type == 1) ? i : (u / 3) * 3 + i / 3;
                        int c = (unit_type == 0) ? i : (unit_type == 1) ? u : (u % 3) * 3 + i % 3;

                        if (temp.player_grid[r][c] == 0 && has_candidate(&temp, r, c, val)) {
                            count++;
                            target_r = r;
                            target_c = c;
                        }
                    }

                    if (count == 1) {
                        temp.player_grid[target_r][target_c] = val;
                        progress = true;
                    }
                }
            }
        }
    }

    // Si des cases restent vides, la grille nécessite obligatoirement des Paires
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (temp.player_grid[r][c] == 0) {
                return true;
            }
        }
    }

    return false;
}

/**
 * Initialise une nouvelle partie avec un garde-fou sur le nombre de tentatives.
 */
void init_game(SudokuGame *game, int difficulty) {
    printf("Génération d'une grille calibrée pour le niveau %d...\n", difficulty);

    int attempts = 0;
    const int MAX_ATTEMPTS = 30; // Garde-fou anti-blocage
    bool valid_grid = false;

    while (!valid_grid && attempts < MAX_ATTEMPTS) {
        attempts++;
        generate_sudoku(game->initial_grid, game->solution, difficulty);

        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                game->player_grid[i][j] = game->initial_grid[i][j];
            }
        }

        if (difficulty == 3) {
            // Niveau 3 : exige l'échec de tous les Singles (Naked + Hidden)
            if (requires_pairs_strategy(game)) {
                valid_grid = true;
            }
        } else {
            valid_grid = true;
        }
    }

    compute_autonote(game);
    printf("Grille validée avec succès (%d tentative(s)).\n", attempts);
}

void print_game_board(const SudokuGame *game) {
    printf("\n    1 2 3   4 5 6   7 8 9\n");
    printf("  +-------+-------+-------+\n");
    
    for (int i = 0; i < 9; i++) {
        printf("%d | ", i + 1);
        for (int j = 0; j < 9; j++) {
            int val = game->player_grid[i][j];
            if (val == 0) {
                printf(". ");
            } else {
                printf("%d ", val);
            }
            if ((j + 1) % 3 == 0) {
                printf("| ");
            }
        }
        printf("\n");
        if ((i + 1) % 3 == 0) {
            printf("  +-------+-------+-------+\n");
        }
    }
    printf("\n");
}

bool make_move(SudokuGame *game, int row, int col, int value) {
    if (row < 0 || row > 8 || col < 0 || col > 8) {
        printf("❌ Erreur : La ligne et la colonne doivent être comprises entre 1 et 9.\n");
        return false;
    }

    if (game->initial_grid[row][col] != 0) {
        printf("❌ Erreur : La case (%d, %d) contient un chiffre fixe de départ !\n", row + 1, col + 1);
        return false;
    }

    if (value < 0 || value > 9) {
        printf("❌ Erreur : Le chiffre doit être entre 1 et 9 (ou 0 pour effacer).\n");
        return false;
    }

    if (value != 0) {
        for (int j = 0; j < 9; j++) {
            if (j != col && game->player_grid[row][j] == value) {
                printf("❌ Coup non éligible : Le chiffre %d est déjà présent sur la ligne %d !\n", value, row + 1);
                return false;
            }
        }

        for (int i = 0; i < 9; i++) {
            if (i != row && game->player_grid[i][col] == value) {
                printf("❌ Coup non éligible : Le chiffre %d est déjà présent dans la colonne %d !\n", value, col + 1);
                return false;
            }
        }

        int start_row = (row / 3) * 3;
        int start_col = (col / 3) * 3;

        for (int i = start_row; i < start_row + 3; i++) {
            for (int j = start_col; j < start_col + 3; j++) {
                if ((i != row || j != col) && game->player_grid[i][j] == value) {
                    printf("❌ Coup non éligible : Le chiffre %d est déjà présent dans la boîte 3x3 !\n", value);
                    return false;
                }
            }
        }

        if (game->solution[row][col] != value) {
            printf("❌ Oops... Mauvais choix ! : Le chiffre %d n'est pas la solution. C'est %d qu'il fallait jouer !\n", value, game->solution[row][col]);
            return false;
        }
    }

    game->player_grid[row][col] = (char)value;
    compute_autonote(game); // Re-calcule les candidats en mémoire après chaque coup
    return true;
}

bool check_victory(const SudokuGame *game) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (game->player_grid[i][j] == 0) {
                return false;
            }
        }
    }
    return true;
}

void provide_hint(const SudokuGame *game) {
    printf("\n--- RECHERCHE D'UN INDICE ---\n");

    for (int l = 0; l < N_LEVELS; l++) {
        StrategyLevel *lvl = &levels[l];

        for (int i = 0; i < lvl->count; i++) {
            if (lvl->strategies[i](game)) {
                printf("[Difficulté : %s | Stratégie : %s]\n", 
                       lvl->level_name, lvl->strategy_names[i]);
                printf("-------------------------------\n\n");
                return;
            }
        }
    }

    printf("Aucun indice trouvé avec les stratégies actuelles.\n");
    printf("-------------------------------\n\n");
}

void print_candidates_grid(const SudokuGame *game) {
    printf("\n==== GRILLE DES CANDIDATS (PENCILMARKS) ====\n");

    for (int r = 0; r < 9; r++) {
        if (r % 3 == 0) {
            printf("+-----------------+-----------------+-----------------+\n");
        }

        for (int sub_r = 0; sub_r < 3; sub_r++) {
            for (int c = 0; c < 9; c++) {
                if (c % 3 == 0) {
                    printf("|");
                } else {
                    printf(" ");
                }

                int val_already_set = game->player_grid[r][c];

                if (val_already_set != 0) {
                    if (sub_r == 1) {
                        printf("  %s%d%s  ", COLOR_WHITE, val_already_set, COLOR_RESET);
                    } else {
                        printf("     ");
                    }
                } else {
                    for (int sub_c = 0; sub_c < 3; sub_c++) {
                        int candidate_val = sub_r * 3 + sub_c + 1;

                        if (has_candidate(game, r, c, candidate_val)) {
                            printf("%s%d%s", COLOR_GRAY, candidate_val, COLOR_RESET);
                        } else {
                            printf("%s.%s", COLOR_GRAY, COLOR_RESET);
                        }

                        if (sub_c < 2) {
                            printf(" ");
                        }
                    }
                }
            }
            printf("|\n");
        }
    }
    printf("+-----------------+-----------------+-----------------+\n\n");
}