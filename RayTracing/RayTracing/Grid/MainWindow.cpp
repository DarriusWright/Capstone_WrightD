#include "MainWindow.h"


MainWindow::MainWindow(RenderWindow * render) : renderer(render)
{
	//resize(renderer->width(), renderer->height());
	mainWidget = new QWidget();
	QHBoxLayout * mainLayout = new QHBoxLayout();
//	QVBoxLayout * featuresLayout = new QVBoxLayout();
//	//featuresWidget = new QWidget();
//	
//	//featuresWidget->setLayout(featuresLayout);
//	featuresLabel = new QLabel("Enable Features");
//	featuresLabel->setContentsMargins(0,0,0,0);
//	featuresLabel->setStyleSheet("text-decoration: underline; font: bold 15px;");
//	//featuresLabel->setMargin(0);
//	enableShadows     = new QCheckBox("Shadows");
//	enableReflections = new QCheckBox("Reflections");
//	enableRefractions = new QCheckBox("Refractions");
//	
//	//enableShadows->setStyleSheet("background-color:green");
//	featuresLabel->setMaximumHeight(40);
//	featuresLayout->addWidget(featuresLabel);
//	featuresLayout->addWidget(enableShadows);
//	featuresLayout->addWidget(enableReflections);
//	featuresLayout->addWidget(enableRefractions, 0, Qt::AlignTop);
//
////	featuresLabel->setStyleSheet("vertical-align: text-top;");
//
//	featuresLayout->setSpacing(0);
//	featuresLayout->setMargin(0);
		

	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->setSpacing(0);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	//mainLayout->addLayout(featuresLayout);
	mainLayout->addWidget(renderer);
	mainWidget->setLayout(mainLayout);
	setCentralWidget(mainWidget);
	addMenus();
	connect(&updateTimer,&QTimer::timeout,this, &MainWindow::updateWindow);
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
	connect(enabledShadows,&QAction::triggered,renderer,&RenderWindow::setShadowsEnabled);
	viewMenu->addAction(enabledShadows);
	enabledShadows->setCheckable(true);


	QAction * enabledReflections = addAction("&Reflection");
	enabledReflections->setCheckable(true);
	connect(enabledReflections,&QAction::toggled,renderer,&RenderWindow::setReflectionsEnabled);
	viewMenu->addAction(enabledReflections);

	QAction * enabledRefractions = addAction("&Refractions");
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
	setWindowTitle( "Time : " + QString::number(renderer->getInterval()) + "\tFrames : "+ QString::number(renderer->getFPS()));

}