#pragma once

#include <QtWidgets\qmainwindow.h>
#include <QtWidgets\qlayout.h>
#include <QtWidgets\qaction.h>
#include <QtCore\qstring.h>
#include <QtWidgets\qmenu.h>
#include <QtWidgets\qmenubar.h>
#include <QtWidgets\qdialog.h>
#include <QtWidgets\qfiledialog.h>
#include <QtCore\qdebug.h>
#include "RenderWindow.h"
#include <string>
class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(RenderWindow * render);
	~MainWindow(void);
	


private:

	RenderWindow * renderer;
	QMenu * fileMenu;
	QAction * addAction(const char * title, int shortCut= 0);
	void addMenu(const char * title,QAction * action);
	void addMenus();

	QTimer updateTimer;

private slots:
	void addObject();
	void addSphere();
	void updateWindow();
	//void renderWindow();
};

