import QtQuick
import QtQuick.Controls
import MeshPlugin

ApplicationWindow {
    visible: true
    width: 1024
    height: 768
    title: "Mesh Viewer"

    SceneView {
        id: sceneView
        anchors.fill: parent
        focus: true

        cameraInfo.fov: 120.0

        layers: [
            MeshLayer {
                id: meshLayer
                source: Qt.resolvedUrl("/s/apps/users/servantf/mesh.obj")
            },
            ImageLayer {
                id: imageLayer
                source: Qt.resolvedUrl("/s/prods/mvg/_source_global/users/servantf/039_010-src-master01-v002-aces_2065.0977.exr")
                visible: true
            },
            AxisLayer { 

            },
            SphereLayer { 

            }
        ]

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton

            property real initialX: 0
            property real initialY: 0

            onClicked: (mouse) => {
                
            }

            onPressed: (mouse) => {
                initialX = mouse.x
                initialY = mouse.y
            }

            onReleased: (mouse) => {
                sceneView.motionInfo.applyTransform()
            }

            onPositionChanged: (mouse) => {

                const deltaX = mouse.x - initialX
                const deltaY = mouse.y - initialY

                if (mouse.buttons & Qt.LeftButton)
                {
                    sceneView.motionInfo.rotationX = deltaY
                    sceneView.motionInfo.rotationY = deltaX
                }
                else if (mouse.buttons & Qt.MiddleButton)
                {
                    sceneView.motionInfo.planeX = deltaX
                    sceneView.motionInfo.planeY = deltaY
                }
                else if (mouse.buttons & Qt.RightButton)
                {
                    sceneView.motionInfo.distance = deltaY
                }
            }
        }
    }


}