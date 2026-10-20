# Apress Source Code

This repository accompanies [*Sudoku Programming with C*](http://www.apress.com/9781484209967) by Giulio Zambon (Apress, 2015).

![Cover image](9781484209967.jpg)

Download the files as a zip using the green button, or clone the repository to your machine using Git.

## Releases

Release v1.0 corresponds to the code in the published book, without corrections or updates.

## Contributions

See the file Contributing.md for more information on how you can contribute to this repository.

# Adaptation
## Version CLI (C Language)

## Version Raylib

## Version Flutter
```
xcode-select --install
sudo xcode-select --switch /Applications/Xcode.app/Contents/Developer
sudo xcodebuild -runFirstLaunch

brew install --cask flutter
source ~/.zshrc

flutter doctor

brew install cocoapods
```


### Clonage du repo Git.
```
ssh -Y gre@vps-1263ad51.vps.ovh.net
cd ~/projets/sudoku
git clone https://github.com/gillesretiere/sudoku-app-backend.git
```

#### Compilation des sources

##### sudoku_game (Jeu)
```
gcc -Wall -Wextra -std=c11 -DGAME_MODE game.c main.c candidates.c bank_loader.c grid_transform.c find_naked_single.c find_hidden_single.c find_naked_pair.c find_hidden_pair.c find_pointing_pairs.c find_box_line_reduction.c find_naked_triple.c find_x_wing.c find_y_wing.c ../Generator/*.c -I. -I../Generator -o sudoku_game
```

##### inspect_bank (Inspecteur de grilles)
```
gcc -Wall -Wextra -std=c11 -DGAME_MODE inspect_bank.c game.c candidates.c solver_eval.c bank_loader.c grid_transform.c find_naked_single.c find_hidden_single.c find_naked_pair.c find_hidden_pair.c find_pointing_pairs.c find_box_line_reduction.c find_naked_triple.c find_x_wing.c find_y_wing.c ../Generator/*.c -I. -I../Generator -o inspect_bank
```

##### build_bank (Générateur de grilles)
```
gcc -Wall -Wextra -std=c11 -DGAME_MODE generate_bank.c game.c candidates.c solver_eval.c bank_loader.c grid_transform.c find_naked_single.c find_hidden_single.c find_naked_pair.c find_hidden_pair.c find_pointing_pairs.c find_box_line_reduction.c find_naked_triple.c find_x_wing.c find_y_wing.c ../Generator/*.c -I. -I../Generator -o build_bank
```
## Notes concernant les compilations séparéés CLI et GUI
### CLI
Penser à ajouter ce code en début de chaque fichier .h et .c
```
/* En mode GUI, la macro GUI_MODE neutralise uniquement les printf de la version graphique */
#ifdef GUI_MODE
    #define printf(...) ((void)0)
#endif
```
#### Compilation des deux versions
```
make clean && make
```

## Consignes de développement pour passer de Linux à Mac
Sur VPS Debian : 
```
# 1. Vérifie l'état de tes fichiers
git status

# 2. Ajoute les éventuelles modifications non enregistrées
git add .

# 3. Valide les modifications
git commit -m "Mise a jour de sudoku_gui.c et du Makefile"

# 4. Envoie les commits vers GitHub (étape essentielle !)
git push origin main

```
Sur Mac : 
```
cd ~/Documents/gre-24/dev/sudoku/sudoku-app-backend

# (optionnel) mise de côté (dans une *boîte de réserve*) des modifications locales sur le Mac, non publiables mais dont la présence empêche de récupérer les derniers changements (git pull).
git stash -u

# Récupérer les derniers changements depuis GitHub
git pull origin main

# Recompiler 
make clean && make
```
# Workflow de compilation
## Sur VPS
```
make clean && make all
./sources/Game/sudoku_game 

```


## Sur Mac (workflow complet de construction): 
```
# ----------------------------------------------------
# ÉTAPE 1 : Mettre à jour et tester le backend C / Raylib
# ----------------------------------------------------
cd ~/Documents/gre-24/dev/sudoku/sudoku-app-backend

# Récupérer les derniers changements depuis GitHub
git pull origin main

# Recompiler 
make clean && make

# Tester la version autonome
./sudoku_gui

# ----------------------------------------------------
# ÉTAPE 2 : Synchroniser les sources C vers le projet Flutter
# ----------------------------------------------------
# Copier les fichiers C mis à jour vers le dossier Runner/Game de Flutter
cp -R sources/Game/* ~/Dev/sudoku_flutter/macos/Runner/Game/

# ----------------------------------------------------
# ÉTAPE 3 : Compiler et exécuter l'application Flutter
# ----------------------------------------------------
cd ~/Dev/sudoku_flutter

# Nettoyer les métadonnées cachées de macOS
xattr -cr .

# Purger l'ancien build pour forcer la recompilation des fichiers C
flutter clean
flutter pub get

# Lancer la compilation native macOS
flutter run -d macos
```

# Liaison Dart (FFI) : Flutter charge la bibliothèque et appelle la fonction C native au clic sur le bouton.
## Gestion de l'audio
```
cd ~/Dev/sudoku_flutter
flutter pub add audioplayers
``` 


## Banques de sons prêtes à l'emploi
- Kenney.nl (Packs Audio) : La référence absolue pour les développeurs indépendants. Tous les éléments sont sous licence CC0 (Domaine Public, utilisables sans restriction ni attribution). Les packs UI Audio et Digital Audio contiennent exactement les bruits de clics, de validations et de victoires adaptés à une interface.
- OpenGameArt.org : Site communautaire dédié aux ressources de jeux vidéo. En cherchant des mots-clés comme UI click, correct ou fanfare, tu trouveras de nombreux kits de bruitages (filtre la recherche sur la licence CC0 ou CC-BY).
- Pixabay Audio & Mixkit.co : Proposent des bibliothèques d'effets sonores (Sound Effects) téléchargeables gratuitement en .wav ou .mp3, utilisables directement sans inscription.
- Freesound.org : Base de données collaborative géante. Utile si tu cherches un son très spécifique, mais pense à vérifier la licence de chaque fichier avant téléchargement.