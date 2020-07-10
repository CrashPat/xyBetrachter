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
	void closeEvent(QCloseEvent *event) override;
private slots:
	void n2DwurdeGesschlossen() {close();}
	bool open_n2D();
	void hilfeDialog();
private:
	bool findAndPlotAllFiles();
	void erstelle_n2D();
	bool getDataOneFile(QString DateiMitPfad);
	void addSeriesSin();
	n2D *n2d = NULL;
	QList<QLineSeries *> n2DSeries;
	QList<QString> n2DnameList;
	const QString m_pfad = ("C:/Users/Patrik Roth/Downloads/");
};
#endif // MAINWINDOW_H
