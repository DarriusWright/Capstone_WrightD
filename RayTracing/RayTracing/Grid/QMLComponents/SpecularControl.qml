import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components
import "../QMLComponents/componentCreation.js" as MyScript

Rectangle
{
	id : specularControl;
	signal specularColorChanged(real x,real y,real z)

	Component.onCompleted :
	{
		specularColor.colorChanged.connect(specularColorChanged);
		//specularControl.specularColorChanged.connect()
	}

	Components.ColorControl
	{
		id : specularColor
		text: "Color"

		


	}


}