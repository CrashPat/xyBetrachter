﻿#include "n2d.h"
//#include <QtCharts/QChart>
//#include <QtCharts/QChartView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtCore/QDebug>
#include <QtCharts/QLegend>
//#include <QtWidgets/QFormLayout>
#include <QtCharts/QLegendMarker>
#include <QtCharts/QLineSeries>
#include <QtCharts/QXYLegendMarker>
#include <QtCore/QtMath>
#include <QShortcut>

QT_CHARTS_USE_NAMESPACE

int n2D::countInstances = 0;

n2D::n2D(QList<QLineSeries *> listLineSeries)
{
	qDebug() << "n2D(): Anzahl Instanzen" << ++countInstances;

	// Create chart view with the chart
	m_chart = new QChart();
	m_chartView = new QChartView(m_chart, this);

	// Create layout for grid and detached legend
	m_mainLayout = new QGridLayout();
	m_mainLayout->addWidget(m_chartView, 0, 1, 3, 1);
	setLayout(m_mainLayout);

	//![2]
	m_axisX = new QCategoryAxis;
	m_axisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
	//m_axisX->setTickCount(((maxX-minX)/rasterX)+1); // Raster wird gesetzt
	//m_axisX->setRange(minX, maxX); // Bereich wird gesetzt
	//m_chart->addAxis(m_axisX, Qt::AlignBottom);
//	setMinMaxXAchse(QPointF(minX, maxX));
//	setRasterXAchse(rasterX);
	//![2]

	// Add few series & Kopie der Werte erstellen
	foreach (QLineSeries *ls, listLineSeries) {
		// Serie kopieren da sonst die Original Serie abstürzt
		QList<QPointF> vpf = ls->points(); // !!! erzeugt eine reale Kopie nur der Datenpunkte !!!
		QLineSeries *series = new QLineSeries();
		series->append(vpf);
		series->setUseOpenGL(true);
		series->setName(ls->name());
		addSeries(series);
	}

	connectMarkers();

	// Set the title and show legend
	setWindowTitle(tr("xyBetrachter: n x 2D               [F1] = Hilfe"));
	//m_chart->setTitle("Legendmarker example (click on legend)");
	m_chart->legend()->setVisible(true);
	//m_chart->legend()->setAlignment(Qt::AlignRight);

	m_chartView->setRubberBand(QChartView::HorizontalRubberBand); //
	//m_chartView->setRenderHint(QPainter::Antialiasing); //--> macht die Grafik sehr langsam

	connect(m_axisX, SIGNAL(rangeChanged(qreal, qreal)), this, SLOT(xAchsenBereich(qreal, qreal))); // für HorizontalRubberBand

//	qDebug() << "Serienfarben:";
//	foreach (QLineSeries *s, m_series) {
//		qDebug() << s->color().rgba();
//	}

	// Shortcuts:
	QShortcut *hilfe = new QShortcut(QKeySequence("F1"), this);
	QObject::connect(hilfe, SIGNAL(activated()), this, SLOT(hilfeSlot()));
	QShortcut *closeSCut = new QShortcut(QKeySequence("Q"), this);
	QObject::connect(closeSCut, SIGNAL(activated()), this, SLOT(close()));
	QShortcut *reopenSCut = new QShortcut(QKeySequence("R"), this);
	QObject::connect(reopenSCut, SIGNAL(activated()), this, SLOT(reOpenSlot()));
	QShortcut *yLogarithmisch = new QShortcut(QKeySequence("L"), this);
	QObject::connect(yLogarithmisch, SIGNAL(activated()), this, SLOT(setYLogarithmisch()));
	QShortcut *delLastSeries = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	QObject::connect(delLastSeries, SIGNAL(activated()), this, SLOT(removeSeries()));
	// --> Hilfetext nachtragen in MainWindow::hilfeDialog();
}

n2D::~n2D()
{
//	delete m_closeSCut; ...
	qDebug() << "~n2D(): Anzahl Instanzen" << --countInstances;
}

