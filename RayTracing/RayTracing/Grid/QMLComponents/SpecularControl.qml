import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components
import "../QMLComponents/componentCreation.js" as MyScript

Rectangle
{
	id : specularControl;
	signal specularColorChanged(real x,real y,real z)

	function setColor(x,y,z)
	{
		specularColor.setColor(x,y,z);
	}


	anchors.topMargin : 50;
	height : 50
	Component.onCompleted :
	{
		specularColor.colorChanging.connect(specularColorChanged);
	}

	Components.ColorControl
	{
		id : specularColor
		text: "Color"
	}


}