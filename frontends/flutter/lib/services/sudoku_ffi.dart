import 'dart:ffi';
import 'package:ffi/ffi.dart';

// Signatures FFI natives (C) et Dart
typedef CGetGameDataC = Void Function(
    Int32 level, Pointer<Int32> outInitial, Pointer<Int32> outPlayer);
typedef CGetGameDataDart = void Function(
    int level, Pointer<Int32> outInitial, Pointer<Int32> outPlayer);

typedef CMakeMoveC = Int32 Function(
    Int32 row, Int32 col, Int32 value, Pointer<Int32> outPlayer);
typedef CMakeMoveDart = int Function(
    int row, int col, int value, Pointer<Int32> outPlayer);

typedef CCheckZoneC = Int32 Function(Int32 row, Int32 col);
typedef CCheckZoneDart = int Function(int row, int col);

typedef CGetCandidatesC = Void Function(Pointer<Int32> outCandidates);
typedef CGetCandidatesDart = void Function(Pointer<Int32> outCandidates);

typedef CProvideHintC = Void Function(Pointer<Utf8> outDesc, Int32 maxLen);
typedef CProvideHintDart = void Function(Pointer<Utf8> outDesc, int maxLen);

typedef CApplyCurrentHintC = Int32 Function(Pointer<Int32> outPlayer);
typedef CApplyCurrentHintDart = int Function(Pointer<Int32> outPlayer);

typedef CGetHighlightsC = Void Function(
    Pointer<Int32> outRed, Pointer<Int32> outGreen, Pointer<Int32> outCyan, Pointer<Int32> outMagenta);
typedef CGetHighlightsDart = void Function(
    Pointer<Int32> outRed, Pointer<Int32> outGreen, Pointer<Int32> outCyan, Pointer<Int32> outMagenta);

/// Service encapsulant tous les appels FFI vers la bibliothèque C native.
class SudokuFFIService {
  static DynamicLibrary get _nativeLib => DynamicLibrary.process();

  /// Récupère la grille initiale et la grille joueur pour un niveau donné.
  static Map<String, List<int>> getGameData(int level) {
    final getGameData = _nativeLib
        .lookup<NativeFunction<CGetGameDataC>>('c_get_game_data')
        .asFunction<CGetGameDataDart>();

    final ptrInitial = calloc<Int32>(81);
    final ptrPlayer = calloc<Int32>(81);

    getGameData(level, ptrInitial, ptrPlayer);

    final initial = List.generate(81, (i) => ptrInitial[i]);
    final player = List.generate(81, (i) => ptrPlayer[i]);

    calloc.free(ptrInitial);
    calloc.free(ptrPlayer);

    return {'initial': initial, 'player': player};
  }

  /// Applique un coup via le moteur C et retourne le résultat.
  static Map<String, dynamic> makeMove(int row, int col, int value) {
    final makeMove = _nativeLib
        .lookup<NativeFunction<CMakeMoveC>>('c_make_move')
        .asFunction<CMakeMoveDart>();

    final ptrPlayer = calloc<Int32>(81);
    final int success = makeMove(row, col, value, ptrPlayer);

    final playerGrid = List.generate(81, (i) => ptrPlayer[i]);
    calloc.free(ptrPlayer);

    return {'success': success == 1, 'playerGrid': playerGrid};
  }

  /// Vérifie si le dernier coup complète une ligne, colonne ou bloc.
  static int checkCompletedZone(int row, int col) {
    final checkZone = _nativeLib
        .lookup<NativeFunction<CCheckZoneC>>('c_check_completed_zone')
        .asFunction<CCheckZoneDart>();
    return checkZone(row, col);
  }

  /// Interroge le moteur C pour obtenir une analyse stratégique (Indice).
  static String provideHint() {
    final provideHint = _nativeLib
        .lookup<NativeFunction<CProvideHintC>>('c_provide_hint')
        .asFunction<CProvideHintDart>();

    final ptrDesc = calloc<Int8>(512);
    provideHint(ptrDesc.cast(), 512);

    final String desc = ptrDesc.cast<Utf8>().toDartString();
    calloc.free(ptrDesc);

    return desc;
  }

  /// Applique automatiquement l'indice courant calculé en C.
  static Map<String, dynamic> applyCurrentHint() {
    final applyHint = _nativeLib
        .lookup<NativeFunction<CApplyCurrentHintC>>('c_apply_current_hint')
        .asFunction<CApplyCurrentHintDart>();

    final ptrPlayer = calloc<Int32>(81);
    final int success = applyHint(ptrPlayer);

    final playerGrid = List.generate(81, (i) => ptrPlayer[i]);
    calloc.free(ptrPlayer);

    return {'success': success == 1, 'playerGrid': playerGrid};
  }

  /// Récupère l'ensemble des candidats et des masques de couleurs d'analyse.
  static Map<String, List<int>> fetchCandidatesAndHighlights() {
    final getCandidates = _nativeLib
        .lookup<NativeFunction<CGetCandidatesC>>('c_get_candidates')
        .asFunction<CGetCandidatesDart>();

    final ptrCandidates = calloc<Int32>(81);
    getCandidates(ptrCandidates);
    final candidates = List.generate(81, (i) => ptrCandidates[i]);
    calloc.free(ptrCandidates);

    final getHighlights = _nativeLib
        .lookup<NativeFunction<CGetHighlightsC>>('c_get_highlights')
        .asFunction<CGetHighlightsDart>();

    final ptrRed = calloc<Int32>(81);
    final ptrGreen = calloc<Int32>(81);
    final ptrCyan = calloc<Int32>(81);
    final ptrMagenta = calloc<Int32>(81);

    getHighlights(ptrRed, ptrGreen, ptrCyan, ptrMagenta);

    final red = List.generate(81, (i) => ptrRed[i]);
    final green = List.generate(81, (i) => ptrGreen[i]);
    final cyan = List.generate(81, (i) => ptrCyan[i]);
    final magenta = List.generate(81, (i) => ptrMagenta[i]);

    calloc.free(ptrRed);
    calloc.free(ptrGreen);
    calloc.free(ptrCyan);
    calloc.free(ptrMagenta);

    return {
      'candidates': candidates,
      'red': red,
      'green': green,
      'cyan': cyan,
      'magenta': magenta,
    };
  }
}