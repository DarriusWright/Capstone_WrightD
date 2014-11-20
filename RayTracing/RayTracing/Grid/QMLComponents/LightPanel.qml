import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components


Rectangle 
{

	anchors.fill : parent;
	radius : 5;
	color : "#3E393A";


	Components.Float3Slider
	{
		id : position;
		text : "Position"
		anchors.leftMargin : 20;
		height : 50;
		width : 100;


		Component.onCompleted : 
		{
			setMin(-10.0,-10.0,-10.0);
			setMax(10.0,10.0,10.0);
			setStep(1,1,1);
		}
	}

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
	}
}
