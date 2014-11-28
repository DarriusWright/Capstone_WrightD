import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components


Rectangle 
{

	anchors.fill : parent;
	radius : 5;
		color : "#00000000";

	signal positionChanged(real x,real y,real z);
	function setPosition(x,y,z)
	{
		position.setValues(x,y,z);
	}

	Components.Float3Slider
	{
		id : position;
		text : "Position"
		anchors.leftMargin : 20;
		height : 50;
		width : 100;

		focus : true;
		Component.onCompleted : 
		{
			setMin(-40.0,-40.0,-40.0);
			setMax(40.0,40.0,40.0);
			setStep(.1,.1,.1);
		}


			onSlidersValueChanged : (function()
			{
				positionChanged(xValue,yValue,zValue);
			}
		
		)
	}

	/*
	Components.Float3Slider
	{
		id : color;
		text : "Color"
		height : 50;
		anchors.leftMargin : 20;

		width : 100;
		anchors.top : position.bottom;
		Component.onCompleted : 
		{
			setMin(0.0,0.0,0.0);
			setMax(1.0,1.0,1.0);
			setStep(.1,.1,.1);
		}



	}*/
}
