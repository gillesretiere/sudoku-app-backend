#include <stdio.h>
#include "game.h"
#include "../Generator/generator.h"
#include "find_naked_single.h"
#include "find_hidden_single.h"

// 1. Définition du type pointeur de fonction pour une stratégie
typedef bool (*StrategyFn)(const SudokuGame *);

// 2. Structure regroupant les données d'un niveau de difficulté
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

// --- Tableau général des niveaux ---
static StrategyLevel levels[] = {
    {
        .level_name = "Niveau 0 (Trivial)",
        .strategies = strat0,
        .strategy_names = strat0_names,
        .count = sizeof(strat0) / sizeof(strat0[0])
    }
    /* Les futurs niveaux (Niveau 1, Niveau 2...) s'ajouteront ici simplement */
};

static const int N_LEVELS = sizeof(levels) / sizeof(levels[0]);

/**
 * Initialise une nouvelle partie en générant une grille dynamique.
 */
void init_game(SudokuGame *game) {
    printf("Génération d'une nouvelle grille de Sudoku en cours...\n");

    /* Appel du moteur de génération d'Apress */
    generate_sudoku(game->initial_grid, game->solution);

    /* Recopie de la grille initiale vers la grille de jeu du joueur */
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            game->player_grid[i][j] = game->initial_grid[i][j];
        }
    }
}

/**
 * Affiche la grille dans le terminal avec les coordonnées (lignes et colonnes).
 */
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

/**
 * Valide et applique le coup du joueur selon les règles du Sudoku.
 */
bool make_move(SudokuGame *game, int row, int col, int value) {
    /* 1. Validation des coordonnées */
    if (row < 0 || row > 8 || col < 0 || col > 8) {
        printf("❌ Erreur : La ligne et la colonne doivent être comprises entre 1 et 9.\n");
        return false;
    }

    /* 2. Validation des cases immuables */
    if (game->initial_grid[row][col] != 0) {
        printf("❌ Erreur : La case (%d, %d) contient un chiffre fixe de départ !\n", row + 1, col + 1);
        return false;
    }

    /* 3. Validation de la plage de valeurs */
    if (value < 0 || value > 9) {
        printf("❌ Erreur : Le chiffre doit être entre 1 et 9 (ou 0 pour effacer).\n");
        return false;
    }

    /* 4. Contrôle d'éligibilité (si la valeur n'est pas 0) */
    if (value != 0) {
        /* A. Vérification de la ligne */
        for (int j = 0; j < 9; j++) {
            if (j != col && game->player_grid[row][j] == value) {
                printf("❌ Coup non éligible : Le chiffre %d est déjà présent sur la ligne %d !\n", value, row + 1);
                return false;
            }
        }

        /* B. Vérification de la colonne */
        for (int i = 0; i < 9; i++) {
            if (i != row && game->player_grid[i][col] == value) {
                printf("❌ Coup non éligible : Le chiffre %d est déjà présent dans la colonne %d !\n", value, col + 1);
                return false;
            }
        }

        /* C. Vérification de la boîte 3x3 */
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
        /* D. Vérification avec la solution */
        if (game->solution[row][col] != value) {
            printf("❌ Oops... Mauvais choix ! : Le chiffre %d n'est pas la solution. C'est %d qu'il fallait jouer !\n", value, game->solution[row][col]);
            return false;
        }
    }

    /* 5. Application du coup valide */
    game->player_grid[row][col] = (char)value;
    return true;
}

/**
 * Vérifie si la grille est entièrement remplie.
 */
bool check_victory(const SudokuGame *game) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (game->player_grid[i][j] == 0) {
                return false; /* Il reste au moins une case vide */
            }
        }
    }
    return true;
}

// Function principale d'indice
void provide_hint(const SudokuGame *game) {
    printf("\n--- RECHERCHE D'UN INDICE ---\n");

    // Parcours de chaque niveau de difficulté
    for (int l = 0; l < N_LEVELS; l++) {
        StrategyLevel *lvl = &levels[l];

        // Parcours des stratégies au sein du niveau courant
        for (int i = 0; i < lvl->count; i++) {
            // Appel dynamique de la stratégie via son pointeur de fonction
            if (lvl->strategies[i](game)) {
                printf("[Difficulté : %s | Stratégie : %s]\n", 
                       lvl->level_name, lvl->strategy_names[i]);
                printf("-------------------------------\n\n");
                return; // Indice trouvé, on sort immédiatement
            }
        }
    }

    printf("Aucun indice trouvé avec les stratégies actuelles.\n");
    printf("-------------------------------\n\n");
}
