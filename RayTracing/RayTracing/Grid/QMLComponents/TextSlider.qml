import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Rectangle
{
	id : textSlider
	anchors.leftMargin : 60;
	anchors.rightMargin : 600;
	property alias min : slider.minimumValue;
	property alias max : slider.maximumValue;
	property alias step : slider.stepSize;
	property string text : "Value";
	property alias value : slider.value;
	property var onSliderValueChanged : (function(){})
	property alias sliderWidth : slider.width;

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
		anchors.leftMargin : 300;
		anchors.rightMargin : 300;
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
		anchors.leftMargin : 10;
		
		stepSize: 1.0;
		value : 1.0;
		onValueChanged : 
		{
			label.text =  text + " : " +  slider.value.toFixed(2);
			valueSliderChanged(slider.value);
			onSliderValueChanged();
		}
	
	}
}