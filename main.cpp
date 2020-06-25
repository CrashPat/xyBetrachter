#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	MainWindow w;
	// Arguments abfragen

	QStringList args = app.arguments(); // = argv
	//if (2 >= args.length()) // mit Argument
			w.getDataOneFile(args.last());
	// End Arguments
	//w.show(); --> wird aktuell nicht bnötigt
	return app.exec();
}
