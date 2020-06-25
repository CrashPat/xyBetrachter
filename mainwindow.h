#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "n2d.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();
	void closeEvent(QCloseEvent *event);
private slots:
	void n2DwurdeGesschlossen() {close();}
private:
	void erstelle_n2D(QList<QLineSeries *> &listLineSeries, QList<QString> &nameListGrafen);
	n2D *n2d = NULL;
	QList<QLineSeries *> n2DlistLineSeries;
	QList<QString> n2DnameListBox;
};
#endif // MAINWINDOW_H
