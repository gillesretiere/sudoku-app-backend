#include <raylib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "game.h"

#define GRID_SIZE 9
#define CELL_SIZE 60
#define OFFSET_X  30
#define OFFSET_Y  45

typedef enum {
    STATE_ATTRACT,
    STATE_PLAYING
} AppState;

typedef struct {
    Rectangle rec;
    const char *label;
    Color color;
    Color textColor;
} GuiButton;

typedef struct {
    Sound click;
    Sound place;
    Sound erase;
    Sound hint;
    Sound winZone;
    bool enabled;
} GameAudio;

// Génération manuelle et sécurisée d'une onde sinusoïdale en C
static Sound CreateSynthSound(float frequency, float duration, float volume) {
    int sampleRate = 44100;
    int sampleCount = (int)(sampleRate * duration);
    
    // Allocation du tampon audio (PCM 16 bits)
    short *samples = (short *)malloc(sampleCount * sizeof(short));
    if (!samples) return (Sound){ 0 };

    for (int i = 0; i < sampleCount; i++) {
        float t = (float)i / sampleRate;
        // Onde sinusoïdale avec fondu pour éviter le "clic"
        float env = 1.0f - (t / duration);
        float wave = sinf(2.0f * PI * frequency * t) * env;
        samples[i] = (short)(wave * 32000.0f * volume);
    }

    Wave wave = {
        .frameCount = sampleCount,
        .sampleRate = sampleRate,
        .sampleSize = 16,
        .channels = 1,
        .data = samples
    };

    Sound snd = LoadSoundFromWave(wave);
    free(samples);
    return snd;
}

static bool DrawButton(GuiButton btn, bool active) {
    if (!active) {
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

    DrawText(btn.label, textX, textY, fontSize, btn.textColor);

    return (isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}

static void DrawTextWrapped(const char *text, int startX, int startY, int maxWidth, int fontSize, Color color) {
    if (!text || text[0] == '\0') return;

    int cursorX = startX;
    int cursorY = startY;
    int lineHeight = fontSize + 3;

    const char *p = text;
    char word[64];
    int wordIdx = 0;

    while (*p != '\0') {
        if (*p == '\n') {
            if (wordIdx > 0) {
                word[wordIdx] = '\0';
                DrawText(word, cursorX, cursorY, fontSize, color);
                wordIdx = 0;
            }
            cursorX = startX;
            cursorY += lineHeight;
            p++;
            continue;
        }

        if (*p == ' ') {
            if (wordIdx > 0) {
                word[wordIdx] = '\0';
                int wordWidth = MeasureText(word, fontSize);
                if (cursorX + wordWidth > startX + maxWidth) {
                    cursorX = startX;
                    cursorY += lineHeight;
                }
                DrawText(word, cursorX, cursorY, fontSize, color);
                cursorX += wordWidth + MeasureText(" ", fontSize);
                wordIdx = 0;
            } else {
                cursorX += MeasureText(" ", fontSize);
            }
            p++;
            continue;
        }

        if (wordIdx < (int)sizeof(word) - 1) {
            word[wordIdx++] = *p;
        }
        p++;
    }

    if (wordIdx > 0) {
        word[wordIdx] = '\0';
        int wordWidth = MeasureText(word, fontSize);
        if (cursorX + wordWidth > startX + maxWidth) {
            cursorX = startX;
            cursorY += lineHeight;
        }
        DrawText(word, cursorX, cursorY, fontSize, color);
    }
}

static void PlayGameSound(Sound snd, bool enabled) {
    if (enabled && IsAudioDeviceReady() && snd.frameCount > 0) {
        PlaySound(snd);
    }
}

static bool is_row_complete(const SudokuGame *game, int r) {
    for (int c = 0; c < 9; c++) {
        if (game->player_grid[r][c] == 0) return false;
    }
    return true;
}

static bool is_col_complete(const SudokuGame *game, int c) {
    for (int r = 0; r < 9; r++) {
        if (game->player_grid[r][c] == 0) return false;
    }
    return true;
}

static bool is_box_complete(const SudokuGame *game, int b) {
    int start_r = (b / 3) * 3;
    int start_c = (b % 3) * 3;
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            if (game->player_grid[start_r + r][start_c + c] == 0) return false;
        }
    }
    return true;
}

