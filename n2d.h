/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Patrik Roth, 07.03.2019
 *
 * Dient zum Darstellen von allen Datensätzen in einer 2D-Grafik
 *
 * Die Grundstrukturen der Klasse n2D sind aus dem Qt-Beispielen aufgebaut:
 * - "legendmakers"
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef N2D_H
#define N2D_H

//#include <QtCharts/QChartGlobal>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtWidgets/QWidget>
#include <QDialog>
#include <QtWidgets/QGraphicsWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGraphicsGridLayout>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QtCore/QDebug>
#include <QMouseEvent>
#include <QScatterSeries>
#include <QVector4D>

QT_CHARTS_USE_NAMESPACE

struct SerieMitXYminMax { // Zusammenfassen der min und max Werte der zugöhrigen Serien --> spart Rechenzeit
	QLineSeries* series;
//	float xMin;
//	float xMax;
	float yMin;
	float yMax;
};

class n2D : public QChartView //QWidget// QDialog
{
	Q_OBJECT
protected:
	void closeEvent(QCloseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	//	bool eventFilter(QObject *obj, QEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override; // damit über die Nummerntasten die Marken gesteuert werden
	void wheelEvent(QWheelEvent *event) override;  // für y-AchsenZoome
	qreal m_FactorZoom = 1.0;

public:
	explicit n2D(QList<QLineSeries *> listLineSeries);
	~n2D();

public slots:
	void removeHiddenSeries();
	void connectMarkers();
	void disconnectMarkers();
	void controlIfaxisXisOutOfRange(qreal min, qreal max);
	void setMinMaxXAchse();
	void setMinMaxYAchsen();
	void setMinNullOderMaxYAchsen();
	void setTheme();
	void setXachseVisebility();
	void setYachsenVisebility();
	void setYachsenVisebilityForMarker();
	void setGridVisebility();
	void makePrintScreen() { qDebug() << "makePrintScreen() noch nicht implementiert."; }
	void toggleWindowSize() {if(isMaximized()) {showNormal();} else {showMaximized();}}
	void setDottedGraphs();
	void moveLeftKreuz();
	void moveRightKreuz();
	void setWerteVisebilityAufKreuz();
	void setExponentielleZahlenDarstellung() {toggleBit(m_exponentielleZahlenDarstellung);}
	void setOrtLegende();
	void setLegendenVisebility();

	void handleMarkerHovered(bool);

	void close() { delete this; }
	void reOpenSlot() { emit reOpenSignal(); }
	void hilfeSlot() { emit hilfeSignal(); }
	void ueberQtSlot() { emit ueberQtSignal(); }
	void ueberCSVSlot() { emit ueberCSVSignal(); }
	void setYLinearOrLogarithmisch();
	void setRubberband();

signals:
	void fensterGeschlossen();
	void reOpenSignal();
	void hilfeSignal();
	void ueberQtSignal();
	void ueberCSVSignal();

private:
	void addSeries(QLineSeries *series, QScatterSeries *scatSer);
	void addAxisYlinear(QLineSeries *series, QScatterSeries *scatSer);
	void addAxisYlogarithmisch(QLineSeries *series, QScatterSeries *scatSer);
	QPointF getYMinMax(QLineSeries *series); // rx = min, ry = max
	QPointF getYMinMaxFromAllSeries(); // rx = minAllerY-Werte, ry = maxAllerY-Werte
	void setKreuzMitXYWerten(QPoint position, QString richtung = "keine");

	static int countInstances;
	void toggleBit(bool &bit) { bit = !bit; }
	QChart *m_chart;
	QList<QLineSeries *> m_series;
	QList<QScatterSeries *> m_scatSer;
	QValueAxis *m_axisX;
	bool m_binLogarithmisch = false;
	bool m_binDark = false;
	bool m_visibleGrid = true;
	bool m_visibleAxisY = true;
	bool m_visibleDots = false;
	bool m_visibleDotsFirstUse = true;
	bool m_visibleWerteAufKreuz = true;
	bool m_visibleLegenden = true;
	bool m_exponentielleZahlenDarstellung = false;
	bool m_isRubberbandHorizontal = true;
	int m_ortLegende = 1;
	QString m_windowTitle;
//	QList<QColor> m_farbPalette = {Qt::gray, Qt::red, Qt::green, Qt::blue, Qt::yellow, Qt::magenta, Qt::cyan};
		QList<QColor> m_farbPalette = {4288268883, 4280328159, 4285358037, 4294354469, 4290730302, 4282156199, 4281904491, 4286283660, 4293625879}; //--> Autogeneriert
//								  = {0xff99ca53F, 0xff209fdfF, 0xff6d5fd5F, 0xfff6a625F, 0xffbf593eF, 0xff38ad6bF, 0xff7b7f8cF, 0xff3c84a7F}; das gleich nur in Hex

	QGraphicsTextItem *m_coordXatUnten;
	QGraphicsTextItem *m_coordXatGraf;
	QList<QGraphicsTextItem *> m_coordListYatUnten; // links unten
	QList<QGraphicsTextItem *> m_coordListYatGraf;
	QList<QGraphicsRectItem *> m_coordListStricheAtYAxes;

	QGraphicsRectItem *m_xHilfsLinie;
	QGraphicsRectItem *m_yHilfsLinie;
};

#endif // N2D_H
