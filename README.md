# Sudoku Kaï

## Installation Flutter et XCode
```
xcode-select --install
sudo xcode-select --switch /Applications/Xcode.app/Contents/Developer
sudo xcodebuild -runFirstLaunch

brew install --cask flutter
source ~/.zshrc

flutter doctor

brew install cocoapods
```

## Clonage du repo Git.
```
ssh -Y gre@vps-1263ad51.vps.ovh.net
cd ~/projets/sudoku
git clone https://github.com/gillesretiere/sudoku-app-backend.git
```

## Compilation des sources

### VPS
```
gcc -Wall -Wextra -std=c11 -I. -Isources/core -Isources/solver -Isources/generator -c sources/generator/generator_royle.c -o sources/generator/generator_royle.o
git status
git add .
git commit -m "Mise a jour de sudoku_gui.c et du Makefile"
git push origin phase2-refactoring
```

## Sur Mac (workflow complet de construction): 
```
# ----------------------------------------------------
# ÉTAPE 1 : Mettre à jour et tester le backend C / Raylib
# ----------------------------------------------------
cd ~/Documents/gre-24/dev/sudoku/sudoku-app-backend

# Récupérer les derniers changements depuis GitHub
git pull origin phase2-refactoring

# Recompiler 
make clean && make

# en cas de push, s'assurer que on utilise ssh et non https
git remote -v
git remote set-url origin git@github.com:gillesretiere/sudoku-app-backend.git
ssh -T git@github.com
git push origin HEAD:phase2-refactoring

# ----------------------------------------------------
# ÉTAPE 3 : Compiler et exécuter l'application Flutter
# ----------------------------------------------------
cd ~/Dev/sudoku_flutter
# copie de la lib libsudoku.dylib générée en amont (make)
cp /Users/retieregilles/Documents/gre-24/dev/sudoku/sudoku-app-backend/libsudoku.dylib ./
cp /Users/retieregilles/Documents/gre-24/dev/sudoku/sudoku-app-backend/libsudoku.dylib ./macos
cp /Users/retieregilles/Documents/gre-24/dev/sudoku/sudoku-app-backend/libsudoku.dylib  ~/Library/Containers/com.example.sudokuFlutter/Data/macos/

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