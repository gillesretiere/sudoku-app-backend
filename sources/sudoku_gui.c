#include <raylib.h>
#include <stdio.h>
#include <stdbool.h>
#include "game.h"

// Constantes d'affichage de la grille
#define GRID_SIZE 9
#define CELL_SIZE 60
#define OFFSET_X  30
#define OFFSET_Y  30

// Structure pour les boutons graphiques
typedef struct {
    Rectangle rec;
    const char *label;
    Color color;
} GuiButton;

// Dessine un bouton et renvoie 'true' si le joueur clique dessus (si actif)
static bool DrawButton(GuiButton btn, bool active) {
    if (!active) {
        // Rendu visuel d'un bouton désactivé
        DrawRectangleRounded(btn.rec, 0.2f, 4, LIGHTGRAY);
        DrawRectangleRoundedLines(btn.rec, 0.2f, 4, GRAY);

        int fontSize = 18;
        int textWidth = MeasureText(btn.label, fontSize);
        int textX = (int)(btn.rec.x + (btn.rec.width - textWidth) / 2);
        int textY = (int)(btn.rec.y + (btn.rec.height - fontSize) / 2);

        DrawText(btn.label, textX, textY, fontSize, GRAY);
        return false;
    }

    Vector2 mousePos = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mousePos, btn.rec);
    
    Color drawColor = isHovered ? ColorAlpha(btn.color, 0.85f) : btn.color;

    DrawRectangleRounded(btn.rec, 0.2f, 4, drawColor);
    DrawRectangleRoundedLines(btn.rec, 0.2f, 4, DARKGRAY);

    int fontSize = 18;
    int textWidth = MeasureText(btn.label, fontSize);
    int textX = (int)(btn.rec.x + (btn.rec.width - textWidth) / 2);
    int textY = (int)(btn.rec.y + (btn.rec.height - fontSize) / 2);

    DrawText(btn.label, textX, textY, fontSize, RAYWHITE);

    return (isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}

