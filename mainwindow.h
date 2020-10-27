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
	void n2DwurdeGesschlossen() { close(); }
	void open_n2D();
	void hilfeDialog();
	void ueberCSVDialog();

private:
	bool findAndPlotAllFiles();
	void erstelle_n2D();
	bool getDataOneFileBin(QString DateiMitPfad);
	bool getDataOneFileCsv(QString DateiMitPfad); // CSV
	bool contains_number(const std::string &c); // für CSV
	void addSeriesSin();
	n2D *n2d = NULL;
	QList<QLineSeries *> n2DSeries;
	QList<xyMinMax> nXYminMax;
	QList<QString> n2DnameList;
	const QString m_pfad = ("C:/Users/Patrik Roth/Downloads/");
};
#endif // MAINWINDOW_H