int main(void) {
    const int screenWidth  = 920;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Sudoku Engine - Interface Graphique");
    SetTargetFPS(60);

    InitAudioDevice();

    GameAudio audio = {0};
    audio.enabled = IsAudioDeviceReady();

    if (audio.enabled) {
        // Tentative de chargement des fichiers .wav sinon génération synthétique
        audio.click   = FileExists("assets/click.wav")   ? LoadSound("assets/click.wav")   : CreateSynthSound(800.0f, 0.03f, 0.4f);
        audio.place   = FileExists("assets/place.wav")   ? LoadSound("assets/place.wav")   : CreateSynthSound(523.25f, 0.08f, 0.5f);
        audio.erase   = FileExists("assets/erase.wav")   ? LoadSound("assets/erase.wav")   : CreateSynthSound(261.63f, 0.10f, 0.5f);
        audio.hint    = FileExists("assets/hint.wav")    ? LoadSound("assets/hint.wav")    : CreateSynthSound(659.25f, 0.12f, 0.5f);
        audio.winZone = FileExists("assets/fanfare.wav") ? LoadSound("assets/fanfare.wav") : CreateSynthSound(880.0f, 0.25f, 0.6f);
    }

    SudokuGame game;
    AppState appState = STATE_ATTRACT;
    int current_level = 0;

    int attract_grid[9][9] = {0};
    Color attract_colors[9][9];
    float attract_timer = 0.0f;

    bool completed_rows[9] = {false};
    bool completed_cols[9] = {false};
    bool completed_boxes[9] = {false};

    float anim_rows[9] = {0};
    float anim_cols[9] = {0};
    float anim_boxes[9] = {0};

    bool menu_open = false;
    bool diff_menu_open = false;

    int selected_row = -1;
    int selected_col = -1;
    bool show_candidates = true;

    GuiButton btnHint  = { { 590, 45,  290, 34 }, "Indice (H)",    DARKBLUE, RAYWHITE };
    GuiButton btnApply = { { 590, 85,  290, 34 }, "Appliquer (A)", DARKGREEN, RAYWHITE };
    GuiButton btnNotes = { { 590, 125, 290, 34 }, "Masquer Notes (N)", DARKGRAY, RAYWHITE };

    GuiButton numButtons[9];
    char numLabels[9][2];
    for (int i = 0; i < 9; i++) {
        numLabels[i][0] = (char)('1' + i);
        numLabels[i][1] = '\0';

        int r = i / 3;
        int c = i % 3;

        float x = 602.0f + c * 93.0f;
        float y = 402.0f + r * 42.0f;

        numButtons[i] = (GuiButton){ { x, y, 80, 36 }, numLabels[i], YELLOW, BLACK };
    }

    GuiButton btnClear = { { 602, 528, 266, 34 }, "Effacer (0)", MAROON, RAYWHITE };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // 1. ANIMATION GUIRLANDE (MODE ATTRACT)
        if (appState == STATE_ATTRACT) {
            attract_timer += dt;
            if (attract_timer >= 0.05f) {
                attract_timer = 0.0f;
                int r = GetRandomValue(0, 8);
                int c = GetRandomValue(0, 8);
                if (GetRandomValue(0, 10) > 2) {
                    attract_grid[r][c] = GetRandomValue(1, 9);
                    attract_colors[r][c] = (Color){
                        (unsigned char)GetRandomValue(160, 255),
                        (unsigned char)GetRandomValue(160, 255),
                        (unsigned char)GetRandomValue(160, 255),
                        255
                    };
                } else {
                    attract_grid[r][c] = 0;
                }
            }
        }

        // 2. LOGIQUE EN JEU ET DÉTECTION DES VICTOIRES DE ZONE
        if (appState == STATE_PLAYING) {
            for (int i = 0; i < 9; i++) {
                if (anim_rows[i] > 0) anim_rows[i] -= dt;
                if (anim_cols[i] > 0) anim_cols[i] -= dt;
                if (anim_boxes[i] > 0) anim_boxes[i] -= dt;

                bool r_full = is_row_complete(&game, i);
                if (r_full && !completed_rows[i]) {
                    completed_rows[i] = true;
                    anim_rows[i] = 1.2f;
                    PlayGameSound(audio.winZone, audio.enabled);
                } else if (!r_full) {
                    completed_rows[i] = false;
                }

                bool c_full = is_col_complete(&game, i);
                if (c_full && !completed_cols[i]) {
                    completed_cols[i] = true;
                    anim_cols[i] = 1.2f;
                    PlayGameSound(audio.winZone, audio.enabled);
                } else if (!c_full) {
                    completed_cols[i] = false;
                }

                bool b_full = is_box_complete(&game, i);
                if (b_full && !completed_boxes[i]) {
                    completed_boxes[i] = true;
                    anim_boxes[i] = 1.2f;
                    PlayGameSound(audio.winZone, audio.enabled);
                } else if (!b_full) {
                    completed_boxes[i] = false;
                }
            }

            if (!menu_open) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    Vector2 mousePos = GetMousePosition();
                    int col = (int)(mousePos.x - OFFSET_X) / CELL_SIZE;
                    int row = (int)(mousePos.y - OFFSET_Y) / CELL_SIZE;

                    if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE) {
                        selected_row = row;
                        selected_col = col;
                        PlayGameSound(audio.click, audio.enabled);
                    }
                }

                if (selected_row != -1 && selected_col != -1) {
                    if (IsKeyPressed(KEY_UP) && selected_row > 0) selected_row--;
                    if (IsKeyPressed(KEY_DOWN) && selected_row < GRID_SIZE - 1) selected_row++;
                    if (IsKeyPressed(KEY_LEFT) && selected_col > 0) selected_col--;
                    if (IsKeyPressed(KEY_RIGHT) && selected_col < GRID_SIZE - 1) selected_col++;

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
                        if (digit_played == 0) {
                            PlayGameSound(audio.erase, audio.enabled);
                        } else {
                            PlayGameSound(audio.place, audio.enabled);
                        }
                    }
                }

                if (IsKeyPressed(KEY_H)) {
                    provide_hint(&game);
                    PlayGameSound(audio.hint, audio.enabled);
                }
                if (IsKeyPressed(KEY_A)) {
                    apply_current_hint(&game);
                    PlayGameSound(audio.hint, audio.enabled);
                }
                if (IsKeyPressed(KEY_N)) {
                    show_candidates = !show_candidates;
                    btnNotes.label = show_candidates ? "Masquer Notes (N)" : "Afficher Notes (N)";
                    PlayGameSound(audio.click, audio.enabled);
                }
            }
        }

        // 3. RENDU GRAPHIQUE
        BeginDrawing();
            ClearBackground(RAYWHITE);

            // A. PLATEAU DE JEU
            for (int r = 0; r < GRID_SIZE; r++) {
                for (int c = 0; c < GRID_SIZE; c++) {
                    int x = OFFSET_X + c * CELL_SIZE;
                    int y = OFFSET_Y + r * CELL_SIZE;
                    int b = (r / 3) * 3 + (c / 3);

                    if (appState == STATE_PLAYING && r == selected_row && c == selected_col) {
                        DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, YELLOW);
                    }

                    DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, LIGHTGRAY);

                    if (appState == STATE_PLAYING) {
                        bool is_animating = (anim_rows[r] > 0.0f || anim_cols[c] > 0.0f || anim_boxes[b] > 0.0f);

                        if (is_animating) {
                            Color win_color = (Color){
                                (unsigned char)GetRandomValue(180, 255),
                                (unsigned char)GetRandomValue(220, 255),
                                (unsigned char)GetRandomValue(150, 255),
                                255
                            };
                            DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, win_color);
                        } else {
                            bool has_hint_highlight = (game.highlight_green[r][c] != 0 || 
                                                       game.highlight_red[r][c] != 0 || 
                                                       game.highlight_cyan[r][c] != 0 || 
                                                       game.highlight_magenta[r][c] != 0);

                            if (has_hint_highlight) {
                                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, DARKGRAY);
                                DrawRectangleLinesEx((Rectangle){ (float)x, (float)y, (float)CELL_SIZE, (float)CELL_SIZE }, 3.0f, DARKGRAY);
                            }
                        }

                        int val = (int)game.player_grid[r][c];
                        int initial_val = (int)game.initial_grid[r][c];

                        if (val != 0) {
                            char text[2] = { (char)('0' + val), '\0' };
                            Color textColor = (initial_val != 0) ? DARKGRAY : BLACK;

                            if (!is_animating) {
                                Color bgColor = (initial_val != 0) ? LIGHTGRAY : WHITE;
                                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, bgColor);
                                DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, LIGHTGRAY);
                            }
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
                    } else {
                        int val = attract_grid[r][c];
                        if (val != 0) {
                            DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, attract_colors[r][c]);
                            char text[2] = { (char)('0' + val), '\0' };
                            DrawText(text, x + 22, y + 15, 30, DARKGRAY);
                        }
                    }
                }
            }

            // B. BORDURES DE BLOCS
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

            // C. PANNEAU BOUTONS
            bool can_act = (appState == STATE_PLAYING && !menu_open);

            if (DrawButton(btnHint, can_act)) {
                provide_hint(&game);
                PlayGameSound(audio.hint, audio.enabled);
            }
            if (DrawButton(btnApply, can_act)) {
                apply_current_hint(&game);
                PlayGameSound(audio.hint, audio.enabled);
            }
            if (DrawButton(btnNotes, can_act)) {
                show_candidates = !show_candidates;
                btnNotes.label = show_candidates ? "Masquer Notes (N)" : "Afficher Notes (N)";
                PlayGameSound(audio.click, audio.enabled);
            }

            // D. CARTE "ANALYSE & THÉORIE"
            Rectangle infoCard = { 590, 165, 290, 185 };
            DrawRectangleRounded(infoCard, 0.08f, 4, LIGHTGRAY);
            DrawRectangleRoundedLines(infoCard, 0.08f, 4, GRAY);

            DrawText("ANALYSE & THEORIE", 605, 173, 16, DARKBLUE);
            DrawLine(605, 193, 865, 193, GRAY);

            if (appState == STATE_PLAYING) {
                if (game.hint_description[0] != '\0') {
                    DrawTextWrapped(game.hint_description, 605, 201, 260, 10, DARKGRAY);
                } else {
                    DrawTextWrapped("Cliquez sur 'Indice' ou appuyez sur 'H' pour analyser la grille.", 
                                    605, 201, 260, 10, GRAY);
                }
            } else {
                DrawTextWrapped("Veuillez demarrer une partie via le menu (≡) pour jouer.", 
                                605, 201, 260, 10, GRAY);
            }

            // E. PAVÉ NUMÉRIQUE FIXE
            Rectangle keypadCard = { 590, 360, 290, 215 };
            DrawRectangleRounded(keypadCard, 0.08f, 4, LIGHTGRAY);
            DrawRectangleRoundedLines(keypadCard, 0.08f, 4, GRAY);

            DrawText("SAISIE MANUELLE", 605, 370, 16, DARKBLUE);
            DrawLine(605, 392, 865, 392, GRAY);

            bool is_cell_selected = can_act && (selected_row != -1 && selected_col != -1);
            bool is_cell_empty = is_cell_selected && (game.player_grid[selected_row][selected_col] == 0);
            bool is_cell_filled_by_player = is_cell_selected && 
                                            (game.player_grid[selected_row][selected_col] != 0) && 
                                            (game.initial_grid[selected_row][selected_col] == 0);

            for (int i = 0; i < 9; i++) {
                int digit = i + 1;
                bool is_digit_available = false;

                if (is_cell_empty) {
                    is_digit_available = has_candidate(&game, selected_row, selected_col, digit);
                }

                if (DrawButton(numButtons[i], is_digit_available)) {
                    make_move(&game, selected_row, selected_col, digit);
                    PlayGameSound(audio.place, audio.enabled);
                }
            }

            if (DrawButton(btnClear, is_cell_filled_by_player)) {
                make_move(&game, selected_row, selected_col, 0);
                PlayGameSound(audio.erase, audio.enabled);
            }

            // F. BARRE D'EN-TÊTE
            Rectangle headerRec = { 0, 0, (float)screenWidth, 35 };
            DrawRectangleRec(headerRec, DARKBLUE);

            GuiButton btnMenu = { { 5, 4, 32, 27 }, "≡", DARKBLUE, RAYWHITE };
            if (DrawButton(btnMenu, true)) {
                menu_open = !menu_open;
                diff_menu_open = false;
                PlayGameSound(audio.click, audio.enabled);
            }

            char status_text[64];
            if (appState == STATE_PLAYING) {
                snprintf(status_text, sizeof(status_text), "Partie Niveau %d", current_level);
            } else {
                snprintf(status_text, sizeof(status_text), "Aucune partie en cours");
            }
            DrawText(status_text, 50, 9, 18, RAYWHITE);

            // G. MENU HAMBURGER DÉROULANT
            if (menu_open) {
                Rectangle menuRec = { 5, 35, 190, 70 };
                DrawRectangleRounded(menuRec, 0.1f, 4, RAYWHITE);
                DrawRectangleRoundedLines(menuRec, 0.1f, 4, DARKGRAY);

                GuiButton btnStart = { { 10, 40, 180, 28 }, "Demarrer partie >", LIGHTGRAY, BLACK };
                GuiButton btnQuit  = { { 10, 72, 180, 28 }, "Quitter", MAROON, RAYWHITE };

                if (DrawButton(btnStart, true)) {
                    diff_menu_open = !diff_menu_open;
                    PlayGameSound(audio.click, audio.enabled);
                }

                if (DrawButton(btnQuit, true)) {
                    PlayGameSound(audio.click, audio.enabled);
                    break;
                }

                if (diff_menu_open) {
                    Rectangle diffRec = { 200, 35, 120, 128 };
                    DrawRectangleRounded(diffRec, 0.1f, 4, RAYWHITE);
                    DrawRectangleRoundedLines(diffRec, 0.1f, 4, DARKGRAY);

                    for (int lvl = 1; lvl <= 4; lvl++) {
                        char lvlLabel[16];
                        snprintf(lvlLabel, sizeof(lvlLabel), "Niveau %d", lvl);
                        GuiButton btnLvl = { { 205, 40 + (lvl - 1) * 30, 110, 26 }, lvlLabel, DARKGREEN, RAYWHITE };

                        if (DrawButton(btnLvl, true)) {
                            current_level = lvl;
                            init_game(&game, current_level);
                            appState = STATE_PLAYING;
                            selected_row = -1;
                            selected_col = -1;
                            menu_open = false;
                            diff_menu_open = false;

                            for (int k = 0; k < 9; k++) {
                                completed_rows[k] = false;
                                completed_cols[k] = false;
                                completed_boxes[k] = false;
                                anim_rows[k] = 0;
                                anim_cols[k] = 0;
                                anim_boxes[k] = 0;
                            }
                            PlayGameSound(audio.click, audio.enabled);
                        }
                    }
                }
            }

        EndDrawing();
    }

    if (audio.enabled) {
        UnloadSound(audio.click);
        UnloadSound(audio.place);
        UnloadSound(audio.erase);
        UnloadSound(audio.hint);
        UnloadSound(audio.winZone);
        CloseAudioDevice();
    }

    CloseWindow();
    return 0;
}