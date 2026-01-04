pragma Singleton

import QtQuick

QtObject {
    property color background: "#272b3c"

    property color primary: "#3f4459"
    property color secondary: "#2e3345"

    property color accent: "#383f54"

    property color text_p: "#f0f0f0"
    property color text_s: "#b3b3b3"

    property color shadow: "#1c1f2b"

    ///////////////////
    /// ANIMATIONS ////
    ///////////////////
    Behavior on background {
        ColorAnimation {
            duration: 300
            easing.type: Easing.InOutQuad
        }
    }
    Behavior on primary {
        ColorAnimation {
            duration: 300
            easing.type: Easing.InOutQuad
        }
    }
    Behavior on secondary {
        ColorAnimation {
            duration: 300
            easing.type: Easing.InOutQuad
        }
    }
    Behavior on accent {
        ColorAnimation {
            duration: 300
            easing.type: Easing.InOutQuad
        }
    }
    Behavior on text_p {
        ColorAnimation {
            duration: 300
            easing.type: Easing.InOutQuad
        }
    }
    Behavior on text_s {
        ColorAnimation {
            duration: 300
            easing.type: Easing.InOutQuad
        }
    }
    Behavior on shadow {
        ColorAnimation {
            duration: 300
            easing.type: Easing.InOutQuad
        }
    }
}
