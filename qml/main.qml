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

        cameraInfo.fov: 70.0

        layers: [
            AxisLayer { 

            },
            GridLayer {
                id: gridLayer
                minorFadeStartPixels: 1
                minorFadeEndPixels: 3
                minorOpacity: 0.45
                minorLineWidth: 1.1
            },
            MeshLayer {
                id: meshLayer
                source: Qt.resolvedUrl("/s/prods/mvg/_source_global/users/servantf/meshviewer/mesh.obj")
                picking: true

                onSelectionChanged: {
                    const updatedPositions = sphereLayer.positions.slice()
                    updatedPositions.push(selection)
                    sphereLayer.positions = updatedPositions
                }
            },
            // ImageLayer {
            //     id: imageLayer
            //     source: Qt.resolvedUrl("/s/prods/mvg/_source_global/users/servantf/meshviewer/039_010-src-master01-v002-aces_2065.0977.exr")
            //     visible: true
            // },
            // SfmDataLayer {
            //     id: sfmDataLayer
            //     source: Qt.resolvedUrl("/s/prods/fant/sequence/172/172_020/trk/meshroom/wip/MeshroomCache/SfMColorizing/ca7c10f0b60e81570ab8d5b3135bc71cfbdb91e7/sfmData.abc")
            //     pointSize: 0.02
            //     cameraSize: 0.2
            //     visible: true
            //     picking: true
            // },
            SphereLayer {
                id: sphereLayer
                positions: [
                    Qt.vector3d(0.0, 0.0, 0.0)
                ]
            }
        ]

        // Text {
        //     anchors.top: parent.top
        //     anchors.right: parent.right
        //     anchors.margins: 8
        //     text: Math.round(sceneView.fps) + " fps"
        //     color: "white"
        //     font.pixelSize: 14
        //     style: Text.Outline
        //     styleColor: "black"
        // }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton

            property real initialX: 0
            property real initialY: 0

            onClicked: (mouse) => {
                if (mouse.button === Qt.LeftButton)
                {
                    sceneView.pick(Qt.vector2d(mouse.x, mouse.y))
                }
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
                    sceneView.motionInfo.rotationX = deltaY * 0.5
                    sceneView.motionInfo.rotationY = deltaX * 0.5
                }
                else if (mouse.buttons & Qt.MiddleButton)
                {
                    sceneView.motionInfo.planeX = deltaX * 0.01
                    sceneView.motionInfo.planeY = deltaY * 0.01
                }
                else if (mouse.buttons & Qt.RightButton)
                {
                    sceneView.motionInfo.distance = deltaY * 0.2;
                }
            }
        }
    }


}