int main(void) {
    const int screenWidth  = 920;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Sudoku Engine - Interface Graphique");
    SetTargetFPS(60);

    SudokuGame game;
    init_game(&game, 1);

    int selected_row = -1;
    int selected_col = -1;
    bool show_candidates = true;

    // Boutons d'action principaux
    GuiButton btnHint  = { { 590, 20,  290, 40 }, "Indice (H)",    DARKBLUE };
    GuiButton btnApply = { { 590, 68,  290, 40 }, "Appliquer (A)", DARKGREEN };
    GuiButton btnNotes = { { 590, 116, 290, 40 }, "Masquer Notes (N)", DARKGRAY };

    // Initialisation du pavé numérique
    GuiButton numButtons[9];
    char numLabels[9][2];
    for (int i = 0; i < 9; i++) {
        numLabels[i][0] = (char)('1' + i);
        numLabels[i][1] = '\0';

        int r = i / 3;
        int c = i % 3;

        float x = 602.0f + c * 93.0f;
        float y = 402.0f + r * 42.0f;

        numButtons[i] = (GuiButton){ { x, y, 80, 36 }, numLabels[i], YELLOW };
    }

    GuiButton btnClear = { { 602, 528, 266, 34 }, "Effacer (0)", MAROON };

    while (!WindowShouldClose()) {
        // ==========================================
        // 1. GESTION DES ENTRÉES CLAVIER & SOURIS
        // ==========================================

        // Clic sur la grille
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();

            int col = (int)(mousePos.x - OFFSET_X) / CELL_SIZE;
            int row = (int)(mousePos.y - OFFSET_Y) / CELL_SIZE;

            if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE) {
                selected_row = row;
                selected_col = col;
            }
        }

        // Navigation au clavier (Flèches)
        if (selected_row != -1 && selected_col != -1) {
            if (IsKeyPressed(KEY_UP) && selected_row > 0) selected_row--;
            if (IsKeyPressed(KEY_DOWN) && selected_row < GRID_SIZE - 1) selected_row++;
            if (IsKeyPressed(KEY_LEFT) && selected_col > 0) selected_col--;
            if (IsKeyPressed(KEY_RIGHT) && selected_col < GRID_SIZE - 1) selected_col++;
        }

        // Saisie au clavier physique
        if (selected_row != -1 && selected_col != -1) {
            int digit_played = -1;

            for (int d = 1; d <= 9; d++) {
                if (IsKeyPressed(KEY_ONE + (d - 1)) || IsKeyPressed(KEY_KP_1 + (d - 1))) {
                    digit_played = d;
                    break;
                }
            }

            if (IsKeyPressed(KEY_ZERO) || IsKeyPressed(KEY_KP_0) || 
                IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_DELETE)) {
                digit_played = 0;
            }

            if (digit_played != -1) {
                make_move(&game, selected_row, selected_col, digit_played);
            }
        }

        // Raccourcis clavier
        if (IsKeyPressed(KEY_H)) provide_hint(&game);
        if (IsKeyPressed(KEY_A)) apply_current_hint(&game);
        if (IsKeyPressed(KEY_N)) {
            show_candidates = !show_candidates;
            btnNotes.label = show_candidates ? "Masquer Notes (N)" : "Afficher Notes (N)";
        }

        // ==========================================
        // 2. RENDU GRAPHIQUE
        // ==========================================
        BeginDrawing();
            ClearBackground(RAYWHITE);

            // --- A. PLATEAU DE JEU ---
            for (int r = 0; r < GRID_SIZE; r++) {
                for (int c = 0; c < GRID_SIZE; c++) {
                    int x = OFFSET_X + c * CELL_SIZE;
                    int y = OFFSET_Y + r * CELL_SIZE;

                    if (r == selected_row && c == selected_col) {
                        DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, YELLOW);
                    }

                    DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, LIGHTGRAY);

                    int val = (int)game.player_grid[r][c];
                    int initial_val = (int)game.initial_grid[r][c];

                    if (val != 0) {
                        char text[2] = { (char)('0' + val), '\0' };
                        Color textColor = (initial_val != 0) ? DARKGRAY : BLUE;
                        DrawText(text, x + 22, y + 15, 30, textColor);
                    } else if (show_candidates) {
                        for (int cand = 1; cand <= 9; cand++) {
                            if (has_candidate(&game, r, c, cand)) {
                                int sub_r = (cand - 1) / 3;
                                int sub_c = (cand - 1) % 3;
                                int cand_x = x + 8 + sub_c * 18;
                                int cand_y = y + 5 + sub_r * 18;

                                uint16_t mask = (1 << cand);
                                Color cand_color = GRAY;

                                if (game.highlight_red[r][c] & mask)       cand_color = RED;
                                else if (game.highlight_green[r][c] & mask) cand_color = GREEN;
                                else if (game.highlight_cyan[r][c] & mask)  cand_color = SKYBLUE;
                                else if (game.highlight_magenta[r][c] & mask) cand_color = PURPLE;

                                char cand_str[2] = { (char)('0' + cand), '\0' };
                                DrawText(cand_str, cand_x, cand_y, 14, cand_color);
                            }
                        }
                    }
                }
            }

            // --- B. BORDURES DES BLOCS 3x3 ---
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

            // --- C. BOUTONS D'ACTION HAUT ---
            if (DrawButton(btnHint, true))  provide_hint(&game);
            if (DrawButton(btnApply, true)) apply_current_hint(&game);
            if (DrawButton(btnNotes, true)) {
                show_candidates = !show_candidates;
                btnNotes.label = show_candidates ? "Masquer Notes (N)" : "Afficher Notes (N)";
            }

            // --- D. CARTE "ANALYSE & THÉORIE" ---
            Rectangle infoCard = { 590, 168, 290, 180 };
            DrawRectangleRounded(infoCard, 0.08f, 4, LIGHTGRAY);
            DrawRectangleRoundedLines(infoCard, 0.08f, 4, GRAY);

            DrawText("ANALYSE & THÉORIE", 605, 178, 16, DARKBLUE);
            DrawLine(605, 200, 865, 200, GRAY);

            if (game.hint_description[0] != '\0') {
                DrawText(game.hint_description, 605, 212, 14, DARKGRAY);
            } else {
                DrawText("Cliquez sur 'Indice' ou\nappuyez sur la touche 'H'\npour analyser la grille.", 
                         605, 212, 14, GRAY);
            }

            // --- E. PAVÉ NUMÉRIQUE FIXE DYNAMIQUE ---
            Rectangle keypadCard = { 590, 360, 290, 215 };
            DrawRectangleRounded(keypadCard, 0.08f, 4, LIGHTGRAY);
            DrawRectangleRoundedLines(keypadCard, 0.08f, 4, GRAY);

            DrawText("SAISIE MANUELLE", 605, 370, 16, DARKBLUE);
            DrawLine(605, 392, 865, 392, GRAY);

            // Vérification de l'état de la case sélectionnée
            bool is_cell_selected = (selected_row != -1 && selected_col != -1);
            bool is_cell_empty = is_cell_selected && (game.player_grid[selected_row][selected_col] == 0);
            bool is_cell_filled_by_player = is_cell_selected && 
                                            (game.player_grid[selected_row][selected_col] != 0) && 
                                            (game.initial_grid[selected_row][selected_col] == 0);

            // Dessin des boutons 1 à 9
            for (int i = 0; i < 9; i++) {
                int digit = i + 1;
                bool is_digit_available = false;

                if (is_cell_empty) {
                    is_digit_available = has_candidate(&game, selected_row, selected_col, digit);
                }

                if (DrawButton(numButtons[i], is_digit_available)) {
                    make_move(&game, selected_row, selected_col, digit);
                }
            }

            // Dessin du bouton Effacer (actif uniquement si la case contient un coup du joueur)
            if (DrawButton(btnClear, is_cell_filled_by_player)) {
                make_move(&game, selected_row, selected_col, 0);
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}