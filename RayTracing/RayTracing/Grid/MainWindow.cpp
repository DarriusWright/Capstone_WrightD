#include "MainWindow.h"
#include <QtQuick\qquickview.h>
#include <QtQml\qqml.h>
#include <QtQml\qqmlengine.h>
#include <QtQml\qqmlfile.h>
#include <QtQml\qjsengine.h>
#include <QtQml\qqmlscriptstring.h>



MainWindow::MainWindow(RenderWindow * render) : renderer(render),
	leftComponent(&engine, QUrl::fromLocalFile("left")),
	rightComponent(&engine, QUrl::fromLocalFile("right"))
{
	initializeViews();
	connectSignals();

	mainWidget = new QWidget();
	QHBoxLayout * mainLayout = new QHBoxLayout();
	QVBoxLayout * tabLayout = new QVBoxLayout();
	pointLightRadius = new FloatSlider("Radius",0.0f, 100.0f,&renderer->lights[0].direction.s[3],1000.0f);
	direction = new Float3Slider("Direction", glm::vec3(-1.0f), glm::vec3(1.0f), &renderer->lights[0].direction.s[0],glm::vec3(20.0f));
	position = new Float3Slider("Position", glm::vec3(-50.0f), glm::vec3(50.0f), &renderer->lights[0].position.s[0],glm::vec3(1000.0f));
	//ambient = new Float3Slider("Ambient", glm::vec3(0.0f), glm::vec3(1.0f), &renderer->lights[0].material.ambient.s[0], glm::vec3(100.0f));
	//diffuse = new Float3Slider("Diffuse", glm::vec3(0.0f), glm::vec3(1.0f), &renderer->lights[0].material.diffuse.s[0], glm::vec3(100.0f));
	//specular = new Float3Slider("Specular", glm::vec3(0.0f), glm::vec3(1.0f), &renderer->lights[0].material.specular.s[0], glm::vec3(100.0f));
	//specularPower = new FloatSlider("Specular Power", 0.0f,200.0f, &renderer->lights[0].material.specular.s[3], 1000.0f);
	sampling = new IntSlider("Samples Squared", 1, 6, &renderer->sampleSquared);
	cameraLookAt = new Float3Slider("Look At", glm::vec3(-10.0f), glm::vec3(10.0f), &renderer->camera.lookAt[0],glm::vec3(200.0f));
	cameraDistance = new FloatSlider("Distance", 0.0f, 2000.0f,&renderer->camera.distance, 10000.0f);
	cameraFocalDistance = new FloatSlider("Focal Distance", 0.0f, 2000.0f,&renderer->camera.focalDistance, 10000.0f);
	cameraRadius  = new FloatSlider("Radius", 0.0f, 1.0f,&renderer->camera.position[3], 10.0f);

	//softShadows =  new QCheckBox("Soft Shadows");
	numberOfBounces = new IntSlider("Number Of Bounces",0, 10, &renderer->maxDepth);
	numberOfReflections = new IntSlider("Number Of Reflections",1, 10, &renderer->numberOfReflections);
	numberOfRefractions = new IntSlider("Number Of Refractions",1, 10, &renderer->numberOfRefractions);

	connect(sampling,&IntSlider::valueChanged,renderer,&RenderWindow::setSamples);
	connect(numberOfBounces,&IntSlider::valueChanged,renderer,&RenderWindow::setMaxBounce);

	lightSelection = new QComboBox();
	lightSelection->addItem("Directional");
	lightSelection->addItem("PointLight");

	connect(lightSelection, SIGNAL(currentIndexChanged(int)), this,SLOT(changeLightType(int)));


	cameraChoice = new QComboBox();
	cameraChoice->addItem("Pinhole");
	cameraChoice->addItem("Thinlens");
	cameraChoice->addItem("Fisheye");
	cameraChoice->addItem("Spherical");
	cameraChoice->addItem("Stereo");
	
	connect(cameraChoice,  SIGNAL(currentIndexChanged(int)), this,SLOT(changeCameraType(int)));


	optimizationSelection  = new QComboBox();
	optimizationSelection->addItem("Grid");
	optimizationSelection->addItem("Octree");

	QTabWidget * tabs = new QTabWidget();
	tabs->setTabShape(QTabWidget::TabShape::Triangular);
	tabLayout->addWidget(tabs);
	QWidget * lightsWidget = new QWidget();
	QWidget * rayWidget = new QWidget();
	QWidget * cameraWidget = new QWidget();
	QWidget * optimizationWidget = new QWidget();

	QVBoxLayout * lightLayout = new QVBoxLayout();
	QVBoxLayout * rayLayout = new QVBoxLayout();
	QVBoxLayout * cameraLayout = new QVBoxLayout();
	QVBoxLayout * optimizationLayout = new QVBoxLayout();

	rayWidget->setLayout(rayLayout);
	rayLayout->addWidget(sampling);
	//rayLayout->addWidget(softShadows);
	rayLayout->addWidget(numberOfBounces);
	//rayLayout->addWidget(numberOfReflections);
	//rayLayout->addWidget(numberOfRefractions);

	cameraWidget->setLayout(cameraLayout);
	//cameraLayout->addWidget(cameraChoice);
	cameraLayout->addWidget(cameraLookAt);
	cameraLayout->addWidget(cameraDistance);
	//cameraLayout->addWidget(cameraFocalDistance);
	//cameraLayout->addWidget(cameraRadius);

	//lightLayout->addWidget(lightSelection);
	//lightLayout->addWidget(direction);
	lightLayout->addWidget(position );
	//lightLayout->addWidget(pointLightRadius);
	//lightLayout->addWidget(ambient);
	//lightLayout->addWidget(diffuse);
	//lightLayout->addWidget(specular);
	//lightLayout->addWidget(specularPower);


	optimizationWidget->setLayout(optimizationLayout);
	optimizationLayout->addWidget(optimizationSelection);

	lightsWidget->setLayout(lightLayout);
	lightsWidget->setMinimumWidth(450);
	tabs->addTab(cameraWidget, "Camera");
	tabs->addTab(lightsWidget, "Light");
	tabs->addTab(optimizationWidget, "Optimization");
	tabs->addTab(rayWidget, "Ray");
	//mainLayout->addLayout(tabLayout);
	mainLayout->addWidget(leftContainer);

	rayLayout->setContentsMargins(0,0,0,0);
	rayLayout->setSpacing(0);
	rayLayout->setMargin(0);


	lightLayout->setContentsMargins(0,0,0,0);
	lightLayout->setSpacing(0);
	lightLayout->setMargin(0);

	tabLayout->setContentsMargins(0,0,0,0);
	tabLayout->setSpacing(0);
	tabLayout->setMargin(0);

	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->setSpacing(0);
	mainLayout->setMargin(0);
	//mainLayout->addLayout(featuresLayout);
	mainLayout->addWidget(renderer);
	mainLayout->addWidget(rightContainer);
	mainWidget->setLayout(mainLayout);
	setCentralWidget(mainWidget);
	//mainLayout->addWidget(renderer);
	//mainLayout->addWidget(mainWidget);
	//rightContainer->setLayout(mainLayout);
	
	//setCentralWidget(mainWidget);
	//QHBoxLayout * l = new QHBoxLayout();
	//l->addWidget(mainWidget);
	//rightContainer->setLayout(l);
	//setCentralWidget(rightContainer);
	//addMenus();
	connect(&updateTimer,&QTimer::timeout,this, &MainWindow::updateWindow);
	//connect(lightSelection, &QComboBox::currentIndexChanged,renderer,  &RenderWindow::changeLightType);

	//lightSelection->currentIndexChanged.connect(renderer->changeLightType);

	//connect(enableShadows, &QCheckBox::stateChanged, renderer, &RenderWindow::setShadowsEnabled);
	//connect(enableReflections, &QCheckBox::stateChanged, renderer, &RenderWindow::setReflectionsEnabled);
	//connect(enableRefractions, &QCheckBox::stateChanged, renderer, &RenderWindow::setRefractionsEnabled);

	updateTimer.start();

	//changeLightType(renderer->lights[0].type);
	//changeCameraType(renderer->camera.type);
}


