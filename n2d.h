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

QT_CHARTS_USE_NAMESPACE

class n2D: public QChartView //QWidget// QDialog
{
	Q_OBJECT
protected:
	void closeEvent(QCloseEvent *event) override {emit fensterGeschlossen();}
	void mouseMoveEvent(QMouseEvent *event) override;
public:
	explicit n2D(QList<QLineSeries *> listLineSeries);
	~n2D();

public slots:
	void removeHiddenSeries();
	void connectMarkers();
	void disconnectMarkers();
	void controlIfaxisXisOutOfRange(qreal min, qreal max);
	void setMinMaxXAchse();
	void setTheme();
	void setXachseVisebility();
	void setYachsenVisebility();
	void setGridVisebility();
	void setAllLegendsVisebility();
	void makePrintScreen() {qDebug() << "makePrintScreen() noch nicht implementiert.";}

	void handleMarkerClicked();

	void close() {delete this;}
	void reOpenSlot() {emit reOpenSignal();}
	void hilfeSlot() {emit hilfeSignal();}
	void setYLogarithmisch();

signals:
	void fensterGeschlossen();
	void reOpenSignal();
	void hilfeSignal();

private:
	void addSeries(QLineSeries *series);
	void addAxisYlinear(QLineSeries *series);
	void addAxisYlogarithmisch(QLineSeries *series);
//	void setRasterXAchse(float rasterX);

	static int countInstances;
	void toggleBit(bool &bit) {bit = !bit;}
	QChart *m_chart;
	QList<QLineSeries *> m_series;

	QValueAxis *m_axisX;

	bool m_binDark = false;
	bool m_visibleGrid = true;

	QGraphicsSimpleTextItem *m_coordX;
	QList<QGraphicsSimpleTextItem *> m_coordListY;

	QGraphicsRectItem *m_xHilfsLinie;
	QGraphicsRectItem *m_yHilfsLinie;
};

#endif // N2D_H
