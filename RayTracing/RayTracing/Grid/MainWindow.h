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
#include <QtWidgets\qtabwidget.h>
#include "UI\FloatSlider.h"
#include "UI\IntSlider.h"
#include "UI\Float3Slider.h"
#include <QtWidgets\qcombobox.h>
class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(RenderWindow * render);
	~MainWindow(void);
	


private:
	QWidget * mainWidget;
	RenderWindow * renderer;
	QMenu * fileMenu;
	QMenu * viewMenu;
	QAction * addAction(const char * title, int shortCut= 0);
	void addMenu(const char * title,QAction * action);
	void addMenus();
	//QLabel * featuresLabel;
	//QCheckBox * enableShadows;
	//QCheckBox * enableReflections;
	//QCheckBox * enableRefractions;
	QComboBox * lightSelection;
	QComboBox * optimizationSelection;
	QComboBox * sampleType;
	FloatSlider * pointLightRadius;
	FloatSlider * specularPower;
	Float3Slider * direction;
	Float3Slider * position;
	Float3Slider * ambient;
	Float3Slider * diffuse;
	Float3Slider * specular;
	Float3Slider * cameraLookAt;
	FloatSlider * cameraDistance;
	FloatSlider * cameraFocalDistance;
	FloatSlider * cameraRadius;
	QComboBox * cameraChoice;

	IntSlider * sampling;
	QCheckBox * softShadows;
	IntSlider * numberOfRefractions;
	IntSlider * numberOfReflections;



	float radius;
	QTimer updateTimer;

private slots:
	void changeLightType(int);
	void changeCameraType(int);
private:
	void addObject();
	void addSphere();
	void updateWindow();
	//void renderWindow();
};

