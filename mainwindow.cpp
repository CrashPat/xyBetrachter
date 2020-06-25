#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	erstelle_n2D();
}

MainWindow::~MainWindow()
{
}

void MainWindow::erstelle_n2D()
{
	QList<QLineSeries *> listLineSeries;

	QList<QString> nameListBox;
	nameListBox << "a" << "b" << "c";

	n2d = new n2D(listLineSeries, nameListBox);
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
