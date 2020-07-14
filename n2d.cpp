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
#include <QFont>

QT_CHARTS_USE_NAMESPACE

int n2D::countInstances = 0;

n2D::n2D(QList<QLineSeries *> listLineSeries)
{
	qDebug() << "n2D(): Anzahl Instanzen" << ++countInstances;

	// Create chart view with the chart
	m_chart = new QChart();
	setChart(m_chart);
	setRubberBand(QChartView::HorizontalRubberBand); //
	setMouseTracking(true);
	qDebug() << "m_chartView->hasMouseTracking()" << hasMouseTracking();
	setMouseTracking(true);
	this->setCursor(Qt::CrossCursor);
	this->setTheme();

	//setRenderHint(QPainter::Antialiasing); //--> macht die Grafik sehr langsam
	//setDragMode(QGraphicsView::NoDrag);

	//![2]
	m_axisX = new QValueAxis;
	connect(m_axisX, SIGNAL(rangeChanged(qreal, qreal)), this, SLOT(controlIfaxisXisOutOfRange(qreal, qreal))); // für HorizontalRubberBand
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

	m_xHilfsLinie = new QGraphicsRectItem(10,10,0,100,m_chart);
	m_yHilfsLinie = new QGraphicsRectItem(10,10,100,0,m_chart);
	QPen pen;
	pen.setColor(Qt::darkGray);
	pen.setWidthF(0.2);
	m_xHilfsLinie->setPen(pen);
	m_yHilfsLinie->setPen(pen);
	pen.setWidthF(0.5);
	m_coordX->setPen(pen);

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
	QObject::connect(yLogarithmisch, SIGNAL(activated()), this, SLOT(setYLinearOrLogarithmisch()));
	QShortcut *theme = new QShortcut(QKeySequence("T"), this);
	QObject::connect(theme, SIGNAL(activated()), this, SLOT(setTheme()));
	QShortcut *xMinMax = new QShortcut(QKeySequence("M"), this);
	QObject::connect(xMinMax, SIGNAL(activated()), this, SLOT(setMinMaxXAchse()));
	QShortcut *xAchse = new QShortcut(QKeySequence("X"), this);
	QObject::connect(xAchse, SIGNAL(activated()), this, SLOT(setXachseVisebility()));
	QShortcut *yAchsen = new QShortcut(QKeySequence("Y"), this);
	QObject::connect(yAchsen, SIGNAL(activated()), this, SLOT(setYachsenVisebility()));
	QShortcut *gridVisebility = new QShortcut(QKeySequence("H"), this);
	QObject::connect(gridVisebility, SIGNAL(activated()), this, SLOT(setGridVisebility()));
	QShortcut *allLegendsVisebility = new QShortcut(QKeySequence("G"), this);
	QObject::connect(allLegendsVisebility, SIGNAL(activated()), this, SLOT(setAllLegendsVisebility()));
	QShortcut *printScreen = new QShortcut(QKeySequence("P"), this);
	QObject::connect(printScreen, SIGNAL(activated()), this, SLOT(makePrintScreen()));
	QShortcut *fullScreen = new QShortcut(QKeySequence("F"), this);
	QObject::connect(fullScreen, SIGNAL(activated()), this, SLOT(showMaximized()));
	// --> Hilfetext nachtragen in MainWindow::hilfeDialog();

	this->setGridVisebility();
}

n2D::~n2D()
{
	//delete m_chart;
	//delete m_axisX;
	//delete m_mainLayout;
	qDebug() << "~n2D(): Anzahl Instanzen" << --countInstances;
}

void n2D::mouseMoveEvent(QMouseEvent *event)
{
	qreal breite = m_chart->size().width()-60;
	qreal hoehe = m_chart->size().height()-20;

	qreal xPosMaus = m_chart->mapToValue(event->pos()).x();
	qreal xPosFirst = m_series.first()->points().first().x();
	qreal xPosLast = m_series.first()->points().last().x();
	qreal yPosMaus = /*m_chart->mapToValue*/(event->pos()).y();

	bool istXWertInnerhalb = false;
	if ( (xPosFirst <= xPosMaus) & (xPosMaus <= xPosLast)) // Schauen ob die Werte auch im Bereich liegen, sont gibts Arrayüberlauf bei ..points().at(xPosMaus)..
		istXWertInnerhalb = true;
	bool istYWertInnerhalb = false; //(hoehe-75-30)
	if ( ( 75 <= yPosMaus) & (yPosMaus <= (hoehe-30))) // Schauen ob die Werte auch im Bereich liegen, sont gibts Arrayüberlauf bei ..points().at(xPosMaus)..
		istYWertInnerhalb = true;

	/// xyWerte als Text unten anzeigen:
	// x:
	m_coordX->setPos(breite, hoehe);
	if (istXWertInnerhalb) {
		qreal xAchsenWert = m_series.first()->points().at(xPosMaus).x();
		m_coordX->setText(QString("X:%1").arg(xAchsenWert));
	}
	else
		m_coordX->setText(QString("X:%1").arg("ausser."));
	// y:
	int rechtsVersatz = 0;
	for (int n = m_coordListY.count()-1; n+1 ; --n) {
		int versatz = rechtsVersatz*80+10; // Textabstand
		m_coordListY.at(n)->setPos(versatz, hoehe);
		if (m_series.at(n)->isVisible()) {
			++rechtsVersatz;
			m_coordListY.at(n)->setPen(m_series.at(n)->pen());
			if (istXWertInnerhalb) // Schauen ob die Werte auch im Bereich liegen, sont gibts Arrayüberlauf bei ..points().at(xPos)..
				m_coordListY.at(n)->setText(QString("%1:%2").arg(n+1).arg( m_series.at(n)->points().at(xPosMaus).y() )); // yWert ausgeben
			else
				m_coordListY.at(n)->setText(QString("%1:%2").arg(n+1).arg("ausserhalb")); // yWert ausgeben
			//m_coordListY.at(n)->setText(QString("%1:%2").arg(n+1).arg(m_chart->mapToValue(event->pos(), m_series.at(n)).y())); // Mausposition in Grafik ausgeben
		}
		else
			m_coordListY.at(n)->setText("");
	}

	/// Hilslinien bei Mausposition zeichnen:
	if (istXWertInnerhalb & istYWertInnerhalb) {
		QPoint pos = event->pos();
		m_xHilfsLinie->setRect(pos.x(),75,0, hoehe-75-30);
		m_xHilfsLinie->setVisible(true);
		m_yHilfsLinie->setRect(30,pos.y(),   breite-17,0);
		m_yHilfsLinie->setVisible(true);
	}
	else {
		m_xHilfsLinie->hide();
		m_yHilfsLinie->hide();
	}

	QChartView::mouseMoveEvent(event); // muss weiter gereicht werden sonst geht Rubberband nicht
}

