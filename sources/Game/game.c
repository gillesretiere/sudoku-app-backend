#include <stdio.h>
#include "game.h"
#include "candidates.h"
#include "bank_loader.h"
#include "../Generator/generator.h"
#include "find_naked_single.h"
#include "find_hidden_single.h"
#include "find_naked_pair.h"
#include "find_hidden_pair.h"
#include "find_pointing_pairs.h"
#include "find_box_line_reduction.h"
#include "find_naked_triple.h"
#include "find_x_wing.h"
#include "find_y_wing.h"

// Séquences de couleurs ANSI pour le terminal
#define COLOR_RESET "\033[0m"
#define COLOR_GRAY  "\033[90m"
#define COLOR_WHITE "\033[1;37m"
#define COLOR_RED     "\033[1;31m"  // Rouge (Éliminations)
#define COLOR_CYAN    "\033[1;36m"  // Cyan (Pivot Y)
#define COLOR_MAGENTA "\033[1;35m"  // Magenta / Rose (Pivot X)
#define COLOR_GREEN   "\033[1;32m"  // Vert (Pince Z)

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

// --- Niveau 2 : Stratégies avancées ---
static StrategyFn strat2[] = {
    find_pointing_pairs,
    find_box_line_reduction,
    find_naked_triple
};

static const char *strat2_names[] = {
    "Paires/Triplets Pointés (Pointing Pairs/Triples)",
    "Réduction Bloc-Ligne (Box-Line Reduction)",
    "Triplet Nu (Naked Triple)"
};

// --- Niveau 3 : Stratégies professionnelles ---
static StrategyFn strat3[] = {
    find_x_wing,
    find_y_wing
};

static const char *strat3_names[] = {
    "X-Wing",
    "Y-Wing (XY-Wing)"
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
    },
    {
        .level_name = "Niveau 2 (Avancé)",
        .strategies = strat2,
        .strategy_names = strat2_names,
        .count = sizeof(strat2) / sizeof(strat2[0])
    },
    {
        .level_name = "Niveau 3 (Professionnel)",
        .strategies = strat3,
        .strategy_names = strat3_names,
        .count = sizeof(strat3) / sizeof(strat3[0])
    }
};

static const int N_LEVELS = sizeof(levels) / sizeof(levels[0]);

void clear_highlights(SudokuGame *game) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            game->highlight_red[r][c] = 0;
            game->highlight_cyan[r][c] = 0;
            game->highlight_magenta[r][c] = 0;
            game->highlight_green[r][c] = 0;
        }
    }
}

void init_game(SudokuGame *game, int difficulty) {
    clear_highlights(game);
    printf("Chargement d'une grille pour le niveau %d...\n", difficulty);
    int target_level = difficulty - 1;
    // réinitialise le masque d'élimination
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            game->eliminated_mask[r][c] = 0;
        }
    }

    if (!load_puzzle_from_bank(game, target_level)) {
        printf("Fallback : Génération à la volée...\n");
        generate_sudoku(game->initial_grid, game->solution, difficulty);
        for (int r = 0; r < 9; r++) {
            for (int c = 0; c < 9; c++) {
                game->player_grid[r][c] = game->initial_grid[r][c];
            }
        }
    }
    compute_autonote(game);
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
    clear_highlights(game);
    compute_autonote(game);
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
    clear_highlights((SudokuGame *)game);
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
    const char *border = "+---------------------+---------------------+---------------------+";

    for (int r = 0; r < 9; r++) {
        if (r % 3 == 0) {
            printf("%s\n", border);
        }

        for (int sub_r = 0; sub_r < 3; sub_r++) {
            for (int c = 0; c < 9; c++) {
                if (c % 3 == 0) {
                    printf("|");
                }

                int val_already_set = game->player_grid[r][c];

                if (val_already_set != 0) {
                    if (sub_r == 1) {
                        printf("   %s%d%s   ", COLOR_WHITE, val_already_set, COLOR_RESET);
                    } else {
                        printf("       ");
                    }
                } else {
                    printf(" ");
                    for (int sub_c = 0; sub_c < 3; sub_c++) {
                        int candidate_val = sub_r * 3 + sub_c + 1;

                        bool is_active  = has_candidate(game, r, c, candidate_val);
                        bool is_red     = is_active && ((game->highlight_red[r][c] & (1 << candidate_val)) != 0);
                        bool is_green   = is_active && ((game->highlight_green[r][c] & (1 << candidate_val)) != 0);
                        bool is_cyan    = is_active && ((game->highlight_cyan[r][c] & (1 << candidate_val)) != 0);
                        bool is_magenta = is_active && ((game->highlight_magenta[r][c] & (1 << candidate_val)) != 0);

                        if (is_active) {
                            if (is_red) {
                                printf("%s%d%s", COLOR_RED, candidate_val, COLOR_RESET);
                            } else if (is_green) {
                                printf("%s%d%s", COLOR_GREEN, candidate_val, COLOR_RESET);
                            } else if (is_cyan) {
                                printf("%s%d%s", COLOR_CYAN, candidate_val, COLOR_RESET);
                            } else if (is_magenta) {
                                printf("%s%d%s", COLOR_MAGENTA, candidate_val, COLOR_RESET);
                            } else {
                                printf("%s%d%s", COLOR_GRAY, candidate_val, COLOR_RESET);
                            }
                        } else {
                            printf("%s.%s", COLOR_GRAY, COLOR_RESET);
                        }

                        if (sub_c < 2) {
                            printf(" ");
                        }
                    }
                    printf(" ");    
                }
            }
            printf("|\n");
        }
    }
    printf("%s\n\n", border);
}

bool apply_current_hint(SudokuGame *game) {
    bool applied = false;

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (game->highlight_red[r][c] != 0) {
                /* Transfère les candidats rouges dans le masque d'élimination permanent */
                game->eliminated_mask[r][c] |= game->highlight_red[r][c];
                applied = true;
            }
        }
    }

    if (applied) {
        clear_highlights(game);
        compute_autonote(game);
        printf("✨ Indice appliqué avec succès ! Les candidats rouges ont été éliminés.\n");
    } else {
        printf("⚠️ Aucun indice avec élimination à appliquer pour le moment.\n");
    }

    return applied;
}