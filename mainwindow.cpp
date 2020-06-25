#include "mainwindow.h"
#include <QtCore/QtMath>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
//	for (int n = 0; n < 3; ++n) {
//		QLineSeries* series = new QLineSeries();
//		series->append(1, 20+n);
//		series->append(2, 21+n);
//		series->append(3, 22+n);
//		n2DSeries.append(series); // --> verursacht Absturz da es wohl zum löschen von den QLineSeries kommt??
//		n2DnameList.append(QString().number(n));
//	}
	addSeriesSin();
	addSeriesSin();
	addSeriesSin();
	addSeriesSin();
	addSeriesSin();
	addSeriesSin();
	erstelle_n2D();
	connect(n2d, SIGNAL(fensterGeschlossen()), this, SLOT(n2DwurdeGesschlossen()));
}

MainWindow::~MainWindow()
{
	// siehe closeEvent()
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	delete n2d;
	qDebug() << "Bin de Main closeEvent";
}

void MainWindow::erstelle_n2D()
{
	n2d = new n2D(n2DSeries, n2DnameList);
	//[n2d]

	//Achsen setzen
//	n2d->setMinMaxXAchse(QPointF(achseMinSBox_f->value(), achseMaxSBox_f->value()));
//	n2d->setRasterXAchse(achseRasterSBox_f->value());
//	int i = 0;
//	foreach (SurfaceGraph *sg, listSurfaceGraph) {
//		n2d->setMinMaxYAchse(QPointF(sg->get_k().minAkt, sg->get_k().maxAkt), i++);
//	}

//	n2d->setSchriftgroesse(schriftgroesseSBox->value());
//	connect(schriftgroesseSBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), n2d, QOverload<float>::of(&n2D::setSchriftgroesse));
//	connect(n2d, SIGNAL(changedMinX(double)), this->achseMinSBox_f, SLOT(setValue(double)));
//	connect(n2d, SIGNAL(changedMaxX(double)), this->achseMaxSBox_f, SLOT(setValue(double)));
//	connect(n2d, SIGNAL(changedMinX(double)), this, SLOT(setzeRasterVonMinMaxAchse_f()));
//	connect(n2d, SIGNAL(changedMaxX(double)), this, SLOT(setzeRasterVonMinMaxAchse_f()));
//	connect(n2d, SIGNAL(changedMinX(double)), this, SLOT(setSurfGrafAchsen()));
//	connect(n2d, SIGNAL(changedMaxX(double)), this, SLOT(setSurfGrafAchsen()));
	//connect(n2d, SIGNAL(fensterGeschlossen()), this, SLOT(close_n2D())); Verursacht ABSTURZ: Warum?

	n2d->resize(QSize(1000,500));
	n2d->move(QPoint(0,0));
	n2d->show();
}

void MainWindow::addSeriesSin()
{

	//	for (int n = 0; n < 3; ++n) {
	//		QLineSeries* series = new QLineSeries();
	//		series->append(1, 20+n);
	//		series->append(2, 21+n);
	//		series->append(3, 22+n);
	//		n2DSeries.append(series); // --> verursacht Absturz da es wohl zum löschen von den QLineSeries kommt??
	//		n2DnameList.append(QString().number(n));
	//	}

	QLineSeries *series = new QLineSeries();
	n2DSeries.append(series);
	//series->setName(QString("line " + QString::number(n2DSeries.count())));
	n2DnameList.append(QString("line " + QString::number(n2DSeries.count())));


	// Make some sine wave for data
	QList<QPointF> data;
	int offset = n2DSeries.count();
	for (int i = 0; i < 360; i++) {
		qreal x = offset * 20 + i;
		data.append(QPointF(i, qSin(qDegreesToRadians(x))));
	}

	series->append(data);
//    m_chart->addSeries(series);

//    if (m_series.count() == 1)
//        m_chart->createDefaultAxes();
}