void n2D::addSeries(QLineSeries *series)
{
	 m_series.append(series);
	 m_chart->addSeries(series);

	 if (m_series.count() == 1) {
		 m_chart->createDefaultAxes();
		 m_chart->axisY()->hide();
	 }
	 // Dünnen Grafen zeichnen:
	 QPen pen = series->pen();
	 pen.setWidth(1);
	 series->setPen(pen);

	 // Achsen anhängen:
	 QValueAxis *axisY = new QValueAxis;
	 m_axisYList.append(axisY);
	 m_chart->addAxis(axisY, Qt::AlignLeft);
	 //series->attachAxis(m_axisX);
	 series->attachAxis(axisY);
	 axisY->setLinePenColor(series->pen().color());
	 axisY->setLabelsColor(series->pen().color());
}

void n2D::setYLogarithmisch()
{
	toggleBit(m_binLogarithmisch);
	if (!m_binLogarithmisch)
	{
//		QValueAxis *axisX = new QValueAxis();
//		//axisX->setTitleText("Data point");
//		//axisX->setLabelFormat("%i");
//		axisX->setTickCount(series->count());
//		chart->addAxis(axisX, Qt::AlignBottom);
//		series->attachAxis(axisX);
//	}
//	else
//	{
//		QLogValueAxis *axisY = new QLogValueAxis();
//		//axisY->setTitleText("Values");
//		//axisY->setLabelFormat("%g");
//		//axisY->setBase(8.0);
//		//axisY->setMinorTickCount(-1);
//		chart->addAxis(axisY, Qt::AlignLeft);
//		series->attachAxis(axisY);
	}

	qDebug() << "setYLogarithmisch(), m_binLogarithmisch = " << m_binLogarithmisch;
}

void n2D::removeSeries()
{
	 // Remove last series from chart
	 if (m_series.count() > 0) {
		  QLineSeries *series = m_series.last();
		  //series->attachedAxes().last()->hide();
		  delete series->attachedAxes().last();
		  m_chart->removeSeries(series);
		  m_series.removeLast();
		  delete series;
	 }
}

void n2D::connectMarkers()
{
	 // Connect all markers to handler
	 const auto markers = m_chart->legend()->markers();
	 for (QLegendMarker *marker : markers) {
		  // Disconnect possible existing connection to avoid multiple connections
		  QObject::disconnect(marker, &QLegendMarker::clicked,
									 this, &n2D::handleMarkerClicked);
		  QObject::connect(marker, &QLegendMarker::clicked, this, &n2D::handleMarkerClicked);
	 }
}

void n2D::disconnectMarkers()
{
	 const auto markers = m_chart->legend()->markers();
	 for (QLegendMarker *marker : markers) {
		  QObject::disconnect(marker, &QLegendMarker::clicked,
									 this, &n2D::handleMarkerClicked);
	 }
}

void n2D::handleMarkerClicked()
{
	 QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());
	 Q_ASSERT(marker);

	 switch (marker->type())
	 {
		  case QLegendMarker::LegendMarkerTypeXY:
		  {
	//![5]
			  // Toggle visibility of series
			  marker->series()->setVisible(!marker->series()->isVisible());
			  marker->series()->attachedAxes().last()->setVisible(marker->series()->isVisible()); // y-Achse Ein/Aus blenden

			  // Turn legend marker back to visible, since hiding series also hides the marker
			  // and we don't want it to happen now.
			  marker->setVisible(true);
	//![5]

	//![6]
			  // Dim the marker, if series is not visible
			  qreal alpha = 1.0;

			  if (!marker->series()->isVisible())
					alpha = 0.5;

			  QColor color;
			  QBrush brush = marker->labelBrush();
			  color = brush.color();
			  color.setAlphaF(alpha);
			  brush.setColor(color);
			  marker->setLabelBrush(brush);

			  brush = marker->brush();
			  color = brush.color();
			  color.setAlphaF(alpha);
			  brush.setColor(color);
			  marker->setBrush(brush);

			  QPen pen = marker->pen();
			  color = pen.color();
			  color.setAlphaF(alpha);
			  pen.setColor(color);
			  marker->setPen(pen);
	//![6]
			  break;
		  }
	 default:
		  {
			  qDebug() << "Unknown marker type";
			  break;
		  }
	 }
}

