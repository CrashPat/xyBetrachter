#include "mainwindow.h"
#include <QtCore/QtMath>
#include <QMessageBox>
#include <QDir>
#include <QGuiApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	qDebug() << "MainWindow()";

	open_n2D();
	return;
}

MainWindow::~MainWindow()
{
	qDebug() << "~MainWindow()";
	// siehe closeEvent()
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	//delete n2d;
	qDebug() << "Bin de Main closeEvent";
}

void MainWindow::open_n2D()
{
	bool dateienGefunden;
	bool useBeispieldaten = false;
	do
	{
		if (!useBeispieldaten)
		{
			if (n2d)
			{
				n2d->~n2D();
				while (!n2DSeries.isEmpty())
					delete n2DSeries.takeFirst();
				qDebug() << "n2d->~n2D();";
			}

			dateienGefunden = findAndPlotAllFiles();

			if (!dateienGefunden)
			{
				QMessageBox msgBox;
				msgBox.setText("Keine Dateien gefunden.");
				msgBox.setInformativeText("Wollen Sie es erneut versuchen\n"
										  "oder Daten generieren [Open]?");
				msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Open | QMessageBox::Close);
				msgBox.setDefaultButton(QMessageBox::Retry);
				int ret = msgBox.exec();

				if (ret == QMessageBox::Close)
				{
					this->~MainWindow();
					return;
				}
				else if (ret == QMessageBox::Open)
				{
//#define BEISPIEL_DATEN
#ifdef BEISPIEL_DATEN
					// Beispiel für Daten 1
					for (int n = 0; n < 3; ++n)
					{
						QLineSeries *series = new QLineSeries();
						series->append(1, 20 + n);
						series->append(2, 21 + n);
						series->append(3, 22 + n);
						n2DSeries.append(series); // --> verursacht Absturz da es wohl zum löschen von den QLineSeries kommt??
						n2DnameList.append(QString().number(n));
					}
#endif //BEISPIEL_DATEN

#define BEISPIEL_SINUS
#ifdef BEISPIEL_SINUS
					//Beispiel für Daten 2
					addSeriesSin();
					addSeriesSin();
					addSeriesSin();
#endif //BEISPIEL_SINUS

					useBeispieldaten = true;
				}
			}
		}
		else
		{
			useBeispieldaten = false;
			dateienGefunden = true;
		}
	} while (!dateienGefunden);

	erstelle_n2D();
	connect(n2d, SIGNAL(fensterGeschlossen()), this, SLOT(n2DwurdeGesschlossen()));
	connect(n2d, SIGNAL(reOpenSignal()), this, SLOT(open_n2D()));
	connect(n2d, SIGNAL(hilfeSignal()), this, SLOT(hilfeDialog()));
	connect(n2d, SIGNAL(ueberQtSignal()), qApp, SLOT(aboutQt()));
	connect(n2d, SIGNAL(ueberCSVSignal()), this, SLOT(ueberCSVDialog()));
	//n2d->setRasterXAchse(10); --> geht noch nicht
	qDebug() << "MainWindow::open_n2D()";
}

bool MainWindow::findAndPlotAllFiles()
{
	QDir dir(m_pfad); // auf Ordner
	QStringList filtersBin, filterCSV;
	filtersBin << "*.bin";
	filtersBin << "*.sbin";
	filterCSV << "*.csv";
	filterCSV << "*.txt";
	bool foundBin = false;
	bool foundCsv = false;

	// .bin
	dir.setNameFilters(filtersBin);
	foundBin = (dir.count() > 0);
	if (foundBin)
	{
		foreach (QString dateiName, dir.entryList())
		{
			if (!getDataOneFileBin(m_pfad + dateiName))
				return false;
		}
	}

	// .csv
	dir.setNameFilters(filterCSV);
	foundCsv = (dir.count() > 0);
	if (foundCsv)
	{
		foreach (QString dateiName, dir.entryList())
		{
			if (!getDataOneFileCsv(m_pfad + dateiName))
				return false;
		}
	}

	// Meldung
	if (!foundBin & !foundCsv)
	{
		QMessageBox::warning(this, "Warnung",
			tr("Im Pfad \"%1\" sind keine Binärdateien als auch CSV-Dateien vorhanden").arg(m_pfad));
		return false;
	}

	return true;
}

