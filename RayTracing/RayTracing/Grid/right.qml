
import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "QMLComponents" as Components
import "QMLComponents/componentCreation.js" as MyScript

Rectangle{
	signal changeModelName();
	property int type : 0;
	anchors.fill: parent;
	color : "#3E393A";
	function updateType(type)
	{
		panel.destory();
		if(type == 0)
		{
			panel =  MyScript.createObjects("MeshPanel", root);
		}
		else if(type == 1)
		{
			panel =  MyScript.createObjects("LightPanel", root);

		}
		else if(type == 2)
		{
			panel =  MyScript.createObjects("CameraPanel", root);
		}
		panel.parent = panelContainer;
		panel.anchors.fill = panelContainer;
	}



	Rectangle
	{
		id : panelContainer;
		color : "#3E393A";
		anchors.fill: parent
		property var panel : MyScript.createObjects("MeshPanel",this);


	}

	Components.MeshPanel
	{

	}


}

/*
Rectangle {
    id: root
   	anchors.fill: parent;
	color : "#3E393A";

	signal changeModelName(int index, int type);
	signal changeMaterial(int materialType, int index, int type);
	signal changePosition(real x, real y ,real z, int index, int type);
	signal changeDirection(real x, real y ,real z, int index, int type);
	signal changeColor(real x, real y ,real z, int index, int type);
	


	function setUp( index,  type)
	{
		if(type == 0)
		{
		}
		else if(type == 1)
		{

		}
		else if(type == 2)
		{

		}
		updateSliders();
	}

	function updateSliders()
	{
		
	}

	Components.Float3Slider
	{
		id : lightPosition;
		text : "Position";
		Component.onCompleted : 
		{
			
			setMin(-10.0,-10.0,-10.0);
			setMax(10.0,10.0,10.0);
			setStep(1.0,1.0,1.0)
		}
		
	}

	Components.Float3Slider
	{
		id : lightColor;
		text : "Color";
		//y: 200;
		anchors.top : lightPosition.bottom;
		Component.onCompleted : 
		{
			
			setMin(-10.0,-10.0,-10.0);
			setMax(10.0,10.0,10.0);
			setStep(0.1,0.1,0.1)
		}
		
	}

}*/