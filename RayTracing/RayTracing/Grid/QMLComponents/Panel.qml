import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components
Rectangle
{
	property alias title : panelText.text;
	color : "#00000000";

	Rectangle
	{
		width : parent.width;
		height : 50;
		color : "#00000000";

		Text
		{
			text : "Default"
			id: panelText;
			horizontalAlignment : Text.AlignHCenter;
			font.pixelSize : parent.height/2;
		}
	}
}