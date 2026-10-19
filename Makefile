# Compilateur
CC = gcc

# Détection automatique du système d'exploitation (Darwin = macOS, Linux = VPS)
UNAME_S := $(shell uname -s)

# Options de compilation communes
CFLAGS = -Wall -Wextra -std=c11 -DGAME_MODE -I. -Isources -Isources/Game -Isources/Generator

# Options et bibliothèques spécifiques selon la plateforme
ifeq ($(UNAME_S), Darwin)
    # Configuration macOS (Homebrew sur Apple Silicon / Intel)
    CFLAGS_GUI = -I/opt/homebrew/include -I/usr/local/include
    LIBS_GUI   = -L/opt/homebrew/lib -L/usr/local/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
else
    # Configuration Linux / VPS
    CFLAGS_GUI =
    LIBS_GUI   = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

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

# Règle par défaut : construit les deux binaires
all: $(TARGET_GUI) $(TARGET_CLI)

# Règle pour la version graphique
gui: $(TARGET_GUI)

$(TARGET_GUI): $(OBJS_GUI)
	$(CC) $(OBJS_GUI) -o $(TARGET_GUI) $(LIBS_GUI)

# Règle pour la version terminal
cli: $(TARGET_CLI)

$(TARGET_CLI): $(OBJS_CLI)
	$(CC) $(OBJS_CLI) -o $(TARGET_CLI) -lm

# Règle pour compiler le moteur C sous forme de bibliothèque dynamique
lib: $(SRCS_ENGINE)
	$(CC) $(CFLAGS) -fPIC -DGUI_MODE -shared $(SRCS_ENGINE) -o $(TARGET_LIB)

# Compilation des objets GUI (inclut les chemins d'en-tête Raylib et active -DGUI_MODE)
%_gui.o: %.c
	$(CC) $(CFLAGS) $(CFLAGS_GUI) -DGUI_MODE -c $< -o $@

# Compilation des objets CLI (sans -DGUI_MODE)
%_cli.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage complet des objets, des deux binaires et de la bibliothèque dynamique
clean:
	rm -f $(TARGET_GUI) $(TARGET_CLI) $(TARGET_LIB)
	find sources -name "*_gui.o" -delete
	find sources -name "*_cli.o" -delete

.PHONY: all gui cli lib clean