import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "QMLComponents" as Components


Rectangle
{
	property alias xMin : xSlider.min;
	property alias yMin : ySlider.min;
	property alias zMin : zSlider.min;
						   
	property alias xMax : xSlider.max;
	property alias yMax : ySlider.max;
	property alias zMax : zSlider.max;

	property alias xStep  : x.step;
	property var onSlidersValueChanged : (function(){})


	signal valueSlidersChanged(float x, float y, float z)

	Component.onComplete : {
		setMin(-1.0f,-1.0f,-1.0f);
		setMax(1.0f,1.0f,1.0f);
		setStep(0.1f,0.1f,0.1f);
	}


	function setMin(float x, float y, float z)
	{
		xSlider.min = x;
		ySlider.min = y;
		zSlider.min = z;
	}

	function setMax(float x, float y, float z)
	{
		xSlider.max = x;
		ySlider.max = y;
		zSlider.max = z;	
	}

	function setStep(float x, float y ,float z)
	{
		xSlider.step = x;
		ySlider.step = y;
		zSlider.step = z;
	}
	

	Components.TextSlider
	{
		id : xSlider;
		onSliderValueChanged : 
		{
			valueSlidersChanged(xSlider.value, ySlider.value, zSlider.value);
		}
	}
	Components.TextSlider
	{
		id : ySlider;
		onSliderValueChanged : 
		{
			valueSlidersChanged(xSlider.value, ySlider.value, zSlider.value);
		}
	}
	Components.TextSlider
	{
		id : zSlider;
		onSliderValueChanged : 
		{
			valueSlidersChanged(xSlider.value, ySlider.value, zSlider.value);
		}
	}

}