import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "QMLComponents" as Components



TabView {
    id: root
    width: 400
    height: 640

	signal bounceValueChanged(int value);


	Tab
	{
		id: rayTab
		title : "Ray"


		Rectangle
		{
			Components.TextSlider
			{
				id : bounceSlider;
				text : "Bounces";
				value : 5.0;
				onSliderValueChanged : (function(){
					bounceValueChanged(bounceSlider.value);
				})

			}
		}
	}

	Tab
	{
		id: lightTab
		title: "Light"
		Rectangle
		{
		
		}
	}

	Tab 
	{
		id: cameraTab
		title: "Camera" 
		Rectangle
		{
			color : "blue";
		}
	}
	Tab
	{
		id: optimizationTab
		title : "Optimization"
		Rectangle
		{
			color : "blue";
		}
	}
	Tab
	{
		id: modelTab
		title : "Models"
		
		Rectangle
		{
			color : "blue"

		}
		
	}
}