MainWindow::~MainWindow(void)
{
}


void MainWindow::initializeViews()
{
	QQuickView * rightView = new QQuickView();
	rightContainer = QWidget::createWindowContainer(rightView, this);
	rightView->setSource(QUrl("right.qml"));
	rightContainer->setMinimumSize(200, 200);
	rightContainer->setFocusPolicy(Qt::TabFocus);
	rightQml =rightView->rootObject();


	QQuickView * leftView = new QQuickView();
	leftContainer = QWidget::createWindowContainer(leftView, this);
	leftView->setSource(QUrl("left.qml"));
	leftContainer->setMinimumSize(400, 200);
	leftContainer->setFocusPolicy(Qt::TabFocus);

	rightQml = rightView->rootObject();
	leftQml = leftView->rootObject();

}

void MainWindow::connectSignals()
{
	//connect(leftQml, SIGNAL(changeModelName()), this,SLOT(changeName()));
	connectQMLSignals();
}

void MainWindow::connectQMLSignals()
{
	connect(leftQml, SIGNAL(bounceValueChanged(int)), this , SLOT(changeBounceValue(int)));
	connect(rightQml, SIGNAL(changeModelName()), this,SLOT(changeName()));

}

void MainWindow::changeBounceValue(int value)
{
	renderer->setMaxBounce(value);
}