bool MainWindow::getDataOneFileBin(QString DateiMitPfad)
{
	/// open file
	QFile file(DateiMitPfad);

	if (!file.open(QIODevice::ReadOnly))
	{ // öffnen der Datei
		QMessageBox::warning(this, "Warnung", tr("Folgende Datei konnte nicht geöffnet werden: \"%1\"").arg(DateiMitPfad));
		return false;
	}

	/// Dateiinhalt (filecontent) in DataOneFile
	std::vector<float> werte;
	werte.resize(file.size() / sizeof(float));
	qint64 nBytes = file.read(reinterpret_cast<char *>(&werte[0]), file.size());
	file.close();

	qDebug() << "nBytes =" << nBytes;
	//	qDebug() << "werte.at(0) =" << werte.at(0); // Werte ausgeben
	//	qDebug() << werte; // Werte ausgeben
	//	qDebug() << "file.size() =" << file.size();
	//	qDebug() << "werte.size() =" << werte.size();
	//	qDebug() << "sizeof(float) =" << sizeof(float);
	qDebug() << "Datei" << DateiMitPfad << "wurden Werte eingelesen.";

	// Daten bei n2D eintragen:
	QLineSeries *series = new QLineSeries();
	n2DSeries.append(series);
	QFileInfo fileInfo(file.fileName()); // um den Pfad zu entfernen
	series->setName(fileInfo.fileName());
	QList<QPointF> data;
	for (int i = 0; i < werte.size(); i++)
	{
		data.append(QPointF(i, werte[i]));
	}
	series->append(data);

	// y MinMax abspeichern
	std::sort(werte.begin(), werte.end());
	yMinMax xym;
	xym.yMin = werte.front();
	xym.yMax = werte.back();
	nXminmax.append(xym);

	return true;
}

