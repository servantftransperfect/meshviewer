find_package(assimp REQUIRED)
find_package(embree REQUIRED)
find_package(meshoptimizer REQUIRED)
find_package(AliceVision REQUIRED)

qt_add_qml_module(meshplugin
    URI MeshPlugin
    VERSION 1.0
    PLUGIN_TARGET meshplugin
    NO_PLUGIN_OPTIONAL
    SOURCES
        LayerItem.hpp
        LayerItem.cpp
        MeshLayer.hpp
        MeshLayer.cpp
        ImageLayer.hpp
        ImageLayer.cpp
        AxisLayer.hpp
        SphereLayer.hpp
        SceneView.hpp
        SceneView.cpp
        SceneRenderer.hpp
        SceneRenderer.cpp
        SceneState.hpp
        IRenderable.hpp
        MeshRenderable.hpp
        MeshRenderable.cpp
        AxisRenderable.hpp
        AxisRenderable.cpp
        SphereRenderable.hpp
        SphereRenderable.cpp
        SphereGeometry.hpp
        MeshLoader.hpp
        MeshLoader.cpp
        MotionInfo.hpp
        MotionInfo.cpp
        MeshPicker.hpp
        MeshPicker.cpp
        CameraInfo.hpp
        CameraInfo.cpp
        AxisGeometry.hpp
        ImageRenderable.hpp
        ImageRenderable.cpp
)

# RHI headers live in GuiPrivate
target_link_libraries(meshplugin PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::GuiPrivate
    Qt6::Quick
    Qt6::ShaderTools
    Qt6::Concurrent
    assimp::assimp
    embree
    meshoptimizer::meshoptimizer
    aliceVision_image
)

# Shaders
qt_add_shaders(meshplugin "mesh_shaders"
    GLSL "460"
    PREFIX "/shaders"
    BASE "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    FILES
        shaders/mesh.vert
        shaders/mesh.frag
        shaders/simpleColor.vert
        shaders/simpleColor.frag
        shaders/instanciedColor.vert
        shaders/imageQuad.vert
        shaders/imageQuad.frag
)

# Output the plugin into a importable directory structure
# QML engine needs :
#   <import_root>/MeshPlugin/qmldir
#   <import_root>/MeshPlugin/libmeshplugin.so
set_target_properties(meshplugin PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/imports/MeshPlugin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/imports/MeshPlugin"
)

# Install the plugin shared library
install(TARGETS meshplugin
    LIBRARY DESTINATION lib/MeshPlugin
    RUNTIME DESTINATION lib/MeshPlugin
)

# Install the generated qmldir
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/qmldir"
    DESTINATION lib/MeshPlugin
)

# Install the compiled shaders
install(DIRECTORY
    "${CMAKE_CURRENT_BINARY_DIR}/.qsb/shaders"
    DESTINATION lib/MeshPlugin
    OPTIONAL
)