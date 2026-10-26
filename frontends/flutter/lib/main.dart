import 'dart:async';
import 'dart:io';
import 'dart:math';
import 'package:flutter/material.dart';

import 'audio/audio_manager.dart';
import 'services/sudoku_ffi.dart';
import 'theme/sudoku_palette.dart';

void main() {
  WidgetsFlutterBinding.ensureInitialized();
  runApp(const SudokuApp());
}

/// Dessinateur personnalisé pour biffer un candidat avec un trait diagonal rouge.
class StrikePainter extends CustomPainter {
  final Color color;
  final double strokeWidth;

  StrikePainter({
    required this.color,
    this.strokeWidth = 1.2,
  });

  @override
  void paint(Canvas canvas, Size size) {
    final paint = Paint()
      ..color = color
      ..strokeWidth = strokeWidth
      ..strokeCap = StrokeCap.round;

    // Diagonale du coin inférieur gauche vers le coin supérieur droit
    canvas.drawLine(
      Offset(2, size.height - 2),
      Offset(size.width - 2, 2),
      paint,
    );
  }

  @override
  bool shouldRepaint(covariant StrikePainter oldDelegate) {
    return oldDelegate.color != color || oldDelegate.strokeWidth != strokeWidth;
  }
}

class SudokuApp extends StatelessWidget {
  const SudokuApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Sudoku Engine',
      theme: ThemeData(
        scaffoldBackgroundColor: SudokuPalette.classic.background,
        primarySwatch: Colors.indigo,
      ),
      home: const SudokuGridPage(),
      debugShowCheckedModeBanner: false,
    );
  }
}

class SudokuGridPage extends StatefulWidget {
  const SudokuGridPage({super.key});

  @override
  State<SudokuGridPage> createState() => _SudokuGridPageState();
}

class _SudokuGridPageState extends State<SudokuGridPage> {
  // --- État de la partie ---
  bool _isPlaying = false;
  int _currentLevel = 0;

  // --- Grilles de jeu ---
  List<int> _initialGrid = List.filled(81, 0);
  List<int> _playerGrid = List.filled(81, 0);
  List<int> _candidatesGrid = List.filled(81, 0);

  // --- Gestion de la Gomme ---
  bool _isEraserMode = false;
  List<int> _eliminatedCandidates = List.filled(81, 0);

  // --- Animation Mosaïque (Mode Attract) ---
  final List<int> _attractGrid = List.filled(81, 0);
  final List<Color> _attractColors = List.filled(81, Colors.grey);
  Timer? _attractTimer;

  // --- Masques de surbrillances colorées (Indices C) ---
  List<int> _highlightRed = List.filled(81, 0);
  List<int> _highlightGreen = List.filled(81, 0);
  List<int> _highlightCyan = List.filled(81, 0);
  List<int> _highlightMagenta = List.filled(81, 0);

  // Mémorisation des zones clôturées
  final Set<int> _completedRows = {};
  final Set<int> _completedCols = {};
  final Set<int> _completedBoxes = {};

  // Animation Mosaïque de victoire de zone
  Set<int> _animatingIndices = {};
  Color _animColor = Colors.lightGreenAccent;
  Timer? _animTimer;

  // --- Configuration de l'interface & Number Scanning ---
  int _selectedIndex = -1;
  int _scannedDigit = 0;
  bool _showNotes = false;
  bool _isLoading = false;
  String _hintDescription = "Veuillez démarrer une partie via le menu (≡) pour jouer.";
  SudokuPalette _currentPalette = SudokuPalette.classic;

  @override
  void initState() {
    super.initState();
    AudioManager.initBackgroundMusic();
    _startAttractMode();
  }

  @override
  void dispose() {
    _attractTimer?.cancel();
    _animTimer?.cancel();
    AudioManager.dispose();
    super.dispose();
  }

