import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Rectangle
{
	id : textSlider
	property alias min : slider.minimumValue;
	property alias max : slider.maximumValue;
	property alias step : slider.stepSize;
	property string text : "Value";
	property alias value : slider.value;
	property var onSliderValueChanged : (function(){})


	signal valueSliderChanged(int value)

	Component.onCompleted: {
		label.text = text + " : " + slider.value;
		//slider.onValueChanged.connect(valueSliderChanged)
		//valueSliderChanged.connect(onSliderValueChanged);
	}


	Text
	{
		id: label
		text : "1"
		horizontalAlignment : Text.AlignHCenter;
		verticalAlignment : Text.AlignVCenter;
	}

	Slider
	{
	
		id: slider;
		maximumValue : 10.0;
		minimumValue  :0.0;
		updateValueWhileDragging :true;
		anchors.left: label.right
		anchors.leftMargin : 20;
		stepSize: 1.0;
		value : 1.0;
		onValueChanged : 
		{
			label.text =  text + " : " +  slider.value;
			valueSliderChanged(slider.value);
			onSliderValueChanged();
		}
	
	}
}