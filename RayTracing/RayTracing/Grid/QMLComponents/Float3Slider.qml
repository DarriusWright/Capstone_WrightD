import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components


Rectangle
{
	property alias xMin : xSlider.min;
	property alias yMin : ySlider.min;
	property alias zMin : zSlider.min;
						   
	property alias xMax : xSlider.max;
	property alias yMax : ySlider.max;
	property alias zMax : zSlider.max;

	property alias xStep  : xSlider.step;
	property alias yStep  : ySlider.step;
	property alias zStep  : zSlider.step;
	property var onSlidersValueChanged : (function(){})
	property alias text : label.text;


	signal valueSlidersChanged(double x, double y, double z);

	Component.onCompleted : {
		label.text = text
		setMin(-1.0,-1.0,-1.0);
		setMax(1.0,1.0,1.0);
		setStep(0.1,0.1,0.1);

	}


	function setMin( x,  y,  z)
	{
		xSlider.min = x;
		ySlider.min = y;
		zSlider.min = z;
	}

	function setMax( x,  y,  z)
	{
		xSlider.max = x;
		ySlider.max = y;
		zSlider.max = z;	
	}

	function setStep( x,  y , z)
	{
		xSlider.step = x;
		ySlider.step = y;
		zSlider.step = z;
	}
	
	Text
	{
		id: label
		text : "1"
		
		horizontalAlignment : Text.AlignHCenter;
		verticalAlignment : Text.AlignVCenter;
	}



	RowLayout
	{
	anchors.leftMargin : 20;
	anchors.left: label.right
	Components.TextSlider
	{
	anchors.leftMargin : 200;
		anchors.rightMargin : 600;
		id : xSlider;
		text : "x";
		width : 80;
		sliderWidth : 50;

		
		onSliderValueChanged : (function()
		{
			valueSlidersChanged(xSlider.value, ySlider.value, zSlider.value);
		})
	}
	Components.TextSlider
	{
		id : ySlider;
		text : "y";
		width : 80;
		sliderWidth : 50;


		onSliderValueChanged : (function()
		{
			valueSlidersChanged(xSlider.value, ySlider.value, zSlider.value);
		})
	}
	Components.TextSlider
	{
		id : zSlider;
		text : "z";
		width: 80;
		sliderWidth : 50;

		onSliderValueChanged : (function(){
			valueSlidersChanged(xSlider.value, ySlider.value, zSlider.value);
		})
	}
	}
}