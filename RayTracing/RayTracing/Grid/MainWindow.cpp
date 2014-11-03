#include "MainWindow.h"


MainWindow::MainWindow(RenderWindow * render) : renderer(render)
{
	//resize(renderer->width(), renderer->height());
	mainWidget = new QWidget();
	QHBoxLayout * mainLayout = new QHBoxLayout();
	QVBoxLayout * tabLayout = new QVBoxLayout();
	radius = 20;
	pointLightRadius = new FloatSlider("Radius",0.0f, 100.0f,&renderer->lights[0].direction.s[3],1000.0f);
	direction = new Float3Slider("Direction", glm::vec3(-1.0f), glm::vec3(1.0f), &renderer->lights[0].direction.s[0],glm::vec3(20.0f));
	position = new Float3Slider("Position", glm::vec3(-50.0f), glm::vec3(50.0f), &renderer->lights[0].position.s[0],glm::vec3(1000.0f));
	ambient = new Float3Slider("Ambient", glm::vec3(0.0f), glm::vec3(1.0f), &renderer->lights[0].material.ambient.s[0], glm::vec3(100.0f));
	diffuse = new Float3Slider("Diffuse", glm::vec3(0.0f), glm::vec3(1.0f), &renderer->lights[0].material.diffuse.s[0], glm::vec3(100.0f));
	specular = new Float3Slider("Specular", glm::vec3(0.0f), glm::vec3(1.0f), &renderer->lights[0].material.specular.s[0], glm::vec3(100.0f));
	specularPower = new FloatSlider("Specular Power", 0.0f,200.0f, &renderer->lights[0].material.specular.s[3], 1000.0f);

	lightSelection = new QComboBox();
	lightSelection->addItem("Directional");
	lightSelection->addItem("PointLight");

	QTabWidget * tabs = new QTabWidget();
	tabs->setTabShape(QTabWidget::TabShape::Triangular);
	tabLayout->addWidget(tabs);
	QWidget * lightsWidget = new QWidget();
	QVBoxLayout * lightLayout = new QVBoxLayout();
	lightLayout->addWidget(lightSelection);
	lightLayout->addWidget(direction);
	lightLayout->addWidget(position );
	lightLayout->addWidget(pointLightRadius);
	lightLayout->addWidget(ambient);
	lightLayout->addWidget(diffuse);
	lightLayout->addWidget(specular);
	lightLayout->addWidget(specularPower);

	lightsWidget->setLayout(lightLayout);
	lightsWidget->setMinimumWidth(450);
	tabs->addTab(lightsWidget, "Light");
	mainLayout->addLayout(tabLayout);

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
	mainWidget->setLayout(mainLayout);
	setCentralWidget(mainWidget);
	addMenus();
	connect(&updateTimer,&QTimer::timeout,this, &MainWindow::updateWindow);
	//connect(lightSelection, &QComboBox::currentIndexChanged,renderer,  &RenderWindow::changeLightType);
	
	//lightSelection->currentIndexChanged.connect(renderer->changeLightType);
	
	//connect(enableShadows, &QCheckBox::stateChanged, renderer, &RenderWindow::setShadowsEnabled);
	//connect(enableReflections, &QCheckBox::stateChanged, renderer, &RenderWindow::setReflectionsEnabled);
	//connect(enableRefractions, &QCheckBox::stateChanged, renderer, &RenderWindow::setRefractionsEnabled);

	updateTimer.start();
}


MainWindow::~MainWindow(void)
{
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

	QAction * enabledShadows = addAction("&Shadows");
	QAction * enabledReflections = addAction("&Reflections");
	QAction * enabledRefractions = addAction("&Refractions");

	enabledShadows->setCheckable(true);
	enabledReflections->setCheckable(true);
	enabledRefractions->setCheckable(true);
	enabledShadows->setChecked(renderer->isShadowsEnabled());
	enabledReflections->setChecked(renderer->isReflectionsEnabled());
	enabledRefractions->setChecked(renderer->isRefractionsEnabled());

	connect(enabledShadows,&QAction::triggered,renderer,&RenderWindow::setShadowsEnabled);
	viewMenu->addAction(enabledShadows);
	enabledShadows->setCheckable(true);

	enabledReflections->setCheckable(true);
	connect(enabledReflections,&QAction::toggled,renderer,&RenderWindow::setReflectionsEnabled);
	viewMenu->addAction(enabledReflections);

	connect(enabledRefractions,&QAction::toggled,renderer,&RenderWindow::setRefractionsEnabled);
	viewMenu->addAction(enabledRefractions);
	enabledRefractions->setCheckable(true);





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
	ambient->update();
	diffuse->update();
	specular->update();
	specularPower->update();

	setWindowTitle( "Time : " + QString::number(renderer->getInterval()) + "\tFrames : "+ QString::number(renderer->getFPS()));

}