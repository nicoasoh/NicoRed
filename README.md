# NicoRed

<p align="center">
  <img src="https://placehold.co/1200x400/101014/9b6cff?text=NicoRed" width="100%" alt="NicoRed Banner">
</p>

<p align="center">
  <a href="https://github.com/nicoasoh">
    <img src="https://img.shields.io/badge/Developer-nicoasoh-8b5cf6?style=for-the-badge&logo=github" alt="Developer">
  </a>
  <img src="https://img.shields.io/badge/Status-Experimental-orange?style=for-the-badge" alt="Status">
  <img src="https://img.shields.io/badge/Platform-macOS-black?style=for-the-badge&logo=apple" alt="macOS">
  <img src="https://img.shields.io/badge/GPU-AMD-red?style=for-the-badge" alt="AMD GPU">
  <img src="https://img.shields.io/badge/Language-C%2B%2B-blue?style=for-the-badge&logo=cplusplus" alt="C++">
</p>

<p align="center">
  <b>Entwicklung. Stabilität. Kontrolle.</b>
</p>

---

## Über NicoRed

NicoRed ist ein Entwicklungsprojekt zur Erforschung und Verbesserung der Grafikstabilität, Kompatibilität und Verarbeitung integrierter AMD-Grafiklösungen unter macOS.

Der Fokus liegt auf der Untersuchung von GPU-Problemen, Rendering-Verhalten, Speicherverwaltung und Synchronisierung. Ziel ist es, Fehler systematisch zu analysieren und langfristig eine stabile und zuverlässige Grafikverarbeitung zu ermöglichen.

Das Projekt befindet sich in aktiver Entwicklung. Funktionen, Kompatibilität und Leistungsmerkmale können sich jederzeit ändern.

---

> [!WARNING]
> NicoRed ist experimentelle Software. Die Verwendung kann zu Grafikfehlern, System-Freezes, Kernel-Abstürzen oder Startproblemen führen. Verwende Entwicklungsbuilds ausschließlich mit einer funktionierenden Sicherung deines Systems und deiner EFI-Konfiguration.

---

## Projektziele

- Verbesserung der GPU-Stabilität
- Untersuchung von Rendering-Fehlern
- Analyse von Grafik-Freezes
- Verbesserung der Speicherverwaltung
- Optimierung der GPU-/CPU-Synchronisierung
- Untersuchung von Metal-Kompatibilität
- Reduzierung von Performance-Einbrüchen
- Verbesserung der Kompatibilität mit Chromium-basierten Anwendungen
- Erweiterung von Debugging- und Logging-Funktionen
- Untersuchung von Fehlererkennung und Wiederherstellung

---

## Grafiksystem

<p align="center">
  <img src="https://placehold.co/1000x350/171717/a78bfa?text=NicoRed+%7C+Graphics+Research" width="100%" alt="NicoRed Grafiksystem">
</p>

NicoRed beschäftigt sich mit verschiedenen Bereichen der Grafikverarbeitung und GPU-Integration.

| Bereich | Entwicklungsfokus |
|---|---|
| GPU-Initialisierung | Zuverlässige Initialisierung |
| Rendering | Fehler und Artefakte untersuchen |
| Framebuffer | Ausgabe und Kompatibilität |
| Speicherverwaltung | GPU-Speicherverhalten analysieren |
| Synchronisierung | CPU-/GPU-Abstimmung |
| Metal | Kompatibilität untersuchen |
| Performance | Engpässe und Overhead analysieren |
| Recovery | Verhalten nach GPU-Fehlern untersuchen |

---

## Hardware

<p align="center">
  <img src="https://placehold.co/1000x300/101014/60a5fa?text=AMD+Integrated+Graphics" width="100%" alt="AMD Integrated Graphics">
</p>

### Entwicklungsziel

| Architektur | Status |
|---|---|
| AMD Picasso | Hauptentwicklungsziel |
| AMD Raven Ridge | In Untersuchung |
| AMD Vega iGPU | In Untersuchung |
| Weitere AMD-GPUs | Nicht bestätigt |

Die tatsächliche Kompatibilität hängt vom Hardwaremodell, der macOS-Version und dem Entwicklungsstand ab.

---

## Kompatibilität

| Technologie | Status |
|---|---|
| macOS | Entwicklungsplattform |
| AMD iGPU | Experimentell |
| GPU-Beschleunigung | In Untersuchung |
| Metal | Untersuchungsbereich |
| Chromium / Chrome | Stabilitätstests |
| Hardwarebeschleunigtes Rendering | Experimentell |
| GPU-Speicherverwaltung | Entwicklungsziel |
| Logging / Debugging | In Entwicklung |

---

## Entwicklungsphilosophie

NicoRed wird schrittweise und modular entwickelt.

Änderungen sollen nachvollziehbar, überprüfbar und möglichst isoliert sein, um Fehler leichter identifizieren zu können.

### Grundsätze

- Stabilität vor Performance
- Keine unbegründeten Änderungen
- Nachvollziehbare Implementierungen
- Reproduzierbare Tests
- Saubere Fehlerdiagnose
- Modularer Aufbau
- Dokumentation wichtiger Änderungen
- Keine ungeprüften Optimierungen

---

## Build

Die Entwicklungsumgebung und die benötigten Abhängigkeiten werden während der Entwicklung definiert.

Eine offizielle Build-Anleitung wird veröffentlicht, sobald ein reproduzierbarer Build-Prozess etabliert wurde.

---

## Installation

Eine offizielle Installationsanleitung ist derzeit nicht verfügbar.

Experimentelle Builds sollten nicht ohne vorherige Sicherung auf produktiv genutzten Systemen installiert werden.

NicoRed verändert nicht automatisch:

- EFI-Konfigurationen
- Bootloader
- Firmware-Einstellungen
- macOS-Systemdateien
- vorhandene Installationen

Installation und Konfiguration erfolgen manuell.

---

## Fehlerberichte

Fehlerberichte sind ein wichtiger Bestandteil der Entwicklung.

Bitte stelle bei einem Report möglichst folgende Informationen bereit:

| Information | Details |
|---|---|
| Hardware | Laptop / Systemmodell |
| CPU | Prozessor |
| GPU | AMD-Grafikmodell |
| macOS | Version und Build |
| NicoRed | Version / Build |
| Fehlerbeschreibung | Was ist passiert? |
| Reproduktion | Schritte zum Auftreten |
| Logs | System- und Kernel-Logs |

---

## Roadmap

- [ ] Projektstruktur etablieren
- [ ] Build-Umgebung definieren
- [ ] Build-Prozess dokumentieren
- [ ] GPU-Initialisierung analysieren
- [ ] Stabilitätsprobleme untersuchen
- [ ] Speicherverwaltung untersuchen
- [ ] Synchronisierungsprobleme analysieren
- [ ] Rendering-Probleme untersuchen
- [ ] Chromium-Kompatibilität testen
- [ ] Logging verbessern
- [ ] Testverfahren etablieren
- [ ] Dokumentation vervollständigen

---

## Entwickler

<p align="center">
  <a href="https://github.com/nicoasoh">
    <img src="https://github.com/nicoasoh.png" width="120" alt="GitHub-Profilbild">
  </a>
  <br>
  <b>nicoasoh</b>
  <br>
  Entwickler von NicoRed
  <br>
  <a href="https://github.com/nicoasoh">GitHub-Profil</a>
</p>

---

<p align="center">
  <b>NicoRed</b>
  <br>
  Entwicklung. Stabilität. Kontrolle.
</p>