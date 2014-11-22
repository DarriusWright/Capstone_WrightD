import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components
import "../QMLComponents/componentCreation.js" as MyScript

Rectangle
{
	id : transparentControl;
	color : "#3E393A";

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
		step : 0.1;
		text: "Index Of Refraction";
		//width : 100;
		height :100;
		anchors.top: transparentColor.bottom;

		onSliderValueChanged :(function() {
			indexOfRefractionChanged( value);
		})
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