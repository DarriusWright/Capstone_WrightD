import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "QMLComponents" as Components

import GameObject 1.0

Rectangle
{
	anchors.fill: parent;
	color : "#3E393A";
//	signal void modelChanged(int index, int type);


	Rectangle
	{
		id : searchBar;
		x: 2;
		y : 10;
		width: search.width;
		height: search.height;
		color: parent.color;
		border.color: "#969696"
		border.width: 2
		radius: 10
		Components.Searchbox
		{
			id : search
		}
	}
	Rectangle
	{
		id: gameObjectUIContainer
		anchors.top : searchBar.bottom;
		anchors.topMargin : 20;
		width : parent.width;
		anchors.leftMargin : 200;
		
		ListView {
			id : gameObjectsListView;
			width: 100; height: 100

			model: gameObject;
			delegate: Rectangle {

				width: gameObjectUIContainer.width;
				height: 20
				color : "#3E393A";
				radius: 3

				Text { 
					text: model.modelData.name 
					
					}
				MouseArea
				{
					anchors.fill: parent
					hoverEnabled : true;
					onEntered : 
					{
						parent.color = "#efefef";
				
					}
					onExited : parent.color = "#3E393A";
				}
			}
		}
	}
	/*
Rectangle
{
	id: gameObjectUIContainer
	anchors.top : searchBar.bottom;
	anchors.topMargin : 20;
    width : parent.width;
	anchors.leftMargin : 200;

Column {
    spacing: 2
	id : gameObjects

    Repeater {
        model: ["Enterprise", "Colombia", "Challenger", "Discovery", "Endeavour", "Atlantis"]

        Rectangle {
            width: gameObjectUIContainer.width;
            height: 20
			color : "#3E393A";
            radius: 3


            Text {
               property int index : index;
                text:  "   "+ modelData
				MouseArea
				{
					anchors.fill: parent
					hoverEnabled : true;
					onEntered : 
					{
						parent.color = "#efefef";
						
					}
					onExited : parent.color = "black";
				}
            }
        }
    }
}
}*/
}