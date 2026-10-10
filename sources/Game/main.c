#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "candidates.h"

int main(void) {

    int difficulty = 0;
    char input[32];
    
    /* Saisie du niveau de difficulté (1 à 4) */
    while (difficulty < 1 || difficulty > 4) {
        printf("=== SÉLECTION DE LA DIFFICULTÉ ===\n");
        printf("  1. Facile    (~48 indices)\n");
        printf("  2. Moyen     (~41 indices)\n");
        printf("  3. Difficile (~32 indices)\n");
        printf("  4. Expert    (~28 indices)\n");
        printf("Votre choix (1-4) : ");

        if (fgets(input, sizeof(input), stdin) != NULL) {
            difficulty = atoi(input);
        }

        if (difficulty < 1 || difficulty > 4) {
            printf("⚠️ Choix invalide ! Veuillez saisir un chiffre entre 1 et 4.\n\n");
        }
    }

    SudokuGame game;
    init_game(&game, difficulty);

    int row, col, val;

    while (!check_victory(&game)) {
        print_game_board(&game);
        printf("Saisir (Ligne Colonne Valeur), 'n' pour AutoNote, 'h' pour Indice, 'q' pour Quitter : ");

        if (fgets(input, sizeof(input), stdin) == NULL) continue;

        /* Option pour quitter la partie */
        if (input[0] == 'q' || input[0] == 'Q') {
            printf("Partie interrompue.\n");
            break;
        }

        /* Détection de la demande d'indice */
        if (input[0] == 'h' || input[0] == 'H') {
            provide_hint(&game);
            continue;
        }

        /* Commande AutoNote : Recalcule et affiche la grille des candidats */
        if (input[0] == 'n' || input[0] == 'N') {
            compute_autonote(&game);
            print_candidates_grid(&game);
            continue;
        }        

        /* Lecture des 3 entiers pour un coup classique */
        if (sscanf(input, "%d %d %d", &row, &col, &val) == 3) {
            make_move(&game, row - 1, col - 1, val);
        } else {
            printf("⚠️ Saisie invalide ! Exemple : '3 4 5' ou 'h'.\n");
        }
    }
        
    print_game_board(&game);

    printf("🎉 Félicitations ! Vous avez terminé la grille !\n");
    return EXIT_SUCCESS;
}