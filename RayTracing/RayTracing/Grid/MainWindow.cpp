#include "MainWindow.h"


MainWindow::MainWindow(RenderWindow * render) : renderer(render)
{
	resize(renderer->width(), renderer->height());
	setCentralWidget(renderer);
	addMenus();
	connect(&updateTimer,SIGNAL(timeout()),this,SLOT(updateWindow()));
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
	QAction * addObject = addAction("&Add Object");
	connect(addObject,SIGNAL(triggered()),this,SLOT(addObject()));
	fileMenu->addAction(addObject);

	QAction * addSphere = addAction("&Add Sphere");
	connect(addSphere,SIGNAL(triggered()),this,SLOT(addSphere()));
	fileMenu->addAction(addSphere);
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