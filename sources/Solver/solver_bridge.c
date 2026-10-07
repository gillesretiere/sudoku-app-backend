#include <stdio.h>
#include "solver_bridge.h"
#include "init.h"
#include "cleanup.h"
#include "execute_strategies.h"
#include "display_strats_in_clear.h"
#include "def.h"

/* Variables globales exportées par sudoku_solver.c */
extern int silent;
extern int n_strats_used;

void provide_hint(const SudokuGame *game) {
    /* 1. Conversion de player_grid en chaîne de 81 caractères */
    char grid_str[82];
    int idx = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            grid_str[idx++] = game->player_grid[i][j] + '0';
        }
    }
    grid_str[81] = '\0';

    /* 2. Initialisation du Solveur */
    silent = TRUE; /* Désactive les logs d'initialisation */
    init(grid_str);
    cleanup();

    /* 3. Recherche du premier indice par niveau de difficulté */
    n_strats_used = 0;
    int hint_found = FALSE;

    printf("\n💡 --- RECHERCHE D'UN INDICE ---\n");

    for (int level = 0; level < 4; level++) {
        if (execute_strategies(level)) {
            hint_found = TRUE;
            printf("Indice trouvé (Niveau de difficulté : %d) :\n", level);
            
            /* Affichage en clair de la stratégie trouvée */
            display_strats_in_clear();
            break; /* On s'arrête dès qu'un indice est trouvé */
        }
    }

    if (!hint_found) {
        printf("❌ Aucun indice direct trouvé avec les stratégies actuelles.\n");
    }
    printf("--------------------------------\n\n");
}