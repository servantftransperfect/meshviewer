cmake_minimum_required(VERSION 4.0)

project(MeshViewer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Qt requires this for automoc
set(CMAKE_AUTOMOC ON)


find_package(PkgConfig REQUIRED)
pkg_check_modules(XKB REQUIRED IMPORTED_TARGET xkbcommon)

find_package(Qt6 6.8 REQUIRED COMPONENTS
    Core
    Gui
    Quick
    OpenGL
    ShaderTools
)

qt_standard_project_setup()

add_subdirectory(plugin)

# Install Python entry point
install(FILES
    "${CMAKE_SOURCE_DIR}/main.py"
    DESTINATION share/meshviewer
)

# Install QML files
install(DIRECTORY
    "${CMAKE_SOURCE_DIR}/qml"
    DESTINATION share/meshviewer
)

# Install a launcher script that sets paths correctly
configure_file(
    "${CMAKE_SOURCE_DIR}/meshviewer.sh.in"
    "${CMAKE_BINARY_DIR}/meshviewer.sh"
    @ONLY
)

install(PROGRAMS
    "${CMAKE_BINARY_DIR}/meshviewer.sh"
    DESTINATION bin
)