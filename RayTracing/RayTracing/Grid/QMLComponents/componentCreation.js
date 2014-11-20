var component;
var sprite;

function createObjects(name, appWindow) {
    component = Qt.createComponent(name + ".qml");
    if (component.status == Component.Ready)
        finishCreation(appWindow);
    else
        component.statusChanged.connect(finishCreation);
}

function finishCreation(appWindow) {
    if (component.status == Component.Ready) {
        sprite = component.createObject(appWindow, { "x": 100, "y": 100 });
        if (sprite == null) {
            // Error Handling
            console.log("Error creating object");
        }
    } else if (component.status == Component.Error) {
        // Error Handling
        console.log("Error loading component:", component.errorString());
    }
}