void MainWindow::changeName()
{
	qDebug() << "Changed Name" ;
}

void MainWindow::changeLightType(int index)
{
	LightType type = (LightType)index;
	lightSelection->setCurrentIndex(index);

	position->setVisible(false);
	direction->setVisible(false);
	pointLightRadius->setVisible(false);


	switch (type)
	{
	case LightType::DIRECTIONAL_TYPE:
		direction->setVisible(true);
		
		break;
	case LightType::POINT_TYPE:
		position->setVisible(true);
		pointLightRadius->setVisible(true);
		break;
	}


	renderer->changeLightType(index);

}
void MainWindow::changeCameraType(int index)
{
	CameraType type = (CameraType)index;

	//cameraDistance->setVisible(false);
	//cameraLookAt->setVisible(false);

	switch (type)
	{
	case CameraType::Pinhole:
		break;
	case CameraType::FishEye:
		break;
	case CameraType::Spherical:
		break;
	case CameraType::Stero:
		break;
	case CameraType::Thinlens:
		break;
	}
	renderer->changeCameraType(index);
}

QAction * MainWindow::addAction(const char * title, int shortCut)
{
	QAction * action = new QAction(tr(title), this);
	if(shortCut != 0 )
		action->setShortcut(shortCut);
	return action;
}
void MainWindow::addMenu(const char * title,QAction * action)
{

}
void MainWindow::addMenus()
{



	fileMenu = menuBar()->addMenu(tr("&File"));
	viewMenu = menuBar()->addMenu(tr("&View"));
	QAction * addObject = addAction("&Add Object");
	connect(addObject,&QAction::triggered,this,&MainWindow::addObject);
	fileMenu->addAction(addObject);

	QAction * addSphere = addAction("&Add Sphere");
	connect(addSphere,&QAction::triggered,this,&MainWindow::addSphere);
	fileMenu->addAction(addSphere);

	QAction * enabledGlobalIllumination = addAction("&Global Illumination");
	QAction * enabledShadows = addAction("&Shadows");
	QAction * enabledReflections = addAction("&Reflections");
	QAction * enabledRefractions = addAction("&Refractions");

	enabledShadows->setCheckable(true);
	enabledReflections->setCheckable(true);
	enabledRefractions->setCheckable(true);
	enabledGlobalIllumination->setCheckable(true);
	enabledShadows->setChecked(renderer->isShadowsEnabled());
	enabledReflections->setChecked(renderer->isReflectionsEnabled());
	enabledRefractions->setChecked(renderer->isRefractionsEnabled());
	enabledGlobalIllumination->setChecked(renderer->globalIlluminationEnabled());

	connect(enabledGlobalIllumination,&QAction::triggered,renderer,&RenderWindow::setGlobalIllumination);
	viewMenu->addAction(enabledGlobalIllumination);


	connect(enabledShadows,&QAction::triggered,renderer,&RenderWindow::setShadowsEnabled);
	viewMenu->addAction(enabledShadows);

	//connect(enabledReflections,&QAction::toggled,renderer,&RenderWindow::setReflectionsEnabled);
	//viewMenu->addAction(enabledReflections);

	//connect(enabledRefractions,&QAction::toggled,renderer,&RenderWindow::setRefractionsEnabled);
	//viewMenu->addAction(enabledRefractions);





}

void MainWindow::addSphere()
{
	renderer->addSphere();
}

void MainWindow::addObject()
{
	std::cout << "adding object" << std::endl;

	QString fileName = QFileDialog::getOpenFileName(this,tr("OpenFile"),"","*.obj");
	if(!fileName.isEmpty())
	{

		std::string stringFile (fileName.toLocal8Bit().constData());
		renderer->addObject(stringFile);
	}

}

void MainWindow::updateWindow()
{

	renderer->updateScene();
	pointLightRadius->update();
	direction->update();
	position->update();
	//ambient->update();
	//diffuse->update();
	//specular->update();
	//specularPower->update();
	cameraLookAt->update();
	cameraDistance->update();
	cameraRadius->update();
	cameraFocalDistance->update();

	//sampling->update();
	//numberOfReflections->update();
	//numberOfRefractions->update();
	setWindowTitle( "Time : " + QString::number(renderer->getInterval()) + "\tFrames : "+ QString::number(renderer->getFPS()));

}