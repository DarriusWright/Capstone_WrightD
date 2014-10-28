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
#include <QtWidgets\qlayout.h>
#include <QtWidgets\qcheckbox.h>
#include "RenderWindow.h"
#include <string>
#include <QtWidgets\qlabel.h>
class MainWindow : public QMainWindow
{
	//Q_OBJECT
public:
	MainWindow(RenderWindow * render);
	~MainWindow(void);
	


private:
	QWidget * mainWidget;
	QWidget * featuresWidget;
	RenderWindow * renderer;
	QMenu * fileMenu;
	QMenu * viewMenu;
	QAction * addAction(const char * title, int shortCut= 0);
	void addMenu(const char * title,QAction * action);
	void addMenus();
	QLabel * featuresLabel;
	QCheckBox * enableShadows;
	QCheckBox * enableReflections;
	QCheckBox * enableRefractions;


	QTimer updateTimer;

//private slots:
private:
	void addObject();
	void addSphere();
	void updateWindow();
	//void renderWindow();
};

