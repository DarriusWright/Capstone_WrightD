import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components

Rectangle
{

	signal lookAtChanged(real x,real y,real z);
	signal distanceChanged(real value);

	property alias setLookAt : lookAt.setValues:
	property alias distance : distanceSlider.value;


	Components.Float3Slider
	{
		id: lookAt;
		Component.onCompleted : {
			label.text = text
			setMin(-10.0,-10.0,-10.0);
			setMax(10.0,10.0,10.0);
			setStep(1,1,1);
		}

		onSlidersValueChanged : 
		{
			lootAtChanged(lookAt.xValue, lookAt.yValue, lookAt.zValue);
		}

	}

	Components.TextSlider
	{
		id : distanceSlider;
		min : 0.0;
		max : 1000.0;
		anchors.top = lookAt.bottom;

		onSliderValueChanged : 
		{
			distanceChanged(distanceSlider.value)
		}
	}
}
