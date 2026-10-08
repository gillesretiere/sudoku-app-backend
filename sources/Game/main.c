#include <stdio.h>
#include <stdlib.h>
#include "game.h"


int main(void) {
    SudokuGame game;
    init_game(&game);

    char input[32];
    int row, col, val;

    while (!check_victory(&game)) {
        print_game_board(&game);
        printf("Saisir (Ligne Colonne Valeur) ou 'h' pour un Indice : ");

                
        /* Option pour quitter la partie */
        if (input[0] == 'q' || input[0] == 'Q') {
            printf("Partie interrompue.\n");
            break;
        }

        if (fgets(input, sizeof(input), stdin) == NULL) continue;
        /* Détection de la demande d'indice */
        if (input[0] == 'h' || input[0] == 'H') {
            provide_hint(&game);
            continue;
        }

        /* Lecture des 3 entiers pour un coup classique */
        if (sscanf(input, "%d %d %d", &row, &col, &val) == 3) {
            /* Conversion des coordonnées (1-9 vers 0-8) */
            make_move(&game, row - 1, col - 1, val);
        } else {
            printf("⚠️ Saisie invalide ! Exemple : '3 4 5' ou 'h'.\n");
        }

    }

    printf("🎉 Félicitations ! Vous avez terminé la grille !\n");
    return EXIT_SUCCESS;
}