#include "n2d.h"
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
#include <QtCharts/QLogValueAxis>
#include <QLayout>

QT_CHARTS_USE_NAMESPACE

int n2D::countInstances = 0;

n2D::n2D(QList<QLineSeries *> listLineSeries)
{
	qDebug() << "n2D(): Anzahl Instanzen" << ++countInstances;

	// Create chart view with the chart
	m_chart = new QChart();
	setChart(m_chart);
	//m_chartView = new ChartViewPat(m_chart);
	/*m_chartView->*/setRubberBand(QChartView::HorizontalRubberBand); //
	/*m_chartView->*/setMouseTracking(true);
	qDebug() << "m_chartView->hasMouseTracking()" << /*m_chartView->*/hasMouseTracking();
	setMouseTracking(true);

	//m_chartView->setRenderHint(QPainter::Antialiasing); //--> macht die Grafik sehr langsam
	//	m_chartView->setDragMode(QGraphicsView::NoDrag);
	//	m_chartView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//	m_chartView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	// Create layout for grid and detached legend
//	m_mainLayout = new QGridLayout();

//	m_mainLayout->addWidget(m_chartView);
	//m_mainLayout->setSpacing(0);
//	setLayout(m_mainLayout);
	//setTheme();

	//![2]
	m_axisX = new QCategoryAxis;
	m_axisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
	//m_chart->addAxis(m_axisX, Qt::AlignBottom); //MACHT PROBLEME
//	connect(m_axisX, SIGNAL(rangeChanged(qreal, qreal)), this, SLOT(xAchsenBereich(qreal, qreal))); // für HorizontalRubberBand
	//![2]

	// Add few series & Kopie der Werte erstellen
	m_coordX = new QGraphicsSimpleTextItem(m_chart);
	m_coordX->setPos(m_chart->size().width()/2, m_chart->size().height());

	int n = 1;
	foreach (QLineSeries *ls, listLineSeries) {
		// Serie kopieren da sonst die Original Serie abstürzt
		QList<QPointF> vpf = ls->points(); // !!! erzeugt eine reale Kopie nur der Datenpunkte !!!
		QLineSeries *series = new QLineSeries();
		series->append(vpf);
		series->setUseOpenGL(true);
		series->setName(QString("%2 (%1)").arg(n++).arg(ls->name()));
		addSeries(series);
		m_coordListY.append(new QGraphicsSimpleTextItem(m_chart));
	}


	connectMarkers();

	// Set the title and show legend
	setWindowTitle(tr("xyBetrachter: n x 2D               [F1] = Hilfe"));
	//m_chart->setTitle("Legendmarker example (click on legend)");
	m_chart->legend()->setVisible(true);
	//m_chart->legend()->setAlignment(Qt::AlignRight);

	// Shortcuts:
	QShortcut *hilfe = new QShortcut(QKeySequence("F1"), this);
	QObject::connect(hilfe, SIGNAL(activated()), this, SLOT(hilfeSlot()));
	QShortcut *delLastSeries = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	QObject::connect(delLastSeries, SIGNAL(activated()), this, SLOT(removeHiddenSeries()));
	QShortcut *closeSCut = new QShortcut(QKeySequence("Q"), this);
	QObject::connect(closeSCut, SIGNAL(activated()), this, SLOT(close()));
	QShortcut *reopenSCut = new QShortcut(QKeySequence("R"), this);
	QObject::connect(reopenSCut, SIGNAL(activated()), this, SLOT(reOpenSlot()));
	QShortcut *yLogarithmisch = new QShortcut(QKeySequence("L"), this);
	QObject::connect(yLogarithmisch, SIGNAL(activated()), this, SLOT(setYLogarithmisch()));
	QShortcut *xMinMax = new QShortcut(QKeySequence("M"), this);
	QObject::connect(xMinMax, SIGNAL(activated()), this, SLOT(setMinMaxXAchse()));
	QShortcut *theme = new QShortcut(QKeySequence("T"), this);
	QObject::connect(theme, SIGNAL(activated()), this, SLOT(setTheme()));
	// --> Hilfetext nachtragen in MainWindow::hilfeDialog();
}

n2D::~n2D()
{
	//delete m_chartView;
	//delete m_chart;
	//delete m_axisX;
	//delete m_mainLayout;
	qDebug() << "~n2D(): Anzahl Instanzen" << --countInstances;
}

