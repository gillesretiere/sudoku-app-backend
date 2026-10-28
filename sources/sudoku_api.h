#ifndef SUDOKU_API_H
#define SUDOKU_API_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Structure simplifiée d'une grille exportable pour les frontends (Raylib / Flutter FFI) */
typedef struct {
    uint8_t cells[9][9];       /* Valeurs des cases (0 = vide, 1-9 = chiffre) */
    uint8_t solution[9][9];    /* Grille de solution complète */
    float se_rating;           /* Score de difficulté exact (SE Rating) */
    int clues_count;           /* Nombre d'indices initiaux */
    bool is_valid;             /* Statut de génération */
} SudokuPuzzleDTO;

/**
 * @brief Génère une grille complète selon un niveau de 1 à 5.
 * 
 * @param level Niveau souhaité (1: Débutant, 2: Intermédiaire, 3: Avancé, 4: Expert, 5: Maître)
 * @param out_puzzle Pointer vers la structure recevant le puzzle et sa solution
 * @return true si la génération est réussie, false sinon
 */
bool sudoku_api_generate_level(int level, SudokuPuzzleDTO *out_puzzle);

#ifdef __cplusplus
}
#endif

#endif /* SUDOKU_API_H */