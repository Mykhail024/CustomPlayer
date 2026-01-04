import QtQuick

Item {
    id: root

    property var loadedObject: null

    function setSourceObject(obj) {
        if (loadedObject === obj) {
            return
        }
        if (loadedObject) {
            loadedObject.parent = null
            loadedObject.visible = false
            loadedObject = null
        }
        if (obj) {
            loadedObject = obj
            loadedObject.parent = root
            loadedObject.visible = true
        }
    }
}
