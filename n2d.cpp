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
	setRubberBand(QChartView::HorizontalRubberBand); // RectangleRubberBand
	setMouseTracking(true);
	qDebug() << "m_chartView->hasMouseTracking()" << hasMouseTracking();
	setCursor(Qt::CrossCursor);
	setTheme();

	//setRenderHint(QPainter::Antialiasing); //--> macht die Grafik sehr langsam
	//setDragMode(QGraphicsView::NoDrag);

	//![2]
	m_axisX = new QValueAxis;
	connect(m_axisX, SIGNAL(rangeChanged(qreal, qreal)), this, SLOT(controlIfaxisXisOutOfRange(qreal, qreal))); // für HorizontalRubberBand
//	qreal a,b;
//	m_axisX->rangeChanged(a,b);
	//![2]

	// Add few series & Kopie der Werte erstellen
	m_coordX = new QGraphicsSimpleTextItem(m_chart);
	m_coordX->setPos(m_chart->size().width()/2, m_chart->size().height());

	int n = 0;
	foreach (QLineSeries *ls, listLineSeries) {
		// Serie kopieren da sonst die Original Serie abstürzt
		QList<QPointF> vpf = ls->points(); // !!! erzeugt eine reale Kopie nur der Datenpunkte !!!
		QLineSeries    *series = new QLineSeries();
		QScatterSeries *scatSer = new QScatterSeries();
		series->append(vpf);
		scatSer->append(vpf);
		series->setUseOpenGL(true);
		scatSer->setUseOpenGL(true);
		n++;
		series->setName(QString("%2 (%1)").arg(n).arg(ls->name()));
		scatSer->setName(QString("(%1)").arg(n));
		addSeries(series, scatSer);
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
	m_windowTitle = "xyBetrachter: n x 2D               [F1] = Hilfe";
	setWindowTitle(m_windowTitle);
	//m_chart->setTitle("Legendmarker example (click on legend)");
	m_chart->legend()->setVisible(true);
	//m_chart->legend()->setAlignment(Qt::AlignRight);

	// Shortcuts:
	QShortcut *rubberbandUmschaltung = new QShortcut(QKeySequence("Z"), this);
	QObject::connect(rubberbandUmschaltung, SIGNAL(activated()), this, SLOT(setRubberband()));
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
	QShortcut *gridVisebility = new QShortcut(QKeySequence("G"), this);
	QObject::connect(gridVisebility, SIGNAL(activated()), this, SLOT(setGridVisebility()));
	QShortcut *printScreen = new QShortcut(QKeySequence("P"), this);
	QObject::connect(printScreen, SIGNAL(activated()), this, SLOT(makePrintScreen()));
	QShortcut *fullScreen = new QShortcut(QKeySequence("F"), this);
	QObject::connect(fullScreen, SIGNAL(activated()), this, SLOT(showMaximized()));
	QShortcut *miniScreen = new QShortcut(QKeySequence(Qt::Key_Escape), this);
	QObject::connect(miniScreen, SIGNAL(activated()), this, SLOT(showMinimized()));
	QShortcut *dottedGraphs = new QShortcut(QKeySequence("D"), this);
	QObject::connect(dottedGraphs, SIGNAL(activated()), this, SLOT(setDottedGraphs()));
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

void n2D::closeEvent(QCloseEvent *event)
{
	emit fensterGeschlossen();
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
		if (m_series.at(n)->isVisible() | m_scatSer.at(n)->isVisible()) {
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

void n2D::addSeries(QLineSeries *series, QScatterSeries *scatSer)
{
	m_series.append(series);
	m_scatSer.append(scatSer);
	m_chart->addSeries(scatSer); // Extra Reihenfolge vertauscht damit Legende passen angezeigt wird
	m_chart->addSeries(series);

	// Dünnen Grafen zeichnen:
	QPen pen = series->pen();
	pen.setWidth(1);
	pen.setColor(m_farbPalette.at(m_series.length() % m_farbPalette.length()));
	series->setPen(pen);

	scatSer->setMarkerSize(3);
	scatSer->setColor(pen.color());
	//scatSer->setMarkerShape(QScatterSeries::MarkerShapeCircle);

	addAxisYlinear(series, scatSer);
	//addAxisYlogarithmisch(series);
}

void n2D::addAxisYlinear(QLineSeries *series, QScatterSeries *scatSer)
{
	// Achsen anhängen:
	if (m_chart->axes(Qt::Horizontal).length() == 0) {
		m_chart->addAxis(m_axisX, Qt::AlignBottom);
	}
	if (series->attachedAxes().length() == 0) {
		series->attachAxis(m_axisX);
		scatSer->attachAxis(m_axisX);
	}
	QValueAxis *axisY = new QValueAxis;
	//axisY->setTitleText("Values");
	//axisY->setLabelFormat("%g");
	//axisY->setBase(8.0);
	m_chart->addAxis(axisY, Qt::AlignLeft);
	series->attachAxis(axisY);
	scatSer->attachAxis(axisY);
	axisY->setLinePenColor(series->pen().color());
	axisY->setLabelsColor(series->pen().color());
}

void n2D::addAxisYlogarithmisch(QLineSeries *series, QScatterSeries *scatSer)
{
	// Abfragen ob logarithmisch überhaubpt möglich:
	QVector<QPointF> werte = series->pointsVector();
	Q_ASSERT(werte.size() > 0);
	float yMin = std::numeric_limits<float>::max();
	foreach (QPointF p, werte) {
		yMin = qMin((double)yMin, p.y());
	}

	if (yMin <= 0) // logarithmisch nicht möglich?
		addAxisYlinear(series, scatSer);
	else { // logarithmisch
		// Achsen anhängen:
		if (m_chart->axes(Qt::Horizontal).length() == 0) {
			m_chart->addAxis(m_axisX, Qt::AlignBottom);
		}
		if (series->attachedAxes().length() == 0) {
			series->attachAxis(m_axisX);
			scatSer->attachAxis(m_axisX);
		}

		QLogValueAxis *axisY = new QLogValueAxis;
		//axisY->setMinorTickCount(-1); // zusätzliche Hilfslinien
		m_chart->addAxis(axisY, Qt::AlignLeft);
		series->attachAxis(axisY);
		scatSer->attachAxis(axisY);
		scatSer->setColor(series->pen().color());
		axisY->setLinePenColor(series->pen().color());
		axisY->setLabelsColor(series->pen().color());
		//axisY->setMinorGridLineVisible(true); --> geht irgendwie nicht
	}
}

void n2D::setYLinearOrLogarithmisch()
{
	toggleBit(m_binLogarithmisch);
	// Remove attachedAxes

	//m_chart->axes(Qt::Vertical).detach(); // alle y-Achsten löschen
	for (int i = 0; i < m_series.length(); ++i) {
		//qDebug() << "series->name() =" << series->name();
		bool isVisibleAxisY = m_series.at(i)->attachedAxes().last()->isVisible();
		delete m_series.at(i)->attachedAxes().last(); // last ist immer die yAchse --> siehe Konstruktor

		if (m_binLogarithmisch)
			addAxisYlogarithmisch(m_series.at(i), m_scatSer.at(i));
		else
			addAxisYlinear(m_series.at(i), m_scatSer.at(i));

		m_series.at(i)->attachedAxes().last()->setVisible(isVisibleAxisY & m_visibleAxisY);
		m_series.at(i)->attachedAxes().last()->setGridLineVisible(m_visibleGrid);
		m_series.at(i)->pointsReplaced(); // Grafik aktualisieren
		m_scatSer.at(i)->pointsReplaced(); // Grafik aktualisieren
	}
}

void n2D::setRubberband()
{
	toggleBit(m_isRubberbandHorizontal);
	if (m_isRubberbandHorizontal) {
		setRubberBand(QChartView::HorizontalRubberBand); // x-Achse
		setWindowTitle(m_windowTitle);
	}
	else {
		setRubberBand(QChartView::VerticalRubberBand); // y-Achse
		setWindowTitle("[y-AchsenZoom] " + m_windowTitle);
	}
}

void n2D::removeHiddenSeries()
{
	// Remove last series from chart
	for (int i = 0; i < m_series.length(); ++i) {
//		if ( ! (m_series.at(i)->isVisible() | m_scatSer.at(i)->isVisible()) )
		if ( ! m_series.at(i)->isVisible() )
		{
			delete m_series.at(i)->attachedAxes().last();
			m_chart->removeSeries(m_series.at(i));
			m_chart->removeSeries(m_scatSer.at(i));
			m_series.removeOne(m_series.at(i));
			m_scatSer.removeOne(m_scatSer.at(i));
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
	n = 1;
	foreach (QScatterSeries *series, m_scatSer) {
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
		// --> void handleMarkerClicked();
		//		  QObject::disconnect(marker, &QLegendMarker::clicked,
		//									 this, &n2D::handleMarkerClicked);
		//		  QObject::connect(marker, &QLegendMarker::clicked, this, &n2D::handleMarkerClicked);
		QObject::disconnect(marker, SIGNAL(hovered(bool)), this, SLOT(handleMarkerHovered(bool)));
		QObject::connect(marker, SIGNAL(hovered(bool)), this, SLOT(handleMarkerHovered(bool)));
	}
}

void n2D::disconnectMarkers()
{
	 const auto markers = m_chart->legend()->markers();
	 for (QLegendMarker *marker : markers) {
		 QObject::disconnect(marker, SIGNAL(hovered(bool)), this, SLOT(handleMarkerHovered(bool)));
	 }
}

void n2D::handleMarkerHovered(bool darueber)
{
	if (!darueber)
		return;
	QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());
	Q_ASSERT(marker);

	switch (marker->type())
	{
		 case QLegendMarker::LegendMarkerTypeXY:
		 {
   //![5]
			 // Toggle visibility of series
//			marker->series()->setVisible(!marker->series()->isVisible());
			marker->series()->setVisible(!marker->series()->isVisible());
			 //marker->series()->attachedAxes().last()->setVisible(marker->series()->isVisible()); // y-Achse Ein/Aus blenden
			 setYachsenVisebilityForMarker(); // y-Achse Ein/Aus blenden

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

//	 // Autogenerierte Farben ausgeben:
//	 for (int i = 0; i < m_series.length(); ++i) {
//		 qDebug() << m_series.at(i)->pen().color().rgb()
//				  << m_scatSer.at(i)->color().rgb();
//	 }
}

void n2D::setXachseVisebility()
{
	m_axisX->setVisible(!m_axisX->isVisible());
}

void n2D::setYachsenVisebility()
{
	toggleBit(m_visibleAxisY);
	for (int i = 0; i < m_series.length(); ++i) {
		if (!m_visibleAxisY)
			m_series.at(i)->attachedAxes().last()->setVisible( false );
		else if (m_series.at(i)->isVisible() | m_series.at(i)->isVisible())
			m_series.at(i)->attachedAxes().last()->setVisible( true );
	}
}

void n2D::setYachsenVisebilityForMarker()
{
	if (m_visibleAxisY)
	{
		for (int i = 0; i < m_series.length(); ++i) {
			m_series.at(i)->attachedAxes().last()
					->setVisible( m_series.at(i)->isVisible() |
								  m_scatSer.at(i)->isVisible() );
		}
	}
}

void n2D::setGridVisebility()
{
	toggleBit(m_visibleGrid);
	QList<QAbstractAxis *> achsen = m_chart->axes();
	foreach (QAbstractAxis *achse, achsen) {
		if (achse->isVisible()) {// yAchsen
			achse->setGridLineVisible(m_visibleGrid);
		}
	}
}

void n2D::setDottedGraphs()
{
	toggleBit(m_visibleDots);
	for (int i = 0; i < m_series.length(); ++i) {
		bool eineOderBeideSerienSichtbar
				= m_series.at(i)->isVisible() |	m_scatSer.at(i)->isVisible();
		m_series.at(i)->setVisible( eineOderBeideSerienSichtbar & !m_visibleDots );
		m_scatSer.at(i)->setVisible( eineOderBeideSerienSichtbar & m_visibleDots );
		if (m_visibleDots)
			m_scatSer.at(i)->setName(m_series.at(i)->name()+"D");
	}
}

void n2D::keyPressEvent(QKeyEvent *event)
{
	int taste = event->key();
	if((Qt::Key_1 <= taste) & (taste <= Qt::Key_9))
	{
		int nr = taste - Qt::Key_0 - 1;
		if (nr < m_series.length()) {
			const auto markers = m_chart->legend()->markers();
			markers.at(2*nr)->hovered(true);
			markers.at(2*nr+1)->hovered(true);
		}
	}
}

void n2D::wheelEvent(QWheelEvent *event)
{ // https://stackoverflow.com/questions/48623595/scale-x-axis-of-qchartview-using-mouse-wheel
	chart()->zoomReset();

	m_FactorZoom *= event->angleDelta().y() > 0 ? 0.5 : 2;

	QRectF rect = chart()->plotArea();
	QPointF c = chart()->plotArea().center();
	//rect.setWidth(m_FactorZoom*rect.width()); // x-Achse
	rect.setHeight(m_FactorZoom*rect.width()); // y-Achse
	rect.moveCenter(c);
	chart()->zoomIn(rect);

	QChartView::wheelEvent(event);
}
