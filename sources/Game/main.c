#include <stdio.h>
#include <stdbool.h>
#include "game.h"

int main(void) {
    SudokuGame game;
    init_game(&game);

    printf("=================================\n");
    printf("   SUDOKU APP - BOUCLE DE JEU    \n");
    printf("=================================\n");

    bool running = true;
    while (running) {
        print_game_board(&game);

        int row = 0, col = 0, val = 0;
        printf("Entrez votre coup (Ligne [1-9] Colonne [1-9] Valeur [0-9]), ou 0 0 0 pour quitter : ");
        
        if (scanf("%d %d %d", &row, &col, &val) != 3) {
            printf("⚠️ Saisie invalide. Veuillez entrer 3 chiffres.\n");
            while (getchar() != '\n'); /* Nettoie le tampon de saisie */
            continue;
        }

        /* Option pour quitter la partie */
        if (row == 0 && col == 0 && val == 0) {
            printf("Partie interrompue.\n");
            running = false;
            break;
        }

        /* Conversion des indices (l'utilisateur entre 1-9, le C utilise 0-8) */
        if (make_move(&game, row - 1, col - 1, val)) {
            if (check_victory(&game)) {
                print_game_board(&game);
                printf(" Bravo ! La grille est complétée !\n");
                running = false;
            }
        }
    }

    return 0;
}