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

	open_n2D();
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
	if (n2d) {
		n2d->~n2D();
		while (!n2DSeries.isEmpty())
			 delete n2DSeries.takeFirst();
		qDebug() << "n2d->~n2D();";
	}

	findAndPlotAllFiles();
	erstelle_n2D();
	connect(n2d, SIGNAL(fensterGeschlossen()), this, SLOT(n2DwurdeGesschlossen()));
	connect(n2d, SIGNAL(reOpenSignal()), this, SLOT(open_n2D()));
	connect(n2d, SIGNAL(hilfeSignal()), this, SLOT(hilfeDialog()));
	//n2d->setRasterXAchse(10); --> geht noch nicht
	qDebug() << "MainWindow::open_n2D()";
}

bool MainWindow::findAndPlotAllFiles()
{
	QDir dir(m_pfad); // auf Ordner /KohN-1
	QStringList filters;
	filters << "*.bin";
	dir.setNameFilters(filters);
	foreach (QString dateiName, dir.entryList())
	{
		getDataOneFile(m_pfad + dateiName);
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
	//[n2d]

	//Achsen setzen
//	n2d->setMinMaxXAchse(QPointF(achseMinSBox_f->value(), achseMaxSBox_f->value()));
//	n2d->setRasterXAchse(achseRasterSBox_f->value());
//	int i = 0;
//	foreach (SurfaceGraph *sg, listSurfaceGraph) {
//		n2d->setMinMaxYAchse(QPointF(sg->get_k().minAkt, sg->get_k().maxAkt), i++);
//	}

	//QSize screenSize = QApplication::desktop()->screen()->size();
	QSize screenSize = QGuiApplication::screens().first()->availableVirtualSize(); //Desktopsize ohne Taskleiste
	n2d->setMinimumWidth(screenSize.width() / 4); // Breite
	n2d->setMinimumHeight(screenSize.height() / 4); // Höhe
	n2d->resize(QSize(screenSize.width()/1.3, screenSize.height()/1.3)); // Breite, Höhe
	//n2d->move(QPoint(0,0));
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
			   "- Clicken auf die Legende kann die zugehörige Grafik ein bzw. ausgeblendet\n"
			   "	werden.\n"
			   "- Betätigen und halten der linken Maustaste kann in der Grafik der \n"
			   "	gewünschte x-Achsenbereich ausgewählt werden.\n"
			   "- Betätigen der rechten Maustaste wird der x-Achsenbereich vergrößert.\n"
			   "\n"
			   "ShortCuts: \n"
			   "- [Q] = Quit: Programm wird beendet.\n"
			   "- [R] = Reload: Grafen werden gelöscht und die Binärdateien\n"
			   "	werden neu eingelesen.\n"
			   "	Pfad der geladenen Binärdateien: \"%1\"\n"
			   "- [L] = y-Achsen werden logarithmisch dargestellt. Bei erneutem betätigen\n"
			   "	wird wieder linear dargestellt.\n"
			   "- [Entf] = Die ausgeblendeten Grafen werden gelöscht.\n"
			   "\n"
			   "					patrik.roth@gmx.de, 30.06.2020").arg(m_pfad) );
}