void n2D::addSeries(QLineSeries *series)
{
	 m_series.append(series);
	 m_chart->addSeries(series);

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
	if (m_chart->axes(Qt::Horizontal).length() == 0)
		m_chart->addAxis(m_axisX, Qt::AlignBottom);
	QValueAxis *axisY = new QValueAxis;
	//axisY->setTitleText("Values");
	//axisY->setLabelFormat("%g");
	//axisY->setBase(8.0);
	m_chart->addAxis(axisY, Qt::AlignLeft);
	series->attachAxis(m_axisX);
	series->attachAxis(axisY);
	axisY->setLinePenColor(series->pen().color());
	axisY->setLabelsColor(series->pen().color());
}

void n2D::addAxisYlogarithmisch(QLineSeries *series)
{
	// Achsen anhängen:
	if (m_chart->axes(Qt::Horizontal).length() == 0)
		m_chart->addAxis(m_axisX, Qt::AlignBottom);
	QLogValueAxis *axisY = new QLogValueAxis;
	//axisY->setMinorTickCount(-1); // zusätzliche Hilfslinien
	m_chart->addAxis(axisY, Qt::AlignLeft);
	series->attachAxis(axisY);
	axisY->setLinePenColor(series->pen().color());
	axisY->setLabelsColor(series->pen().color());
}

void n2D::setYLinearOrLogarithmisch()
{
	static bool binLogarithmisch = false;
	toggleBit(binLogarithmisch);
	// Remove attachedAxes

	foreach (QLineSeries *series, m_series) {
		//qDebug() << "series->name() =" << series->name();
		bool isVisible = series->isVisible();
		//series->attachedAxes().last()->;
		delete series->attachedAxes().last();
		if (!binLogarithmisch)
			addAxisYlinear(series);
		else {
//			if (series->attachedAxes(Qt::Horizontal)
			addAxisYlogarithmisch(series);
		}
//		series->setVisible(isVisible);
//		series->attachedAxes().last()->setVisible(isVisible);
	}
	qDebug() << "todo n2D::setYLogarithmisch(): geht bei nur einem Grafen nicht.";
	qDebug() << "todo n2D::setYLogarithmisch(): negativer Logarithmus geht nicht, deshalb alle Grafen mit negative y-Wert weiterhin linear anzeigen.";
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

void n2D::controlIfaxisXisOutOfRange(qreal min, qreal max)
{
	QList<QPointF> p = m_series.first()->points();
	if ((min < p.first().rx()) & (p.last().rx() <= max))
		setMinMaxXAchse();
}

void n2D::setMinMaxXAchse()
{
	QList<QPointF> p = m_series.first()->points();
	m_axisX->setRange(p.first().rx(), p.last().rx());
}

void n2D::setTheme()
{
	toggleBit(m_binDark);
	QPalette pal = window()->palette();
	if (m_binDark) {
		chart()->setTheme(QChart::QChart::ChartThemeDark);
		pal.setColor(QPalette::Window, QRgb(0x121218));
		pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
	}
	else
	{
		chart()->setTheme(QChart::ChartThemeLight);
		pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
		pal.setColor(QPalette::WindowText, QRgb(0x404044));
	}
	 window()->setPalette(pal);
	 //qDebug() << QString("n2D::setTheme(), binDark = %1").arg(m_binDark);
}

void n2D::setXachseVisebility()
{
	m_axisX->setVisible(!m_axisX->isVisible());
}

void n2D::setYachsenVisebility()
{
	foreach (QLineSeries *series, m_series) {
		if (series->isVisible())
			series->attachedAxes().last()->setVisible( !series->attachedAxes().last()->isVisible() );
	}
}

void n2D::setGridVisebility()
{
	toggleBit(m_visibleGrid);
	QList<QAbstractAxis *> achsen = m_chart->axes();
	foreach (QAbstractAxis *achse, achsen) {
		if (achse->isVisible()) {// yAchsen
			achse->setGridLineVisible(m_visibleGrid);
			achse->setMinorGridLineVisible(-m_visibleGrid);
		}
	}
	//qDebug() << "n2D::setGridVisebility()";
}

void n2D::setAllLegendsVisebility()
{
	const auto markers = m_chart->legend()->markers();
	foreach (QLegendMarker* marker, m_chart->legend()->markers()) {
		marker->clicked();
	}
}
