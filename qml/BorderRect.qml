import QtQuick

Item {
    id: root
    default property alias data: content.data
    property alias backgroundColor: content.color
    //property alias borderColor: borderRect.border.color
    property int borderLeft: 0
    property int borderRight: 0
    property int borderTop: 0
    property int borderBottom: 0
    property color borderColor
    clip: true
    Rectangle {
        id: content
        anchors {
            fill: parent
            leftMargin: borderLeft
            rightMargin: borderRight
            topMargin: borderTop
            bottomMargin: borderBottom
        }
    }
    Rectangle { width: borderLeft; height: parent.height; color: borderColor }
    Rectangle { width: parent.width; height: borderTop; color: borderColor }
    Rectangle { width: borderRight; height: parent.height; color: borderColor }
    Rectangle { width: parent.width; height: borderBottom; color: borderColor }
    /*Rectangle {
        id: borderRect
        color: "transparent"
        border.width: Math.max(borderLeft, borderRight, borderTop, borderBottom)
        anchors {
            fill: parent
            leftMargin: borderLeft - border.width
            rightMargin: borderRight - border.width
            topMargin: borderTop - border.width
            bottomMargin: borderBottom - border.width
        }
    }*/
}
