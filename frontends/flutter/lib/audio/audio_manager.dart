import 'package:audioplayers/audioplayers.dart';
import 'package:flutter/material.dart';

/// Gestionnaire statique de la musique et des effets sonores.
class AudioManager {
  static final AudioPlayer _bgPlayer = AudioPlayer();
  static final AudioPlayer _sfxPlayer = AudioPlayer();

  /// Initialise la musique de fond en boucle continue.
  static Future<void> initBackgroundMusic() async {
    try {
      await _bgPlayer.setReleaseMode(ReleaseMode.loop);
      await _bgPlayer.setVolume(0.3); // Volume à 30%
      await _bgPlayer.play(AssetSource('audio/track.wav'));
    } catch (e) {
      debugPrint("Erreur chargement musique : $e");
    }
  }

  /// Joue un effet sonore ponctuel.
  static Future<void> playSfx(String fileName) async {
    try {
      await _sfxPlayer.stop(); // Stoppe le son en cours si répétition rapide
      await _sfxPlayer.play(AssetSource('audio/$fileName'));
    } catch (e) {
      debugPrint("Erreur lecture SFX $fileName : $e");
    }
  }

  /// Libère les ressources audio à la fermeture.
  static void dispose() {
    _bgPlayer.dispose();
    _sfxPlayer.dispose();
  }
}