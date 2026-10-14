#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "candidates.h"
#include "bank_loader.h"

int main(void) {

    /* Initialisation du générateur de nombres aléatoires */
    srand((unsigned int)time(NULL));
    
    int difficulty = 0;
    char input[32];
    
    /* Saisie du niveau de difficulté (1 à 4) */
    while (difficulty < 1 || difficulty > 4) {
        printf("=== SÉLECTION DE LA DIFFICULTÉ ===\n");
        printf("  1. Facile    (Niveau N0)\n");
        printf("  2. Moyen     (Niveau N1)\n");
        printf("  3. Difficile (Niveau N2)\n");
        printf("  4. Expert    (Niveau N3)\n");
        printf("Votre choix (1-4) : ");

        if (fgets(input, sizeof(input), stdin) != NULL) {
            difficulty = atoi(input);
        }

        if (difficulty < 1 || difficulty > 4) {
            printf("⚠️ Choix invalide ! Veuillez saisir un chiffre entre 1 et 4.\n\n");
        }
    }

    SudokuGame game;

    /* Conversion du choix utilisateur (1..4) vers l'index de la banque (0..3) */
    int internal_level = difficulty - 1;

    printf("\nChargement d'une grille pour le niveau %d...\n", difficulty);

    /* Tentative de chargement depuis la banque de grilles vérifiées */
    if (!load_puzzle_from_bank(&game, internal_level)) {
        printf("⚠️ Chargement depuis la banque échoué. Génération par défaut...\n");
        init_game(&game, difficulty);
    } else {
        /* On réinitialise les masques de couleurs et la grille d'élimination */
        clear_highlights(&game);
        for (int r = 0; r < 9; r++) {
            for (int c = 0; c < 9; c++) {
                game.eliminated_mask[r][c] = 0;
            }
        }
    }

    /* Calcule les candidats initiaux */
    compute_autonote(&game);

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

        /* Commande Apply Hint : Retire les candidats éliminés de la grille des candidats */
        if (input[0] == 'a' || input[0] == 'A') {
            if (apply_current_hint(&game)) {
                print_candidates_grid(&game);
            }
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