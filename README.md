# xyBetrachter
Patrik Roth, 16.10.2020, V1

![Titelbild](https://github.com/CrashPat/xyBetrachter/blob/master/Titelbild.png)

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
- [F3]	Über CSV-Dateien: Aufbau von einlesbaren Dateien

CSV-DATEIEN:
<br>Möglicher Inhalt einer CSV-Datei:
<br>erste; zweite mit ; dritte;und vierte spalte
<br>2;1.90E+03;1.5;100
<br>4;2.00E+03;2.6;97
<br>6;2.10E+03;3.7;94
<br>8;2.20E+03;4.8;91
<br>10;2.30E+03;5.9;88
<br>12;2.40E+03;7;85'
<br><br>Zu beachten ist:
- ';' separiert die Werte
- '.' Werte nur mit Punkt nicht mit Komma
- Spaltenüberschriften sind optional. Dies wird automatisch erkannt in dem in der ersten Spaltenüberschrift keine Zahl vorhanden ist.
