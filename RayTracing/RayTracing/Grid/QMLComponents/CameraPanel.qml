import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components

Rectangle
{

	signal lookAtChanged(real x,real y,real z);
	signal positionChanged(real x,real y,real z);
	signal distanceValueChanged(real value);

	property var setLookAt :(function(x,y,z){ lookAt.setValues(x,y,z) });
	property var setPosition :(function(x,y,z){ position.setValues(x,y,z) });
	property alias distance : distanceSlider.value;
	property alias text : lookAt.text;


	Components.Float3Slider
	{
		id: position;
		height : 50;
		text: "Position : ";
		Component.onCompleted : {
		
			setMin(-40.0,-40.0,-10.0);
			setMax(40.0,40.0,1000.0);
			setStep(1,1,1);
		}

		onSlidersValueChanged : (function()
		{
			positionChanged(position.xValue, position.yValue, position.zValue);
		})

	}


	Components.Float3Slider
	{
		id: lookAt;
		anchors.top : position.bottom;

		height : 50;
		text: "Look At : ";
		Component.onCompleted : {
		
			setMin(-10.0,-10.0,-10.0);
			setMax(10.0,10.0,10.0);
			setStep(1,1,1);
		}

		onSlidersValueChanged : (function()
		{
			lookAtChanged(lookAt.xValue, lookAt.yValue, lookAt.zValue);
		})

	}



	Components.TextSlider
	{
		id : distanceSlider;
		anchors.top : lookAt.bottom;

		min : 0.0;
		max : 1000.0;
		height : 50;
		text : "Distance"

		onSliderValueChanged : (function()
		{
			distanceValueChanged(distanceSlider.value)
			
		})
	}
}
