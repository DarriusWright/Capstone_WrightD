import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components
import "../QMLComponents/componentCreation.js" as MyScript

Rectangle
{
	id : diffuseControl;
	signal diffuseColorChanged(real x,real y,real z)

	Component.onCompleted :
	{
		diffuseColor.colorChanged.connect(diffuseColorChanged);
	}

	Components.ColorControl
	{
		id : diffuseColor
		text: "Color"


	}
}