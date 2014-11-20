import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components
import "../QMLComponents/componentCreation.js" as MyScript

Rectangle
{
	id : transparentControl;
	color : "#3E393A";
	x : 0;
	y : 0;
	anchors.leftMargin : 200;
	signal transparentColorChanged(real x,real y,real z)
	signal indexOfRefractionChanged(real value)

	Component.onCompleted :
	{
		transparentColor.colorChanged.connect(transparentColorChanged);
	}

	Components.TextSlider
	{
		color : "#3E393A";

		id : refractionIndex;
		min : 0.0;
		max : 3.0;
		text: "Index Of Refraction";
		//width : 100;
		height :100;
		anchors.top: transparentColor.bottom;

		onSliderValueChanged : 
		{
			indexOfRefractionChanged( value);
		}
	}

	Components.ColorControl
	{
		color : "#3E393A";
		width : 100;
		height :100;
		anchors.topMargin : 30;
		y: 100;
		id : transparentColor
		text: "Color"
	}




}