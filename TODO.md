# TODO:
 * Dokumentation
 * saubere Kommentierung
    * tone.h/.c: Konstantin

# Vorstellung
 * Namen, Projekt
 * MSP430 in C, Education Board, Sequenzer
 * Sequenzer: Konstantin (Folien?)
 * Präsentation: Bildschirm(Start im Bearbeitungsmodus) - immer ein Schritt wird angezeigt und bearbeitet
                 Einstellungen: Tonhöhe, Tonlänge, Tempo
                 Modus: Abspielen
                 neue Melodie eingeben und abspielen
Ende Vorstellung, Fragen zu Code (Wechsel von Präsentation oder Kamera zu QUELLCODE)
 * main.c Haupteinstiegspunkt mit int main()
 * Bibliotheken: msp430 - Register und Funktionen des verwendeten Mikrorechners
                 stdbool - verwenden des Datentyps bool
                 eigene Bibliotheken
 * NICHT sequentiell: interrupts
 * main.c: INPUT HANDLING - Eingaben auslesen und entsprechend des Modus darauf reagieren
