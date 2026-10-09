#include <stdio.h>
#include <stdlib.h>
#include "game.h"


int main(void) {

    int difficulty = 0;
    char input[32];
    
    /* Saisie du niveau de difficulté */
    while (difficulty < 1 || difficulty > 3) {
        printf("=== SÉLECTION DE LA DIFFICULTÉ ===\n");
        printf("  1. Facile   (Grille très fournie)\n");
        printf("  2. Moyen    (Equilibrée)\n");
        printf("  3. Difficile (Requiert des stratégies avancées)\n");
        printf("Votre choix (1-3) : ");

        if (fgets(input, sizeof(input), stdin) != NULL) {
            difficulty = atoi(input);
        }

        if (difficulty < 1 || difficulty > 3) {
            printf("⚠️ Choix invalide ! Veuillez saisir 1, 2 ou 3.\n\n");
        }
    }

    SudokuGame game;
    init_game(&game, difficulty);

  
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

        /* Commande AutoNote : Recalcule et affiche la grille des candidats */
        if (input[0] == 'n' || input[0] == 'N') {
            compute_autonote(&game);
            print_candidates_grid(&game); // Utilise désormais has_candidate()
            continue;
        }        
        
        /* Affichage des candidats */
        if (input[0] == 'p' || input[0] == 'P') {
            print_candidates_grid(&game);
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