bool MainWindow::getDataOneFileCsv(QString DateiMitPfad)
{ // https://de.wikipedia.org/wiki/CSV_(Dateiformat)
	/// open file
	QFile file(DateiMitPfad);

	if (!file.open(QIODevice::ReadOnly))
	{ // öffnen der Datei
//		QMessageBox::warning(this, "Warnung", tr("Folgende Datei konnte nicht geöffnet werden: \"%1\"").arg(DateiMitPfad));
		QMessageBox::warning(this, "Warnung", file.errorString());
		return false;
	}

	/// Dateiinhalt (filecontent) in DataOneFile
	/// Header
	QByteArray line = file.readLine();
	QList<QByteArray> werteZeile = line.split(';'); // = Seperator Zeile
	if (werteZeile.size()==1)
		  werteZeile = line.split('\t'); // = Seperator Zeile = Tablullator
//	if (werteZeile.length()==1)
//		werteZeile = line.split(' '); // = Seperator = Leerzeichen --> VORSICHT: Tabellenüberschriften fürfen keine Leerzeichen in sich haben.

	QStringList spaltenueberschriften;
	if (!contains_number(werteZeile.first().toStdString())) // Wenn die allerste Zelle in der Datei kein Zahlenwert hat dann ist es eine Überschrift
	{ // has Header
		foreach (QString spalteName, werteZeile) {
			spaltenueberschriften.append(spalteName.simplified()); // simplified() --> überflüssiger Zeilenumbruch löschen
		}
	}
	qDebug() << " Spaltenüberschriften:" << spaltenueberschriften;

	/// Body of Data:
	int nSpalten = werteZeile.size(); // Anzahl Spalten
	QVector<QVector<float>> spalten(nSpalten); // Spalten erzeugen, [col][row]

	bool keineUeberschrift = !spaltenueberschriften.size(); // Null länge

	while (!file.atEnd()) { // Daten abfüllen
		// Wenn Überschriften vorhanden sind, dann neue Linie beim ersten mal nicht einlesen, sonst geht die erste Zeile verloren.
		if (!keineUeberschrift) {
			line = file.readLine();

			line.replace(",", "."); // Kommazahlen in Punkzahlen umwandeln, damit die Zahlenwerte richtig eingelesenwerden

			werteZeile = line.split(';'); // = Seperator Zeile
			if (werteZeile.size()==1)
				werteZeile = line.split('\t'); // = Seperator Zeile = Tablullator

			if (werteZeile.size() != nSpalten)
			{
				QMessageBox::warning(this, "Warnung", QString("Anzahl Datenspalten zur vorangegangenen Zeile stimmen nicht überein! "
															  "Siehe Zeile %1 in '%2'. (Bitte korrigieren)").arg(spalten[0].size()+1).arg(DateiMitPfad));
				return false;
			}
			nSpalten = werteZeile.size();
		}
		else {		
			nSpalten = werteZeile.size(); // Notwendig, wenn keine Überschrift vorhanden ist
			spalten.resize(nSpalten);
			keineUeberschrift = false;
		}

		for (int i = 0; i < nSpalten; ++i) {
			spalten[i].append(werteZeile[i].toDouble());
		}
	}
	file.close();

	qDebug() << " Spalten:" << spalten;
	qDebug() << "Datei" << DateiMitPfad << "wurden Werte eingelesen.";
	qDebug() << "file.size()       =" << file.size();
	qDebug() << "spalten.size()    =" << spalten.size() << "= Spalten";
	qDebug() << "spalten[0].size() =" << spalten[0].size() << "= Zeilen";
	qDebug() << "sizeof(float)     =" << sizeof(float);


	// Daten bei n2D eintragen:
	int i = 0;
	foreach (QVector<float> werte, spalten) {
		QLineSeries *series = new QLineSeries();
		n2DSeries.append(series);
		QFileInfo fileInfo(file.fileName()); // um den Pfad zu entfernen

		// Überschriften:
		QString seriesName;
		uint nSpalten = spaltenueberschriften.size();
		if (nSpalten)
			seriesName += spaltenueberschriften.at(i++) + ": ";
		seriesName += fileInfo.fileName();
		series->setName(seriesName);

		// Daten:
		QList<QPointF> data;
		for (int i = 0; i < werte.size(); i++)
		{
			data.append(QPointF(i, werte[i])); // (x,y)
		}
		series->append(data);
		qDebug() << "data" << data;

		// y MinMax abspeichern:
		std::sort(werte.begin(), werte.end());
		yMinMax xym;
		xym.yMin = werte.front();
		xym.yMax = werte.back();
		nXminmax.append(xym);
	}
	return true;
}

bool MainWindow::contains_number(const std::string &c)
{
	return (c.find_first_of("0123456789") != std::string::npos);
}

void MainWindow::erstelle_n2D()
{
	n2d = new n2D(n2DSeries, nXminmax);

	////	QSize screenSize = QApplication::desktop()->screen()->size();
	//	QSize screenSize = QGuiApplication::screens().first()->availableVirtualSize(); //Desktopsize ohne Taskleiste
	//	n2d->setMinimumWidth(screenSize.width() / 4); // Breite
	//	n2d->setMinimumHeight(screenSize.height() / 4); // Höhe
	//	n2d->resize(QSize(screenSize.width()/1.3, screenSize.height()/1.3)); // Breite, Höhe
	//	n2d->move(QPoint(0,0));
	n2d->showMaximized();
	n2d->show();
}

void MainWindow::addSeriesSin()
{
	QLineSeries *series = new QLineSeries();
	n2DSeries.append(series);
	series->setName(QString("line " + QString::number(n2DSeries.count())));

	// Make some sine wave for data
	QList<QPointF> data;
	int offset = n2DSeries.count();
	for (int i = 0; i < 360; i++)
	{
		qreal x = offset * 20 + i;
		if (offset == 1) // --> dient nur um Logarithmus benutzen zu können
			data.append(QPointF(i, qAbs(qSin(qDegreesToRadians(x)))));
		else
			data.append(QPointF(i, qSin(qDegreesToRadians(x))));
	}

	series->append(data);
}

