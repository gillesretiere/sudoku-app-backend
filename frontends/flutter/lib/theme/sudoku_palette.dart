import 'package:flutter/material.dart';

/// Modèle de palette de couleurs avec support du Number Scanning et de la Gomme.
class SudokuPalette {
  final String name;
  final Color background;
  final Color cardBackground;
  final Color cardBorder;
  
  // Grille et cases
  final Color cellInitialBg;
  final Color cellPlayerBg;
  final Color cellSelectedBg;
  final Color cellHintBg;
  
  // Couleurs du Number Scanning
  final Color scanBright;    // Teinte vive (chiffre trouvé)
  final Color scanMedium;    // Teinte moyenne (autres chiffres en zone)
  final Color scanLight;     // Teinte faible (cases vides en zone)
  final Color scanCandidate; // Fond mini-badge pour candidat AutoNote
  
  // Bordures de la grille
  final Color borderThin;
  final Color borderThick;
  final Color borderSelected;
  
  // Textes de chiffres
  final Color textInitial;
  final Color textPlayer;
  
  // Candidats d'indices (Couleurs stratégiques C)
  final Color candDefault;
  final Color candRed;
  final Color candGreen;
  final Color candCyan;
  final Color candMagenta;

  // En-tête, boutons et Gomme
  final Color appBarBackground;
  final Color buttonPrimary;
  final Color buttonDangerText;
  final Color eraserActive;

  const SudokuPalette({
    required this.name,
    required this.background,
    required this.cardBackground,
    required this.cardBorder,
    required this.cellInitialBg,
    required this.cellPlayerBg,
    required this.cellSelectedBg,
    required this.cellHintBg,
    required this.scanBright,
    required this.scanMedium,
    required this.scanLight,
    required this.scanCandidate,
    required this.borderThin,
    required this.borderThick,
    required this.borderSelected,
    required this.textInitial,
    required this.textPlayer,
    required this.candDefault,
    required this.candRed,
    required this.candGreen,
    required this.candCyan,
    required this.candMagenta,
    required this.appBarBackground,
    required this.buttonPrimary,
    required this.buttonDangerText,
    required this.eraserActive,
  });

  // ==========================================================================
  // PALETTES PRÉCONFIGURÉES
  // ==========================================================================

  /// Thème 1 : Classique (Lumineux)
  static const SudokuPalette classic = SudokuPalette(
    name: 'Classique',
    background: Color(0xFFF1F5F9),
    cardBackground: Colors.white,
    cardBorder: Color(0xFFCBD5E1),
    cellInitialBg: Color(0xFFE2E8F0),
    cellPlayerBg: Colors.white,
    cellSelectedBg: Color(0xFFFEF08A),
    cellHintBg: Color(0xFFFED7AA),
    scanBright: Color(0xFFFF8200),     // Orange vif
    scanMedium: Color(0xFFCFE8EF),     // Bleu/Cyan moyen
    scanLight: Color(0xFFF5E6D3),      // Beige/Pêche doux
    scanCandidate: Color(0xFFFF8200),  // Orange vif pour mini-candidat
    borderThin: Color(0xFFCBD5E1),
    borderThick: Color(0xFF1E293B),
    borderSelected: Color(0xFFEAB308),
    textInitial: Color(0xFF0F172A),
    textPlayer: Color(0xFF1D4ED8),
    candDefault: Color(0xFF64748B),
    candRed: Color(0xFFDC2626),
    candGreen: Color(0xFF16A34A),
    candCyan: Color(0xFF0891B2),
    candMagenta: Color(0xFFC026D3),
    appBarBackground: Color(0xFF1E1B4B),
    buttonPrimary: Color(0xFF4338CA),
    buttonDangerText: Color(0xFFB91C1C),
    eraserActive: Color(0xFFDC2626),  // Rouge vif pour le mode Gomme
  );

  /// Thème 2 : Sombre (Mode Nuit)
  static const SudokuPalette dark = SudokuPalette(
    name: 'Sombre',
    background: Color(0xFF0F172A),
    cardBackground: Color(0xFF1E293B),
    cardBorder: Color(0xFF334155),
    cellInitialBg: Color(0xFF334155),
    cellPlayerBg: Color(0xFF1E293B),
    cellSelectedBg: Color(0xFF854D0E),
    cellHintBg: Color(0xFF9A3412),
    scanBright: Color(0xFFEA580C),
    scanMedium: Color(0xFF1E3A8A),
    scanLight: Color(0xFF334155),
    scanCandidate: Color(0xFFEA580C),
    borderThin: Color(0xFF475569),
    borderThick: Color(0xFF94A3B8),
    borderSelected: Color(0xFFFACC15),
    textInitial: Color(0xFFF8FAFC),
    textPlayer: Color(0xFF60A5FA),
    candDefault: Color(0xFF94A3B8),
    candRed: Color(0xFFEF4444),
    candGreen: Color(0xFF22C55E),
    candCyan: Color(0xFF06B6D4),
    candMagenta: Color(0xFFE081F9),
    appBarBackground: Color(0xFF020617),
    buttonPrimary: Color(0xFF2563EB),
    buttonDangerText: Color(0xFFEF4444),
    eraserActive: Color(0xFFEF4444),
  );

  /// Thème 3 : Océan
  static const SudokuPalette ocean = SudokuPalette(
    name: 'Océan',
    background: Color(0xFFECFEFF),
    cardBackground: Colors.white,
    cardBorder: Color(0xFFA5F3FC),
    cellInitialBg: Color(0xFFCFFAFE),
    cellPlayerBg: Colors.white,
    cellSelectedBg: Color(0xFFA5F3FC),
    cellHintBg: Color(0xFFFDE68A),
    scanBright: Color(0xFFF97316),
    scanMedium: Color(0xFFBAE6FD),
    scanLight: Color(0xFFFEF3C7),
    scanCandidate: Color(0xFFF97316),
    borderThin: Color(0xFFA5F3FC),
    borderThick: Color(0xFF0891B2),
    borderSelected: Color(0xFF0284C7),
    textInitial: Color(0xFF164E63),
    textPlayer: Color(0xFF0284C7),
    candDefault: Color(0xFF64748B),
    candRed: Color(0xFFDC2626),
    candGreen: Color(0xFF16A34A),
    candCyan: Color(0xFF0891B2),
    candMagenta: Color(0xFFC026D3),
    appBarBackground: Color(0xFF155E75),
    buttonPrimary: Color(0xFF0891B2),
    buttonDangerText: Color(0xFFB91C1C),
    eraserActive: Color(0xFFDC2626),
  );

  static const List<SudokuPalette> all = [classic, dark, ocean];
}