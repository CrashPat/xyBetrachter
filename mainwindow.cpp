#include "mainwindow.h"
#include <QtCore/QtMath>
#include <QMessageBox>
#include <QDir>
#include <QGuiApplication>
#include <QScreen>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	qDebug() << "MainWindow()";

	//#define BEISPIEL_DATEN
	#ifdef BEISPIEL_DATEN
	// Beispiel für Daten 1
	for (int n = 0; n < 3; ++n) {
		QLineSeries* series = new QLineSeries();
		series->append(1, 20+n);
		series->append(2, 21+n);
		series->append(3, 22+n);
		n2DSeries.append(series); // --> verursacht Absturz da es wohl zum löschen von den QLineSeries kommt??
		n2DnameList.append(QString().number(n));
	}
	#endif //BEISPIEL_DATEN

	//#define BEISPIEL_SINUS
	#ifdef BEISPIEL_SINUS
	//Beispiel für Daten 2
	addSeriesSin();
	addSeriesSin();
	addSeriesSin();
	#endif //BEISPIEL_SINUS

	while (!open_n2D())
	{
		QMessageBox msgBox;
		msgBox.setText("Keine Dateien gefunden.");
		msgBox.setInformativeText("Wollen Sie es erneut versuchen?");
		msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Close);
		msgBox.setDefaultButton(QMessageBox::Retry);
		int ret = msgBox.exec();

		if (ret == QMessageBox::Close)
		{
			this->~MainWindow();
			return;
		}
	}
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

bool MainWindow::open_n2D()
{
	if (n2d) {
		n2d->~n2D();
		while (!n2DSeries.isEmpty())
			 delete n2DSeries.takeFirst();
		qDebug() << "n2d->~n2D();";
	}

	if (!findAndPlotAllFiles())
	{
		return false;
	}
	erstelle_n2D();
	connect(n2d, SIGNAL(fensterGeschlossen()), this, SLOT(n2DwurdeGesschlossen()));
	connect(n2d, SIGNAL(reOpenSignal()), this, SLOT(open_n2D()));
	connect(n2d, SIGNAL(hilfeSignal()), this, SLOT(hilfeDialog()));
	//n2d->setRasterXAchse(10); --> geht noch nicht
	qDebug() << "MainWindow::open_n2D()";
	return true;
}

bool MainWindow::findAndPlotAllFiles()
{
	QDir dir(m_pfad); // auf Ordner
	QStringList filters;
	filters << "*.bin";
	dir.setNameFilters(filters);
	if (!dir.count())
	{
		QMessageBox::warning(this, "Warnung", tr("Im Pfad \"%1\" sind keine Binärdateien vorhanden").arg(m_pfad) );
		return false;
	}
	foreach (QString dateiName, dir.entryList())
	{
		if (!getDataOneFile(m_pfad + dateiName))
		{
			return false;
		}
	}

	return true;
}

bool MainWindow::getDataOneFile(QString DateiMitPfad)
{
	/// open file
	QFile file(DateiMitPfad);

	if (!file.open(QIODevice::ReadOnly)){ // öffnen der Datei
		QMessageBox::warning(this, "Warnung", tr("Folgende Datei konnte nicht geöffnet werden: \"%1\"").arg(DateiMitPfad) );
		return false;
	}

	/// Dateiinhalt (filecontent) in DataOneFile
	std::vector<float> werte;
	werte.resize(file.size() / sizeof(float));
	file.read(reinterpret_cast<char *>(&werte[0]), file.size());
	file.close();

	qDebug() << "werte.at(0) =" << werte.at(0); // Werte ausgeben
//	qDebug() << werte; // Werte ausgeben
//	qDebug() << "file.size() =" << file.size();
//	qDebug() << "werte.size() =" << werte.size();
//	qDebug() << "sizeof(float) =" << sizeof(float);
	qDebug() << "Datei" << DateiMitPfad << "wurde geöffnet.";

	// Daten bei n2D eintragen:
	QLineSeries *series = new QLineSeries();
	n2DSeries.append(series);
	QFileInfo fileInfo(file.fileName()); // um den Pfad zu entfernen
	series->setName(fileInfo.fileName());
	QList<QPointF> data;
	for (int i = 0; i < werte.size(); i++) {
		data.append(QPointF(i, werte[i]));
	}
	series->append(data);

	return true;
}

void MainWindow::erstelle_n2D()
{
	n2d = new n2D(n2DSeries);

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
	for (int i = 0; i < 360; i++) {
		qreal x = offset * 20 + i;
		data.append(QPointF(i, qSin(qDegreesToRadians(x))));
	}

	series->append(data);
}

void MainWindow::hilfeDialog()
{
		QMessageBox::information(n2d, "Hilfe",
		  tr(  "Grafik:\n"
			   "- Mit Maus über Legende fahren für ein/ausblenden.\n"
			   "- Betätigen und halten der linken Maustaste kann in der Grafik der \n"
			   "	gewünschte x-Achsenbereich ausgewählt werden.\n"
			   "- Betätigen der rechten Maustaste wird der x-Achsenbereich vergrößert.\n"
			   "\n"
			   "ShortCuts: \n"
			   "- [Q] 	Quit: Programm wird beendet.\n"
			   "- [R] 	Reload: Grafen werden gelöscht und die Binärdateien\n"
			   "	werden neu eingelesen.\n"
			   "	Pfad der geladenen Binärdateien: \"%1\"\n"
			   "- [Entf]	Die ausgeblendeten Graphen werden gelöscht.\n"
			   "- [L] 	Logarithmische y-Achsen\n"
			   "	wird wieder linear dargestellt.\n"
			   "- [T] 	Theme: Zwischen heller und dunkler Ansicht wechseln.\n"
			   "- [M] 	x-Achsen wird auf ihre maximale Darstellung gebracht.\n"
			   "- [X] 	x-Achse aus/ein-blenden.\n"
			   "- [Y] 	y-Achsen aus/ein-blenden.\n"
			   "- [G] 	Hilfslinien (Grid) aus/ein-blenden.\n"
			   "- [P] 	Printscreen wird als PNG unter dem obigen genannten Pfad erzeugt.\n"
			   "- [F] 	Fenster MAX (Fullscreen)\n"
			   "- [Esc] 	Fenster ausblenden\n"
			   "- [D] 	Dottet: Toggeln zwischen Linear/Scattert-Darstellung\n"
			   "- [1..9] 	Entsprechende Legenden ein/ausblenden\n"
			   "\n"
			   "					patrik.roth@gmx.de, 16.07.2020").arg(m_pfad) );
}
