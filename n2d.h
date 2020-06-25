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

class n2D: public QWidget// QDialog
{
	Q_OBJECT
protected:
	void closeEvent(QCloseEvent *event) override {m_fensterGeschlossen = true;}
public:
	explicit n2D(QList<QLineSeries *> listLineSeries, QList<QString> nameListGrafen);
	~n2D();
	void setMinMaxXAchse(QPointF x);
	void setRasterXAchse(float rasterX);
	void setMinMaxYAchse(QPointF y, int instanzNr);
	bool getFensterGeschlossen() {return m_fensterGeschlossen;}

public slots:
	void addSeries(QLineSeries *series, QString nameGrafen);
	void removeSeries();
	void connectMarkers();
	void disconnectMarkers();
	void setSchriftgroesse(float fsize);

	void handleMarkerClicked();

	void xAchsenBereich(qreal minX, qreal maxX);

signals:
	void changedMinX(double minX);
	void changedMaxX(double minX);
	//void fensterGeschlossen();  // Falls das Fenster n2D selber geschlossen wird, nur hide()!

private:
	static int countInstances;
	QChart *m_chart;
	QList<QLineSeries *> m_series;

	QChartView *m_chartView;
	QGridLayout *m_mainLayout;
	QGridLayout *m_fontLayout;

	//QValueAxis *m_axisX;
	QCategoryAxis *m_axisX;
	QList<QValueAxis *> m_axisYList; // für verschiedene Achsenskalierungen
	bool m_fensterGeschlossen; // wenn auf X von Fenster 'n 2D' gedrück wird
};

#endif // N2D_H