void n2D::setMinMaxXAchse(QPointF x)
{
	m_chart->axisX()->setRange(x.x(), x.y());
	//qDebug() << QString("n2D::setMinMaxXAchse(QPointF(%1,%2))").arg(x.x()).arg(x.y());
}

void n2D::setRasterXAchse(float rasterX)
{ // ähnlich wie Chart2D::setRaster(float *pRasterHz)
	float minX, maxX;
	minX = m_axisX->min();
	maxX = m_axisX->max();
	//qDebug() << QString("n2D::setRasterXAchse(rasterX = %1)").arg(rasterX);

	foreach(QString label, m_axisX->categoriesLabels()) {
		m_axisX->remove(label);
	}
	int vielfacheStart = ceil(minX/rasterX); // damit nur der Achsenausschnitt beschriftet wird
	int nAbbruch = 500; // wenn ausversehen ein zu kleines Raster gesetzt wurde
	for (float rHz = rasterX*vielfacheStart; rHz <= maxX; rHz = rHz + rasterX ) {
		m_axisX->append(QString("%1").arg(rHz), rHz);
		nAbbruch--;
		if (nAbbruch<=0) {
			//QMessageBox::warning(this, "Raster zu klein", QString("Ihr 2D-Raster ist auf %1 Hz eingestellt und für die Darstellung somit zu klein. Bitte erhöhen sie das 2D-Raster.").arg(rasterHz));
			break;
		}
	}
	bool gedreht;
	if (m_axisX->count()>11) {// Bei vielen Labels um -90° drehen um Platz zu sparen.
		m_axisX->setLabelsAngle(-90); gedreht = true;
	} else {
		m_axisX->setLabelsAngle(0); gedreht = false;
	}

//	if (jaXAchse) {
		if (m_axisX->categoriesLabels().count()!=0) { // Einheit in Hz beim ersten Label setzen
			QString erstesLabel = m_axisX->categoriesLabels().at(0);
			//if (*pBinDemod) m_axisX->replaceLabel(erstesLabel, erstesLabel + " ms");
			///*else*/ m_axisX->replaceLabel(erstesLabel, erstesLabel + " Hz");
		}
//	}
}

void n2D::setMinMaxYAchse(QPointF y, int instanzNr)
{
	m_axisYList.at(instanzNr)->setRange(y.x(), y.y());
	//m_chart->axisY()->setRange(y.x(), y.y());
	//qDebug() << QString("n2D::setMinMaxYAchse(QPointF(%1,%2))").arg(y.x()).arg(y.y());
}

void n2D::setSchriftgroesse(float fsize)
{
	QFont schrift;
	schrift.setPointSize(fsize);
	m_chart->legend()->setFont(schrift);
	m_axisX->setLabelsFont(schrift);
	foreach (QValueAxis *axisY, m_axisYList) {
		axisY->setLabelsFont(schrift);
	}
}

void n2D::xAchsenBereich(qreal minX, qreal maxX)
{ // für HorizontalRubberBand
	QList<QPointF> p = m_series.at(0)->points();
	QPointF min, max;
	min = p.first();
	max = p.last();

	if ( minX >= min.x())
		emit changedMinX(minX);
	if (maxX <= max.x())
		emit changedMaxX(maxX);
	if ((minX <  min.x()) | (maxX > max.x()))
		setMinMaxXAchse(QPointF(min.x(), max.x()));
	qDebug() << QString("n2D::xAchsenBereich(float minX = %1, float maxX = %2)").arg(minX).arg(maxX);
}