void n2D::mouseMoveEvent(QMouseEvent *event)
{
//	QGraphicsSimpleTextItem *m_coordX = new QGraphicsSimpleTextItem(m_chart);
//	m_coordX->setPos(m_chart->size().width()/2 - 50, m_chart->size().height());
	qreal x = m_chart->size().width()-60;
	qreal y = m_chart->size().height()-20;

	m_coordX->setPos(x, y);
	m_coordX->setText(QString("X:%1").arg(m_chart->mapToValue(event->pos()).x()));
	int rechtsVersatz = 0;
	for (int n = m_coordListY.count()-1; n+1 ; --n) {
		x = rechtsVersatz*70+10;
		m_coordListY.at(n)->setPos(x, y);
		if (m_series.at(n)->isVisible()) {
			++rechtsVersatz;
			m_coordListY.at(n)->setPen(m_series.at(n)->pen());
			m_coordListY.at(n)->setText(QString("%1:%2").arg(n+1).arg(m_chart->mapToValue(event->pos(), m_series.at(n)).y()));
		}
		else
			m_coordListY.at(n)->setText("");
	}

	QChartView::mouseMoveEvent(event); // muss weiter gereicht werden sonst geht Rubberband nicht
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

	 addAxisYlinear(series);
	 //addAxisYlogarithmisch(series);
}

void n2D::addAxisYlinear(QLineSeries *series)
{
	// Achsen anhängen:
	QValueAxis *axisY = new QValueAxis;
	m_chart->addAxis(axisY, Qt::AlignLeft);
	//series->attachAxis(m_axisX);
	series->attachAxis(axisY);
	axisY->setLinePenColor(series->pen().color());
	axisY->setLabelsColor(series->pen().color());
}

void n2D::addAxisYlogarithmisch(QLineSeries *series)
{
	// Achsen anhängen:
	QLogValueAxis *axisY = new QLogValueAxis;
	//axisY->setTitleText("Values");
	//axisY->setLabelFormat("%g");
	//axisY->setBase(8.0);
	axisY->setMinorTickCount(-1);
	m_chart->addAxis(axisY, Qt::AlignLeft);
	//series->attachAxis(m_axisX);
	series->attachAxis(axisY);
	axisY->setLinePenColor(series->pen().color());
	axisY->setLabelsColor(series->pen().color());
}

void n2D::setYLogarithmisch()
{
	static bool binLogarithmisch = false;
	toggleBit(binLogarithmisch);
	// Remove attachedAxes

	foreach (QLineSeries *series, m_series) {
		qDebug() << "series->name() =" << series->name();
		bool isVisible = series->isVisible();
		//series->detachAxis(series->attachedAxes().last());
		delete series->attachedAxes().last();
		//m_chart->removeSeries(series);
		if (!binLogarithmisch)
			addAxisYlinear(series);
		else
			addAxisYlogarithmisch(series);
		series->setVisible(isVisible);
		series->attachedAxes().last()->setVisible(isVisible);
	}
	qDebug() << "setYLogarithmisch(), binLogarithmisch = " << binLogarithmisch;
}

void n2D::removeHiddenSeries()
{
	// Remove last series from chart
	foreach (QLineSeries *series, m_series) {
		if (!series->isVisible())
		{
			delete series->attachedAxes().last();
			m_chart->removeSeries(series);
			m_series.removeOne(series);
			m_coordListY.last()->setText("");
			m_coordListY.removeLast();
		}
	}
	// richtig durchnummerrieren:
	int n = 1;
	foreach (QLineSeries *series, m_series) {
		QString name = series->name();
		int pos = name.lastIndexOf(QChar('('));
		name = name.left(pos);
		series->setName(QString("%1 (%2)").arg(name).arg(n++));
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

void n2D::setMinMaxXAchse()
{
	QList<QPointF> p = m_series.at(0)->points();
	m_axisX->setRange(p.first().rx(), p.last().rx()); // geht leider nicht
	qDebug() << QString("n2D::setMinMaxXAchse(), (%1,%2)").arg(p.first().rx()).arg(p.last().rx());
}

void n2D::setTheme()
{
	toggleBit(m_binDark);
	QPalette pal = window()->palette();
	if (m_binDark) {
		/*m_chartView->*/chart()->setTheme(QChart::QChart::ChartThemeDark);
		pal.setColor(QPalette::Window, QRgb(0x121218));
		pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
	}
	else
	{
		/*m_chartView->*/chart()->setTheme(QChart::ChartThemeLight);
		pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
		pal.setColor(QPalette::WindowText, QRgb(0x404044));
	}
	 window()->setPalette(pal);
	qDebug() << QString("n2D::setTheme(), binDark = %1").arg(m_binDark);
}