void MainWindow::hilfeDialog()
{
	QString text = QString(
		"ZOOM:\n"
		"- [Z]	x/y Achse umschalten:\n"
		"--	-: zurück: Rechten Maustaste drücken\n"
		"--	+: vergrössern: Linken Maustaste gedrückt halten,\n"
		"	   gewünschter Bereich auswählen und linke Taste los lassen\n"
		"- [Rad]	+/- y-Achsen\n"
		"- [L] 	Logarithmische/Lineare y-Achsen umschalten\n"
		"- [N] 	x-Achse wird auf ihre maximale Darstellung gebracht.\n"
		"- [M] 	y-Achsen wird auf ihre maximale Darstellung gebracht.\n"
		"- [,] 	y-Achsen: Gleiche Skalierung aller y-Achsen\n"
		"                             (ymin=0, ymax=größte aller Werte)\n"
		"\n"
		"GRAPHEN:\n"
		"- [Entf]	Die ausgeblendeten Graphen werden geschlossen.\n"
		"- [X] 	x-Achse aus/ein-blenden.\n"
		"- [Y] 	y-Achsen aus/ein-blenden.\n"
		"- [H] 	Hilfslinien (Grid) aus/ein-blenden.\n"
		"- [D] 	Dottet: umschalten zwischen Linear/Scattert-Darstellung\n"
		"- [->] 	Kreuz nach rechts um einen Datenpunkt verschieben [rechte Pfeiltaste]\n"
		"- [<-] 	Kreuz nach links um einen Datenpunkt verschieben [linke Pfeiltaste]\n"
		"- [W] 	Werte auf Kreuz aus/ein-blenden.\n"
		"- [E] 	Exponentielle Zahlendarstellung (umschalten)\n"
		"\n"
		"LEGENDE:\n"
		"- Legende einzeln ein/ausblenden: Mit Mauszeiger darüberfahren\n"
		"- [1..9] 	Entsprechende Legenden ein/ausblenden\n"
		"- [O] 	Ort der Legende oben/rechts/ausblenden umschalten\n"
		"- [A] 	Alle Legenden ein/ausblenden\n"
		"\n"
		"ALLGEMEIN: \n"
		"- [F] 	Fenster normal/max (Fullscreen) umschalten\n"
		"- [Esc] 	Fenster ausblenden\n"
		"- [T] 	Theme: Zwischen heller und dunkler Ansicht wechseln\n"
		"- [P] 	Printscreen wird als PNG unter dem obigen genannten Pfad erzeugt.\n"
		"- [R] 	Reload: Alle Grafen werden geschlossen und die Binärdateien\n"
		"	werden neu eingelesen.\n"
		"	Pfad der geladenen Dateien: \"%1\"\n"
		"- [Q] 	Quit: Programm wird beendet.\n"
		"\n"
		"HILFE:\n"
		"- [F1]	Hilfe\n"
		"- [F2]	Über Qt\n"
		"- [F3]	Über CSV-Dateien: Aufbau von einlesbaren Dateien\n"
		"\n"
		"					Patrik Roth, 28.10.2020, V1.1\n"
		"\n"
		"ToDo: [,] yAxe->setRange(0, m_YmaxAll) benötigt viel Rechenzeit bei über 100dert Achsen").arg(m_pfad);
	qDebug() << text.toStdString().c_str(); // einfaches Kopieren des Hilfetextes
	QMessageBox::information(n2d, "Hilfe", text);
}

void MainWindow::ueberCSVDialog()
{
	QString text = QString(
		"CSV-Dateien:\n"
		"\n"
		"Möglicher Inhalt einer CSV-Datei:\n"
		"	erste; zweite mit ; dritte;und vierte spalte\n"
		"	2;1.90E+03;1.5;100\n"
		"	4;2.00E+03;2.6;97\n"
		"	6;2.10E+03;3.7;94\n"
		"	8;2.20E+03;4.8;91\n"
		"	10;2.30E+03;5.9;88\n"
		"	12;2.40E+03;7;85\n"
		"\n"
		"Zu beachten ist:\n"
		"- ';' oder '\t' (=Tabulator) separiert die Werte\n"
		"- '.' Werte nur mit Punkt nicht mit Komma\n"
		"- Spaltenüberschriften sind optional. Dies wird automatisch erkannt in dem in der ersten Spaltenüberschrift keine Zahl vorhanden ist.");
	qDebug() << text.toStdString().c_str(); // einfaches Kopieren des Hilfetextes
	QMessageBox::information(n2d, "Hilfe", text);
}
