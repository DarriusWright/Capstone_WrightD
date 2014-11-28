import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components
import "../QMLComponents/componentCreation.js" as MyScript

Rectangle
{
	id : diffuseControl;
	signal diffuseColorChanged(real x,real y,real z)

	
	function setColor(x,y,z)
	{
		diffuseColor.setColor(x,y,z);
	}

	anchors.topMargin : 50;
	height : 50

	Component.onCompleted :
	{
		diffuseColor.colorChanging.connect(diffuseColorChanged);
	}

	Components.ColorControl
	{
		id : diffuseColor
		text: "Color"
	}
}