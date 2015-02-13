
import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "QMLComponents" as Components
import "QMLComponents/componentCreation.js" as MyScript

Rectangle{

	property int type : 0;
	property int lastType : 0;
	anchors.fill : parent;
	id: root;
	objectName :"rootRect"
	
	 gradient: Gradient {
            GradientStop {
                position: 0.0; color: "#3E393A"
            }
            GradientStop {
                position: 1.0; color: "#000000"
            }
    }

	function setCameraLookAt(x, y,z)
	{
		cameraPanel.setLookAt(x,y,z);
	}

	function setCameraPosition(x,y,z)
	{
		cameraPanel.setPosition(x,y,z);
	}

	function setCameraDistance(value)
	{
		cameraPanel.distance = value;
	}

	function setLightPosition(x,y,z)
	{
		lightPanel.setPosition(x,y,z);
		console.log("setLight Position");
	}

	function setMaterialType( type)
	{
		meshPanel.setMaterialType(type);
	}

	function setLightColor(r,g,b)
	{

	}
	
	function setMeshColor(x,y,z)
	{
		meshPanel.setColor(x,y,z);
	}

	function setRefractionIndex(value)
	{
		meshPanel.setRefractionIndex(value);
	}


	
	signal update(int type);
	signal materialChanged(int type)
	signal colorChanged(real x, real y, real z);
	signal lightPositionChanged(real x, real y, real z);
	signal cameraPositionChanged(real x, real y, real z);
	signal cameraLookAtChanged(real x, real y, real z);
	signal cameraDistanceChanged(real value);
	signal indexOfRefractionChanged(real value);
	signal modelChanged(int index, int t);
	signal numberOfBouncesChanged(int change);
	signal numberOfSamplesChanged(int change);
	
	Component.onCompleted : 
	{
		root.modelChanged.connect(updateGameObject);
		meshPanel.indexChanged.connect(materialChanged);
		meshPanel.colorChanged.connect(colorChanged);
		meshPanel.indexOfRefractionChanged.connect(indexOfRefractionChanged);
		lightPanel.positionChanged.connect(lightPositionChanged);
		cameraPanel.positionChanged.connect(cameraPositionChanged);
		cameraPanel.lookAtChanged.connect(cameraLookAtChanged);
		cameraPanel.distanceValueChanged.connect(cameraDistanceChanged);
		rayPanel.numberOfBouncesChanged.connect(numberOfBouncesChanged);
		rayPanel.numberOfSamplesChanged.connect(numberOfSamplesChanged);
	}


	property var updateGameObject : (function(index, type){
			lightContainer.visible = false;
			cameraContainer.visible = false;
			meshContainer.visible = false;

			switch(type)
			{
				case  0 : // mesh
				meshContainer.visible = true;
				break;
				case  1 :  // light
				lightContainer.visible = true;
				break;
				case 2 :  // camera
				cameraContainer.visible = true;
				break;
			}

		}
	)




	function updateType(type)
	{
		update(type);
	}

	Rectangle
	{
		id: gameObjectUIContainer

		anchors.topMargin : 20;
		width : parent.width;
		height : 200;
		anchors.leftMargin : 200;
		color : "#00000000";

		ListView {
			id : gameObjectsListView;
			width: parent.width; height: 100

			clip : true;
			model: gameObject;
			
			focus : true;
			delegate: Rectangle {

				width:  gameObjectUIContainer.width - 10;
				height: 20
				color : ListView.isCurrentItem ?  "#81ACD8": "#00000000";
				radius: 3
				x: 4;
				property alias textColor : nameText.color;
				anchors.leftMargin : 3;
				anchors.rightMargin : 3;

				Text 
				{ 
					id : nameText
					text:  "   "+ model.modelData.name 
					color:(gameObjectsListView.currentIndex == index) ?   "#3E393A" : "#81ACD8" 
					font.pixelSize : 16;
					//color : ""

				}
				MouseArea
				{
					anchors.fill: parent
					hoverEnabled : true;
					onClicked : 
					{
						gameObjectsListView.currentItem.color = "#00000000";
						gameObjectsListView.currentItem.textColor = "#81ACD8";

						parent.color = "#81ACD8"
						parent.textColor = "#3E393A"
						
						gameObjectsListView.currentIndex = index;
						modelChanged(model.modelData.index,model.modelData.type);
						
					}
					onEntered : 
					{
						if(gameObjectsListView.currentIndex != index)
						{
							parent.color = "#efefef";
							nameText.color = "#3E393A";
						}
					}
					onExited : 
					{
						if(gameObjectsListView.currentIndex != index)
						{
							parent.color =  "#00000000";
							nameText.color =  "#81ACD8" 
						}
					}
				}

			}

			
		}
		Components.RayPanel
		{
			id : rayPanel;
			width : parent.fill;
			height : 100;
			anchors.top : gameObjectsListView.bottom;
		}
	}			

	
	Rectangle
	{
		id : cameraContainer
		color : "#00000000";
		anchors.top : gameObjectUIContainer.bottom;
		visible : false;
		height : 100;
		width : parent.width;

		Components.CameraPanel
		{
			id : cameraPanel;
			width : parent.width;
			height : parent.height;
		}
	}
	
		Rectangle
	{
		id : meshContainer;

		objectName : "mesh"
		color : "#00000000";
		anchors.top : gameObjectUIContainer.bottom;
		width : parent.width;
		height : 200;
		//opacity: 1 ;
		visible : false;
		Components.MeshPanel
		{
			id : meshPanel;
			width : parent.width;
			height : parent.height;
		}

	}
	
	Rectangle
	{
		
		
		height : 100;
		id : lightContainer;
		color : "#00000000";
		objectName : "light";
		anchors.top : gameObjectUIContainer.bottom;
		
		width : parent.width ;
		visible : true;
		Components.LightPanel
		{
			id : lightPanel;

		}
	}

	

}

