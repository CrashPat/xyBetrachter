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
private:
	void erstelle_n2D();

	n2D *n2d;
	QList<QLineSeries *> listLineSeries;
	QList<QString> nameListBox;
};
#endif // MAINWINDOW_H
