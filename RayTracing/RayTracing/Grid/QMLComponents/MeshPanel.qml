import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents/componentCreation.js" as MyScript
import "../QMLComponents/" as Components
Rectangle 
{
	signal indexChanged(int type)
	anchors.fill : parent;
	radius : 5;
	color : "#3E393A";

	id : root;
	function completed(object)
	{
		object.anchors.top = materialUI.anchors.bottom;
	}

	property var materialControls  : MyScript.createObjects("TransparentControl", materialUI , materialUI);

	Component.onCompleted : 
	{
	
	}


	Rectangle 
	{
		id : materialUI;
		
		Text
		{
			id : materialText
			
			anchors.topMargin : 20;
			x : 20;
			y: 20;
			text: "Material : "
			color : "#81ACD8";
			font.pixelSize : 16;
		}
		ComboBox
		{
			id : materialChoice;
			y: 20;
			anchors.left : materialText.right;
			currentIndex : 0;
			property int lastIndex : currentIndex ;
			width : 200;
			model : ["Diffuse", "Specular", "Transparent"]

			onCurrentIndexChanged :{
				 indexChanged(currentIndex);

				 if(currentIndex != lastIndex)
				 {
					materialControls.destroy();
					if(currentIndex == 0)
					{
						//diffuse
						
						materialControls =  MyScript.createObjects("DiffuseControl", root);
						
					}
					else if( currentIndex == 1)
					{
						//specular
						materialControls =  MyScript.createObjects("SpecularControl", root);

					}
					else if(currentIndex == 2)
					{
						//glass
						materialControls =  MyScript.createObjects("TransparentControl", materialUI);

					}
					materialControls.parent = root;
					materialControls.anchors.top = materialChoice.bottom;
					
					
				 }

				 lastIndex = currentIndex;


			}
		}

	}

}