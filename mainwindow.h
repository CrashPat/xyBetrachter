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
	bool getDataOneFile(QString DateiMitPfad);
private slots:
	void n2DwurdeGesschlossen() {close();}
private:
	void addSeriesSin();
	void erstelle_n2D();
	n2D *n2d = NULL;
	QList<QLineSeries *> n2DSeries;
	QList<QString> n2DnameList;
};
#endif // MAINWINDOW_H
