#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	for (int n = 0; n < 3; ++n) {
		QLineSeries* series = new QLineSeries();
		n2DlistLineSeries.append(series); // --> verursacht Absturz da es wohl zum löschen von den QLineSeries kommt??
		n2DnameListBox.append(QString().number(n));
	}
	n2DnameListBox << "a" << "b" << "c";
	erstelle_n2D(n2DlistLineSeries,n2DnameListBox);
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

void MainWindow::erstelle_n2D(QList<QLineSeries *> &listLineSeries, QList<QString> &nameListGrafen)
{
	n2d = new n2D(listLineSeries, nameListGrafen);
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
