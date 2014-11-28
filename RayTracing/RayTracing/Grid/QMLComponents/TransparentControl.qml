import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components
import "../QMLComponents/componentCreation.js" as MyScript

Rectangle
{
	id : transparentControl;
	color : "#00000000";

	signal transparentColorChanged(real x,real y,real z)
	signal indexOfRefractionChanged(real value)


	function setColor(x,y,z)
	{
		transparentColor.setColor(x,y,z);
	}

	function setRefractionIndex(value)
	{
		refractionIndex.value = value;
	}

	Component.onCompleted :
	{
		transparentColor.colorChanging.connected(transparentColorChanged);
		console.log("color changed");

	}

	Components.TextSlider
	{
		color : "#00000000";

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
		color : "#00000000";
		width : 100;
		height :100;
		anchors.topMargin : 30;
		y: 100;
		id : transparentColor
		text: "Color"
	}
}