  void _startAttractMode() {
    _attractTimer?.cancel();
    _attractTimer = Timer.periodic(const Duration(milliseconds: 50), (timer) {
      if (!_isPlaying && mounted) {
        setState(() {
          int r = Random().nextInt(9);
          int c = Random().nextInt(9);
          int idx = r * 9 + c;

          if (Random().nextInt(10) > 2) {
            _attractGrid[idx] = Random().nextInt(9) + 1;
            _attractColors[idx] = Color.fromRGBO(
              160 + Random().nextInt(95),
              160 + Random().nextInt(95),
              160 + Random().nextInt(95),
              1.0,
            );
          } else {
            _attractGrid[idx] = 0;
          }
        });
      }
    });
  }

  void _refreshCandidatesAndHighlights() {
    final data = SudokuFFIService.fetchCandidatesAndHighlights();
    setState(() {
      _candidatesGrid = data['candidates']!;
      _highlightRed = data['red']!;
      _highlightGreen = data['green']!;
      _highlightCyan = data['cyan']!;
      _highlightMagenta = data['magenta']!;
    });
  }

  void _loadGameData(int level) {
    _attractTimer?.cancel();

    setState(() {
      _isLoading = true;
      _isPlaying = true;
      _selectedIndex = -1;
      _scannedDigit = 0;
      _isEraserMode = false;
      _eliminatedCandidates = List.filled(81, 0);
      _currentLevel = level;
      _showNotes = false;
      _animatingIndices.clear();
      _completedRows.clear();
      _completedCols.clear();
      _completedBoxes.clear();
      _hintDescription = "Cliquez sur « Indice » pour analyser la grille.";
    });

    try {
      final gameData = SudokuFFIService.getGameData(level);
      _refreshCandidatesAndHighlights();

      setState(() {
        _initialGrid = gameData['initial']!;
        _playerGrid = gameData['player']!;
        _isLoading = false;
      });
    } catch (e) {
      setState(() => _isLoading = false);
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text("Erreur d'initialisation C : $e")),
      );
    }
  }

  void _requestHint() {
    if (!_isPlaying) return;
    try {
      final desc = SudokuFFIService.provideHint();
      _refreshCandidatesAndHighlights();
      AudioManager.playSfx('hint.wav');

      setState(() {
        _hintDescription = desc;
      });
    } catch (e) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text("Erreur demande d'indice : $e")),
      );
    }
  }

  bool _checkAndAnimateNewZones(int row, int col, int zoneMask) {
    bool isNewZoneCompleted = false;

    if ((zoneMask & 1) != 0 && !_completedRows.contains(row)) {
      _completedRows.add(row);
      isNewZoneCompleted = true;
    }
    if ((zoneMask & 2) != 0 && !_completedCols.contains(col)) {
      _completedCols.add(col);
      isNewZoneCompleted = true;
    }
    final int box = (row ~/ 3) * 3 + (col ~/ 3);
    if ((zoneMask & 4) != 0 && !_completedBoxes.contains(box)) {
      _completedBoxes.add(box);
      isNewZoneCompleted = true;
    }

    if (isNewZoneCompleted) {
      _triggerZoneAnimation(row, col, zoneMask);
    }
    return isNewZoneCompleted;
  }

  void _triggerZoneAnimation(int row, int col, int zoneMask) {
    final Set<int> indicesToAnimate = {};

    if ((zoneMask & 1) != 0) {
      for (int c = 0; c < 9; c++) indicesToAnimate.add(row * 9 + c);
    }
    if ((zoneMask & 2) != 0) {
      for (int r = 0; r < 9; r++) indicesToAnimate.add(r * 9 + col);
    }
    if ((zoneMask & 4) != 0) {
      int startR = (row ~/ 3) * 3;
      int startC = (col ~/ 3) * 3;
      for (int r = startR; r < startR + 3; r++) {
        for (int c = startC; c < startC + 3; c++) {
          indicesToAnimate.add(r * 9 + c);
        }
      }
    }

    if (indicesToAnimate.isNotEmpty) {
      AudioManager.playSfx('fanfare.wav');

      int ticks = 0;
      _animTimer?.cancel();

      _animTimer = Timer.periodic(const Duration(milliseconds: 100), (timer) {
        ticks++;
        setState(() {
          _animatingIndices = indicesToAnimate;
          _animColor = Colors.primaries[Random().nextInt(Colors.primaries.length)].shade200;
        });

        if (ticks >= 10) {
          timer.cancel();
          setState(() {
            _animatingIndices.clear();
          });
        }
      });
    }
  }

  void _applyHint() {
    if (!_isPlaying) return;
    try {
      final result = SudokuFFIService.applyCurrentHint();
      final bool success = result['success'];
      final List<int> fetchedPlayer = result['playerGrid'];

      if (success) {
        bool hasNewZone = false;
        bool hasPlacedNumber = false;

        for (int i = 0; i < 81; i++) {
          if (_playerGrid[i] == 0 && fetchedPlayer[i] != 0) {
            hasPlacedNumber = true;
            final int row = i ~/ 9;
            final int col = i % 9;
            final int zoneMask = SudokuFFIService.checkCompletedZone(row, col);
            if (zoneMask > 0) {
              if (_checkAndAnimateNewZones(row, col, zoneMask)) {
                hasNewZone = true;
              }
            }
          }
        }

        _refreshCandidatesAndHighlights();

        if (!hasNewZone && hasPlacedNumber) {
          AudioManager.playSfx('place.wav');
        }

        setState(() {
          _playerGrid = fetchedPlayer;
          _hintDescription = "Indice appliqué avec succès !";
        });
      } else {
        ScaffoldMessenger.of(context).showSnackBar(
          const SnackBar(content: Text("Aucun indice à appliquer actuellement.")),
        );
      }
    } catch (e) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text("Erreur application indice : $e")),
      );
    }
  }

  void _playNumber(int number) {
    if (!_isPlaying || _selectedIndex == -1) return;
    if (_initialGrid[_selectedIndex] != 0) return;

    final int row = _selectedIndex ~/ 9;
    final int col = _selectedIndex % 9;

    try {
      final result = SudokuFFIService.makeMove(row, col, number);
      final bool success = result['success'];
      final List<int> fetchedPlayer = result['playerGrid'];

      if (success) {
        _refreshCandidatesAndHighlights();

        setState(() {
          _playerGrid = fetchedPlayer;
          _eliminatedCandidates[_selectedIndex] = 0;
        });

        if (number == 0) {
          AudioManager.playSfx('erase.wav');
        } else {
          final int zoneMask = SudokuFFIService.checkCompletedZone(row, col);
          bool hasNewZone = false;
          if (zoneMask > 0) {
            hasNewZone = _checkAndAnimateNewZones(row, col, zoneMask);
          }

          if (!hasNewZone) {
            AudioManager.playSfx('place.wav');
          }
        }
      } else {
        AudioManager.playSfx('error.wav');
        ScaffoldMessenger.of(context).clearSnackBars();
        ScaffoldMessenger.of(context).showSnackBar(
          const SnackBar(
            content: Text('Coup invalide selon le moteur C !'),
            backgroundColor: Colors.redAccent,
            duration: Duration(seconds: 2),
          ),
        );
      }
    } catch (e) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text("Erreur d'exécution C : $e")),
      );
    }
  }

  void _toggleEliminateCandidate(int number) {
    if (_selectedIndex == -1) return;
    AudioManager.playSfx('erase.wav');
    setState(() {
      _eliminatedCandidates[_selectedIndex] ^= (1 << number);
    });
  }

  Border _buildCellBorder(int index, bool isSelected) {
    if (isSelected && _isPlaying) {
      return Border.all(color: _currentPalette.borderSelected, width: 2.5);
    }

    final int row = index ~/ 9;
    final int col = index % 9;

    return Border(
      top: BorderSide(
        color: (row % 3 == 0) ? _currentPalette.borderThick : _currentPalette.borderThin,
        width: (row % 3 == 0) ? 2.0 : 0.5,
      ),
      left: BorderSide(
        color: (col % 3 == 0) ? _currentPalette.borderThick : _currentPalette.borderThin,
        width: (col % 3 == 0) ? 2.0 : 0.5,
      ),
      right: BorderSide(
        color: (col == 8) ? _currentPalette.borderThick : _currentPalette.borderThin,
        width: (col == 8) ? 2.0 : 0.5,
      ),
      bottom: BorderSide(
        color: (row == 8) ? _currentPalette.borderThick : _currentPalette.borderThin,
        width: (row == 8) ? 2.0 : 0.5,
      ),
    );
  }

  Widget _buildCandidatesGrid(int index) {
    final mask = _candidatesGrid[index];
    final red = _highlightRed[index];
    final green = _highlightGreen[index];
    final cyan = _highlightCyan[index];
    final magenta = _highlightMagenta[index];
    final eliminatedMask = _eliminatedCandidates[index];

    return GridView.builder(
      physics: const NeverScrollableScrollPhysics(),
      itemCount: 9,
      gridDelegate: const SliverGridDelegateWithFixedCrossAxisCount(
        crossAxisCount: 3,
        childAspectRatio: 1.0,
      ),
      itemBuilder: (context, i) {
        final digit = i + 1;
        final bit = (1 << digit);
        final hasCandidate = (mask & bit) != 0;
        final isScannedCandidate = _scannedDigit != 0 && digit == _scannedDigit;
        final isEliminated = (eliminatedMask & bit) != 0;

        Color textColor = _currentPalette.candDefault;
        if (hasCandidate) {
          if (isScannedCandidate) {
            textColor = Colors.white;
          } else if ((red & bit) != 0) {
            textColor = _currentPalette.candRed;
          } else if ((green & bit) != 0) {
            textColor = _currentPalette.candGreen;
          } else if ((cyan & bit) != 0) {
            textColor = _currentPalette.candCyan;
          } else if ((magenta & bit) != 0) {
            textColor = _currentPalette.candMagenta;
          }
        }

        return Container(
          decoration: (hasCandidate && isScannedCandidate)
              ? BoxDecoration(
                  color: _currentPalette.scanCandidate,
                  borderRadius: BorderRadius.circular(2),
                )
              : null,
          child: Stack(
            alignment: Alignment.center,
            children: [
              Center(
                child: Text(
                  hasCandidate ? '$digit' : '',
                  style: TextStyle(
                    fontSize: 10,
                    color: isEliminated ? Colors.red.shade300 : textColor,
                    fontWeight: (isScannedCandidate || textColor != _currentPalette.candDefault)
                        ? FontWeight.bold
                        : FontWeight.normal,
                  ),
                ),
              ),
              if (hasCandidate && isEliminated)
                Positioned.fill(
                  child: CustomPaint(
                    painter: StrikePainter(
                      color: Colors.red.shade700,
                      strokeWidth: 1.2,
                    ),
                  ),
                ),
            ],
          ),
        );
      },
    );
  }

  @override
  Widget build(BuildContext context) {
    final bool isCellSelected = _selectedIndex != -1 && _isPlaying;
    final bool isInitialCell = isCellSelected && _initialGrid[_selectedIndex] != 0;
    final bool isFilledByPlayer = isCellSelected &&
        _playerGrid[_selectedIndex] != 0 &&
        _initialGrid[_selectedIndex] == 0;
    final bool canEditCurrentCell = isCellSelected && !isInitialCell;

    final int selectedCandidateMask =
        (canEditCurrentCell && _playerGrid[_selectedIndex] == 0)
            ? _candidatesGrid[_selectedIndex]
            : 0;

    final int selectedEliminatedMask =
        (canEditCurrentCell && _playerGrid[_selectedIndex] == 0)
            ? _eliminatedCandidates[_selectedIndex]
            : 0;

    // --- CALCUL DES ZONES D'INFLUENCE DU NUMBER SCANNING ---
    final Set<int> scannedRows = {};
    final Set<int> scannedCols = {};
    final Set<int> scannedBoxes = {};

    if (_scannedDigit != 0) {
      for (int i = 0; i < 81; i++) {
        if (_playerGrid[i] == _scannedDigit) {
          scannedRows.add(i ~/ 9);
          scannedCols.add(i % 9);
          scannedBoxes.add(((i ~/ 9) ~/ 3) * 3 + ((i % 9) ~/ 3));
        }
      }
    }

    return Scaffold(
      backgroundColor: _currentPalette.background,
      appBar: AppBar(
        title: Text(_isPlaying
            ? 'Sudoku Engine — Niveau $_currentLevel'
            : 'Sudoku Engine — Aucune partie en cours'),
        backgroundColor: _currentPalette.appBarBackground,
        foregroundColor: Colors.white,
        actions: [
          MenuAnchor(
            builder: (context, controller, child) {
              return IconButton(
                icon: const Icon(Icons.menu),
                onPressed: () {
                  AudioManager.playSfx('click.wav');
                  if (controller.isOpen) {
                    controller.close();
                  } else {
                    controller.open();
                  }
                },
              );
            },
            menuChildren: [
              SubmenuButton(
                menuChildren: List.generate(5, (i) {
                  final level = i + 1;
                  return MenuItemButton(
                    onPressed: () {
                      AudioManager.playSfx('click.wav');
                      _loadGameData(level);
                    },
                    child: Text('Niveau $level'),
                  );
                }),
                child: const Text('Démarrer une partie'),
              ),
              MenuItemButton(
                onPressed: _isPlaying
                    ? () {
                        AudioManager.playSfx('click.wav');
                        setState(() {
                          _showNotes = !_showNotes;
                        });
                      }
                    : null,
                child: Row(
                  children: [
                    Icon(
                      _showNotes ? Icons.edit_note_outlined : Icons.edit_note,
                      size: 18,
                      color: _currentPalette.buttonPrimary,
                    ),
                    const SizedBox(width: 8),
                    Text(_showNotes ? 'Masquer notes' : 'Afficher notes'),
                  ],
                ),
              ),
              SubmenuButton(
                menuChildren: List.generate(SudokuPalette.all.length, (i) {
                  final palette = SudokuPalette.all[i];
                  return MenuItemButton(
                    onPressed: () {
                      AudioManager.playSfx('click.wav');
                      setState(() {
                        _currentPalette = palette;
                      });
                    },
                    child: Row(
                      children: [
                        Icon(
                          _currentPalette.name == palette.name
                              ? Icons.radio_button_checked
                              : Icons.radio_button_unchecked,
                          size: 16,
                          color: _currentPalette.buttonPrimary,
                        ),
                        const SizedBox(width: 8),
                        Text(palette.name),
                      ],
                    ),
                  );
                }),
                child: const Text('Thème visuel'),
              ),
              const Divider(),
              MenuItemButton(
                onPressed: () {
                  AudioManager.playSfx('click.wav');
                  exit(0);
                },
                child: const Row(
                  children: [
                    Icon(Icons.power_settings_new, color: Colors.red, size: 18),
                    SizedBox(width: 8),
                    Text('Quitter', style: TextStyle(color: Colors.red)),
                  ],
                ),
              ),
            ],
          ),
        ],
      ),
      body: _isLoading
          ? const Center(child: CircularProgressIndicator())
          : Padding(
              padding: const EdgeInsets.all(16.0),
              child: Row(
                children: [
                  // --- PARTIE GAUCHE : GRILLE SUDOKU (~70%) ---
                  Expanded(
                    flex: 7,
                    child: Center(
                      child: AspectRatio(
                        aspectRatio: 1.0,
                        child: GridView.builder(
                          physics: const NeverScrollableScrollPhysics(),
                          itemCount: 81,
                          gridDelegate: const SliverGridDelegateWithFixedCrossAxisCount(
                            crossAxisCount: 9,
                            childAspectRatio: 1.0,
                          ),
                          itemBuilder: (context, index) {
                            if (!_isPlaying) {
                              final val = _attractGrid[index];
                              return Container(
                                margin: const EdgeInsets.all(1.0),
                                color: _attractColors[index],
                                child: val != 0
                                    ? Center(
                                        child: Text(
                                          '$val',
                                          style: const TextStyle(
                                            fontSize: 22,
                                            fontWeight: FontWeight.bold,
                                            color: Colors.black54,
                                          ),
                                        ),
                                      )
                                    : null,
                              );
                            }

                            final isInitial = _initialGrid[index] != 0;
                            final val = _playerGrid[index];
                            final isSelected = index == _selectedIndex;
                            final isAnimating = _animatingIndices.contains(index);

                            final hasHighlight = _highlightRed[index] != 0 ||
                                _highlightGreen[index] != 0 ||
                                _highlightCyan[index] != 0 ||
                                _highlightMagenta[index] != 0;

                            final int r = index ~/ 9;
                            final int c = index % 9;
                            final int b = (r ~/ 3) * 3 + (c ~/ 3);

                            final bool inScanZone = _scannedDigit != 0 &&
                                (scannedRows.contains(r) ||
                                    scannedCols.contains(c) ||
                                    scannedBoxes.contains(b));

                            Color cellBgColor;
                            if (isAnimating) {
                              cellBgColor = _animColor;
                            } else if (_scannedDigit != 0 && val == _scannedDigit) {
                              cellBgColor = _currentPalette.scanBright;
                            } else if (inScanZone && val != 0) {
                              cellBgColor = _currentPalette.scanMedium;
                            } else if (inScanZone && val == 0) {
                              cellBgColor = _currentPalette.scanLight;
                            } else if (isSelected) {
                              cellBgColor = _currentPalette.cellSelectedBg;
                            } else if (hasHighlight) {
                              cellBgColor = _currentPalette.cellHintBg;
                            } else if (isInitial) {
                              cellBgColor = _currentPalette.cellInitialBg;
                            } else {
                              cellBgColor = _currentPalette.cellPlayerBg;
                            }

                            return GestureDetector(
                              onTap: () {
                                AudioManager.playSfx('click.wav');
                                setState(() {
                                  _selectedIndex = index;
                                  if (val != 0) {
                                    _scannedDigit = (_scannedDigit == val) ? 0 : val;
                                  } else {
                                    _scannedDigit = 0;
                                  }
                                });
                              },
                              child: AnimatedContainer(
                                duration: const Duration(milliseconds: 100),
                                decoration: BoxDecoration(
                                  color: cellBgColor,
                                  border: _buildCellBorder(index, isSelected),
                                ),
                                child: val != 0
                                    ? Center(
                                        child: Text(
                                          '$val',
                                          style: TextStyle(
                                            fontSize: 24,
                                            fontWeight: isInitial
                                                ? FontWeight.bold
                                                : FontWeight.normal,
                                            color: (_scannedDigit != 0 && val == _scannedDigit)
                                                ? Colors.white
                                                : (isInitial
                                                    ? _currentPalette.textInitial
                                                    : _currentPalette.textPlayer),
                                          ),
                                        ),
                                      )
                                    : (_showNotes
                                        ? _buildCandidatesGrid(index)
                                        : const SizedBox.shrink()),
                              ),
                            );
                          },
                        ),
                      ),
                    ),
                  ),

                  const SizedBox(width: 20),

                  // --- PARTIE DROITE : PANNEAU DE COMMANDES (~30%) ---
                  Expanded(
                    flex: 3,
                    child: SingleChildScrollView(
                      child: Column(
                        crossAxisAlignment: CrossAxisAlignment.stretch,
                        children: [
                          // 1. Boutons d'Action (Indice & Appliquer)
                          Row(
                            children: [
                              Expanded(
                                child: ElevatedButton.icon(
                                  style: ElevatedButton.styleFrom(
                                    backgroundColor: _currentPalette.buttonPrimary,
                                    foregroundColor: Colors.white,
                                    padding: const EdgeInsets.symmetric(vertical: 12),
                                    shape: RoundedRectangleBorder(
                                      borderRadius: BorderRadius.circular(6),
                                    ),
                                  ),
                                  onPressed: _isPlaying ? _requestHint : null,
                                  icon: const Icon(Icons.lightbulb, size: 18),
                                  label: const Text('Indice'),
                                ),
                              ),
                              const SizedBox(width: 8),
                              Expanded(
                                child: ElevatedButton.icon(
                                  style: ElevatedButton.styleFrom(
                                    backgroundColor: Colors.teal.shade700,
                                    foregroundColor: Colors.white,
                                    padding: const EdgeInsets.symmetric(vertical: 12),
                                    shape: RoundedRectangleBorder(
                                      borderRadius: BorderRadius.circular(6),
                                    ),
                                  ),
                                  onPressed: _isPlaying ? _applyHint : null,
                                  icon: const Icon(Icons.check_circle, size: 18),
                                  label: const Text('Appliquer'),
                                ),
                              ),
                            ],
                          ),

                          const SizedBox(height: 16),

                          // 2. Encadré "ANALYSE & THÉORIE"
                          Container(
                            padding: const EdgeInsets.all(12.0),
                            decoration: BoxDecoration(
                              color: _currentPalette.cardBackground,
                              border: Border.all(color: _currentPalette.cardBorder),
                              borderRadius: BorderRadius.circular(8.0),
                              boxShadow: [
                                BoxShadow(
                                  color: Colors.black.withOpacity(0.04),
                                  blurRadius: 4,
                                  offset: const Offset(0, 2),
                                ),
                              ],
                            ),
                            child: Column(
                              crossAxisAlignment: CrossAxisAlignment.start,
                              children: [
                                Text(
                                  'ANALYSE & THÉORIE',
                                  style: TextStyle(
                                    fontSize: 13,
                                    fontWeight: FontWeight.bold,
                                    color: _currentPalette.buttonPrimary,
                                  ),
                                ),
                                const Divider(),
                                Text(
                                  _hintDescription,
                                  style: TextStyle(
                                    fontSize: 12,
                                    height: 1.4,
                                    color: _currentPalette.textInitial,
                                  ),
                                ),
                              ],
                            ),
                          ),

                          const SizedBox(height: 16),

                          // 3. Pavé Numérique de Saisie & Gomme
                          Container(
                            padding: const EdgeInsets.all(12.0),
                            decoration: BoxDecoration(
                              color: _currentPalette.cardBackground,
                              border: Border.all(color: _currentPalette.cardBorder),
                              borderRadius: BorderRadius.circular(8.0),
                            ),
                            child: Column(
                              children: [
                                Text(
                                  _isEraserMode
                                      ? 'EFFACEMENT DE CANDIDATS'
                                      : 'SAISIE MANUELLE',
                                  style: TextStyle(
                                    fontSize: 12,
                                    fontWeight: FontWeight.bold,
                                    color: _isEraserMode
                                        ? _currentPalette.eraserActive
                                        : _currentPalette.candDefault,
                                  ),
                                ),
                                const SizedBox(height: 10),

                                // Matrice de boutons 1 à 9
                                GridView.builder(
                                  shrinkWrap: true,
                                  physics: const NeverScrollableScrollPhysics(),
                                  itemCount: 9,
                                  gridDelegate: const SliverGridDelegateWithFixedCrossAxisCount(
                                    crossAxisCount: 3,
                                    childAspectRatio: 1.6,
                                    crossAxisSpacing: 8,
                                    mainAxisSpacing: 8,
                                  ),
                                  itemBuilder: (context, i) {
                                    final number = i + 1;
                                    final bit = (1 << number);

                                    final bool isCandidatePresent = (selectedCandidateMask & bit) != 0;
                                    final bool isCandidateEliminated = (selectedEliminatedMask & bit) != 0;

                                    // Condition d'activation :
                                    // Mode Gomme : cliquable si le candidat est présent en C
                                    // Mode Normal : cliquable uniquement s'il est présent ET non biffé
                                    final bool isEnabled = canEditCurrentCell &&
                                        isCandidatePresent &&
                                        (_isEraserMode || !isCandidateEliminated);

                                    return Stack(
                                      children: [
                                        Positioned.fill(
                                          child: ElevatedButton(
                                            style: ElevatedButton.styleFrom(
                                              padding: EdgeInsets.zero,
                                              backgroundColor: isCandidatePresent
                                                  ? _currentPalette.buttonPrimary
                                                  : null,
                                              foregroundColor: Colors.white,
                                              shape: RoundedRectangleBorder(
                                                borderRadius: BorderRadius.circular(6),
                                              ),
                                            ),
                                            onPressed: isEnabled
                                                ? () {
                                                    if (_isEraserMode) {
                                                      _toggleEliminateCandidate(number);
                                                    } else {
                                                      _playNumber(number);
                                                    }
                                                  }
                                                : null,
                                            child: Text(
                                              '$number',
                                              style: const TextStyle(
                                                fontSize: 18,
                                                fontWeight: FontWeight.bold,
                                              ),
                                            ),
                                          ),
                                        ),
                                        // Rature rouge diagonale sur le pavé numérique
                                        if (isCandidatePresent && isCandidateEliminated)
                                          Positioned.fill(
                                            child: IgnorePointer(
                                              child: CustomPaint(
                                                painter: StrikePainter(
                                                  color: Colors.red.shade700,
                                                  strokeWidth: 2.5,
                                                ),
                                              ),
                                            ),
                                          ),
                                      ],
                                    );
                                  },
                                ),
                                const SizedBox(height: 10),

                                // Ligne inférieure : Gomme + Effacer
                                Row(
                                  children: [
                                    Expanded(
                                      child: SizedBox(
                                        height: 38,
                                        child: ElevatedButton.icon(
                                          style: ElevatedButton.styleFrom(
                                            backgroundColor: _isEraserMode
                                                ? _currentPalette.eraserActive
                                                : Colors.grey.shade300,
                                            foregroundColor: _isEraserMode
                                                ? Colors.white
                                                : Colors.black87,
                                            padding: const EdgeInsets.symmetric(horizontal: 4),
                                            shape: RoundedRectangleBorder(
                                              borderRadius: BorderRadius.circular(6),
                                            ),
                                          ),
                                          onPressed: _isPlaying
                                              ? () {
                                                  AudioManager.playSfx('click.wav');
                                                  setState(() {
                                                    _isEraserMode = !_isEraserMode;
                                                    if (_isEraserMode) _showNotes = true;
                                                  });
                                                }
                                              : null,
                                          icon: const Icon(Icons.cleaning_services, size: 16),
                                          label: const Text(
                                            'Gomme',
                                            maxLines: 1,
                                            overflow: TextOverflow.ellipsis,
                                            style: TextStyle(fontSize: 13),
                                          ),
                                        ),
                                      ),
                                    ),
                                    const SizedBox(width: 8),
                                    Expanded(
                                      child: SizedBox(
                                        height: 38,
                                        child: OutlinedButton.icon(
                                          style: OutlinedButton.styleFrom(
                                            foregroundColor: _currentPalette.buttonDangerText,
                                            shape: RoundedRectangleBorder(
                                              borderRadius: BorderRadius.circular(6),
                                            ),
                                          ),
                                          onPressed: (canEditCurrentCell && isFilledByPlayer)
                                              ? () => _playNumber(0)
                                              : null,
                                          icon: const Icon(Icons.backspace_outlined, size: 16),
                                          label: const Text('Effacer'),
                                        ),
                                      ),
                                    ),
                                  ],
                                ),
                              ],
                            ),
                          ),
                        ],
                      ),
                    ),
                  ),
                ],
              ),
            ),
    );
  }
}