# Compilateur
CC = gcc

# Options de compilation communes
CFLAGS = -Wall -Wextra -std=c11 -DGAME_MODE -I. -Isources -Isources/Game -Isources/Generator

# Bibliothèques pour l'interface graphique Raylib
LIBS_GUI = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Fichiers sources communs du moteur C
SRCS_ENGINE = sources/Game/game.c \
              sources/Game/candidates.c \
              sources/Game/bank_loader.c \
              sources/Game/grid_transform.c \
              sources/Game/find_naked_single.c \
              sources/Game/find_hidden_single.c \
              sources/Game/find_naked_pair.c \
              sources/Game/find_hidden_pair.c \
              sources/Game/find_pointing_pairs.c \
              sources/Game/find_box_line_reduction.c \
              sources/Game/find_naked_triple.c \
              sources/Game/find_x_wing.c \
              sources/Game/find_y_wing.c \
              $(wildcard sources/Generator/*.c)

# Fichiers sources des points d'entrée
SRC_GUI = sources/sudoku_gui.c
SRC_CLI = sources/Game/main.c

# Objets compilés avec des extensions distinctes
OBJS_GUI = $(SRCS_ENGINE:.c=_gui.o) $(SRC_GUI:.c=_gui.o)
OBJS_CLI = $(SRCS_ENGINE:.c=_cli.o) $(SRC_CLI:.c=_cli.o)

# Chemins et noms exacts des exécutables cibles
TARGET_GUI = sudoku_gui
TARGET_CLI = sources/Game/sudoku_game
# Nom de la bibliothèque partagée pour macOS
TARGET_LIB = libsudoku.dylib

# Règle pour compiler le moteur C sous forme de bibliothèque dynamique
lib: $(SRCS_ENGINE)
	$(CC) $(CFLAGS) -fPIC -DGUI_MODE -shared $(SRCS_ENGINE) -o $(TARGET_LIB)

# Règle par défaut : construit les deux binaires
all: $(TARGET_GUI) $(TARGET_CLI)

# Target pour la version graphique
gui: $(TARGET_GUI)

$(TARGET_GUI): $(OBJS_GUI)
	$(CC) $(OBJS_GUI) -o $(TARGET_GUI) $(LIBS_GUI)

# Target pour la version terminal
cli: $(TARGET_CLI)

$(TARGET_CLI): $(OBJS_CLI)
	$(CC) $(OBJS_CLI) -o $(TARGET_CLI) -lm

# Règle de compilation des objets GUI (active -DGUI_MODE)
%_gui.o: %.c
	$(CC) $(CFLAGS) -DGUI_MODE -c $< -o $@

# Règle de compilation des objets CLI (sans -DGUI_MODE)
%_cli.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage complet des objets et des deux binaires
clean:
	rm -f $(TARGET_GUI) $(TARGET_CLI)
	find sources -name "*_gui.o" -delete
	find sources -name "*_cli.o" -delete

.PHONY: all gui cli clean