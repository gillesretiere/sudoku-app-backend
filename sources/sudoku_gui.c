#include <raylib.h>
#include <stdio.h>
#include "game.h"

// Constantes d'affichage de la grille
#define GRID_SIZE 9
#define CELL_SIZE 60
#define OFFSET_X  30
#define OFFSET_Y  30

int main(void) {
    const int screenWidth = 600;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Sudoku Engine - Interface Raylib");
    SetTargetFPS(60);

    // Initialisation du jeu Sudoku (niveau 1)
    SudokuGame game;
    init_game(&game, 1);

    // Variables pour la sélection de case (-1 = aucune sélection)
    int selected_row = -1;
    int selected_col = -1;

    while (!WindowShouldClose()) {
        // ==========================================
        // 1. GESTION DES ENTRÉES SOURIS ET CLAVIER
        // ==========================================

        // --- Clic de souris ---
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();

            int col = (int)(mousePos.x - OFFSET_X) / CELL_SIZE;
            int row = (int)(mousePos.y - OFFSET_Y) / CELL_SIZE;

            if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE) {
                selected_row = row;
                selected_col = col;
            } else {
                selected_row = -1;
                selected_col = -1;
            }
        }

        // --- Navigation au clavier (Flèches) ---
        if (selected_row != -1 && selected_col != -1) {
            if (IsKeyPressed(KEY_UP) && selected_row > 0) selected_row--;
            if (IsKeyPressed(KEY_DOWN) && selected_row < GRID_SIZE - 1) selected_row++;
            if (IsKeyPressed(KEY_LEFT) && selected_col > 0) selected_col--;
            if (IsKeyPressed(KEY_RIGHT) && selected_col < GRID_SIZE - 1) selected_col++;
        }

        // --- Saisie des chiffres ---
        if (selected_row != -1 && selected_col != -1) {
            int digit_played = -1;

            // Clavier principal (1 à 9) et Pavé numérique
            for (int d = 1; d <= 9; d++) {
                if (IsKeyPressed(KEY_ONE + (d - 1)) || IsKeyPressed(KEY_KP_1 + (d - 1))) {
                    digit_played = d;
                    break;
                }
            }

            // Touches d'effacement (0, Backspace, Delete)
            if (IsKeyPressed(KEY_ZERO) || IsKeyPressed(KEY_KP_0) || 
                IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_DELETE)) {
                digit_played = 0;
            }

            // Si un chiffre a été saisi, appel de la logique du moteur C
            if (digit_played != -1) {
                make_move(&game, selected_row, selected_col, digit_played);
            }
        }

        // ==========================================
        // 2. RENDU GRAPHIQUE
        // ==========================================
        BeginDrawing();
            ClearBackground(RAYWHITE);

            // Dessin des cases et de leur état
            for (int r = 0; r < GRID_SIZE; r++) {
                for (int c = 0; c < GRID_SIZE; c++) {
                    int x = OFFSET_X + c * CELL_SIZE;
                    int y = OFFSET_Y + r * CELL_SIZE;

                    // Fond de case : surlignage jaune si la case est sélectionnée
                    if (r == selected_row && c == selected_col) {
                        DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, YELLOW);
                    }

                    // Contour fin de la case
                    DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, LIGHTGRAY);

                    // Valeurs de la grille
                    int val = (int)game.player_grid[r][c];
                    int initial_val = (int)game.initial_grid[r][c];

                    if (val != 0) {
                        char text[2];
                        snprintf(text, sizeof(text), "%d", val);

                        // Distinction : Chiffre fixe = DARKGRAY, coup du joueur = BLUE
                        Color textColor = (initial_val != 0) ? DARKGRAY : BLUE;

                        DrawText(text, x + 22, y + 15, 30, textColor);
                    }
                }
            }

            // Dessin des bordures épaisses 3x3
            for (int i = 0; i <= GRID_SIZE; i += 3) {
                int posX = OFFSET_X + i * CELL_SIZE;
                int posY = OFFSET_Y + i * CELL_SIZE;

                DrawLineEx((Vector2){(float)posX, (float)OFFSET_Y}, 
                           (Vector2){(float)posX, (float)(OFFSET_Y + 9 * CELL_SIZE)}, 
                           4.0f, BLACK);

                DrawLineEx((Vector2){(float)OFFSET_X, (float)posY}, 
                           (Vector2){(float)(OFFSET_X + 9 * CELL_SIZE), (float)posY}, 
                           4.0f, BLACK);
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}