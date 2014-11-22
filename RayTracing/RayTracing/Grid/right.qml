
import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "QMLComponents" as Components
import "QMLComponents/componentCreation.js" as MyScript

Rectangle{
	signal changeModelName();
	property int type : 0;
	property int lastType : 0;
	anchors.fill: parent;
	id: root;
	color : "#3E393A";

	signal update(int type);
	
	Component.onCompleted : 
	{
		root.update.connect(updatePanel);
	}

	property var updatePanel : (function(type)
	{
		lightContainer.anim();
		console.log("animation begin");

		/*
		if(type == 0)
		{
			//root.panel =  MyScript.createObjects("MeshPanel", root);
			meshContainer.opacity  = 1;
		}
		else if(type == 1)
		{
			//root.panel =  MyScript.createObjects("LightPanel", root);
			lightContainer.opacity  = 1;

		}
		else if(type == 2)
		{
			//root.panel =  MyScript.createObjects("CameraPanel", root);
			cameraContainer.opacity  = 1;

		}
		*/
	})



	function updateType(type)
	{
	//	cameraContainer.destroy();
		update(type);
	//	console.log(type);

	}


	Rectangle
	{
		id : cameraContainer
		color : "#3E393A";
		anchors.fill: parent;
		opacity : 0;
		Components.CameraPanel
		{

		}
	}

	Rectangle
	{
		id : lightContainer;
		property alias animation : animateOpacity;
		color : "#3E393A";
		anchors.fill: parent;
		opacity : 1;
		property var anim : (function(){animateOpacity.start();console.log(opacity)})

		Components.LightPanel
		{

		}
		     MouseArea {
         anchors.fill: parent
         onClicked: {
            console.log("click animate");
             animateOpacity.start()
         }
     }


		NumberAnimation {
         id: animateOpacity
         target: lightContainer
         properties: "opacity"
         from: 1.0
         to: 0.0
         loops: Animation.Infinite
         easing {type: Easing.OutBack; overshoot: 500}
    }

	}

	Rectangle
	{
		id : meshContainer;
		color : "#3E393A";
		anchors.fill: parent;
		opacity: 0 ;
		Components.MeshPanel
		{

		}

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