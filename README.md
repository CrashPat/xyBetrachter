# xyBetrachter
patrik.roth@gmx.de, 16.10.2020, V1

# Beschreibung:
Dient zum Darstellen und Analysieren von Datensätzen in einer 2D-Grafik in Form von beliebig vielen xy-Graphen. Alle Funktionalitäten sind in der Bedienungsanleitung aufgelistet.

Die Grundstrukturen der Klasse n2D sind aus dem Qt-Beispiel "legendmakers" aufgebaut.

# Bedienungsanleitung:

ZOOM:
- [Z]	x/y Achse umschalten:
  -	-: zurück: Rechten Maustaste drücken
  -	+: vergrössern: Linken Maustaste gedrückt halten,
	   gewünschter Bereich auswählen und linke Taste los lassen
- [Rad]	+/- y-Achsen
- [L] 	Logarithmische/Lineare y-Achsen umschalten
- [N] 	y-Achsen wird auf ihre maximale Darstellung gebracht.
- [M] 	x-Achse wird auf ihre maximale Darstellung gebracht.

GRAPHEN:
- [Entf]	Die ausgeblendeten Graphen werden geschlossen.
- [X'] 	x-Achse aus/ein-blenden.
- [Y] 	y-Achsen aus/ein-blenden.
- [H] 	Hilfslinien (Grid) aus/ein-blenden.
- [D] 	Dottet: umschalten zwischen Linear/Scattert-Darstellung
- [->] 	Kreuz nach rechts um einen Datenpunkt verschieben [rechte Pfeiltaste]
- [<-] 	Kreuz nach links um einen Datenpunkt verschieben [linke Pfeiltaste]
- [W] 	Werte auf Kreuz aus/ein-blenden.

LEGENDE:
- Legende einzeln ein/ausblenden: Mit Mauszeiger darüberfahren
- [1..9] 	Entsprechende Legenden ein/ausblenden
- [O] 	Ort der Legende oben/rechts/ausblenden umschalten
- [A] 	Alle Legenden ein/ausblenden

ALLGEMEIN: 
- [F] 	Fenster normal/max (Fullscreen) umschalten
- [Esc] 	Fenster ausblenden
- [T] 	Theme: Zwischen heller und dunkler Ansicht wechseln
- [P] 	Printscreen wird als PNG unter dem obigen genannten Pfad erzeugt.
- [R] 	Reload: Alle Grafen werden geschlossen und die Binärdateien
	werden neu eingelesen.
	Pfad der geladenen Dateien: "C:/Users/Patrik Roth/Downloads/"
- [Q] 	Quit: Programm wird beendet.

HILFE:
- [F1]	Hilfe
- [F2]	Über Qt
