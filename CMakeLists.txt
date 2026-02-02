cmake_minimum_required(VERSION 3.20)
project(hypersphere LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)

add_executable(hypersphere
        src/main.cpp
        src/MainWindow.h src/MainWindow.cpp
        src/ViewportWidget.h src/ViewportWidget.cpp
        src/ParamPanel.h src/ParamPanel.cpp

        src/core/geo/Geometry.h src/core/geo/Geometry.cpp

        src/core/graph/Node.h src/core/graph/Node.cpp
        src/core/graph/Graph.h src/core/graph/Graph.cpp
        src/core/graph/NodeRegistry.h src/core/graph/NodeRegistry.cpp

        src/core/eval/Cooker.h src/core/eval/Cooker.cpp

        src/core/ops/GridSop.h src/core/ops/GridSop.cpp
        src/core/ops/TransformSop.h src/core/ops/TransformSop.cpp
        src/core/ops/MergeSop.h src/core/ops/MergeSop.cpp
        src/core/ops/NullSop.h src/core/ops/NullSop.cpp
        src/main.cpp
        src/core/geo/Geometry.cpp
        src/core/geo/Geometry.h
        src/core/graph/Node.cpp
        src/core/graph/Node.h
        src/core/graph/Graph.cpp
        src/core/graph/Graph.h
        src/core/graph/NodeRegistry.cpp
        src/core/graph/NodeRegistry.h
        src/core/eval/Cooker.cpp
        src/core/eval/Cooker.h
        src/core/ops/GridSop.cpp
        src/core/ops/GridSop.h
        src/core/ops/TransformSop.cpp
        src/core/ops/TransformSop.h
        src/core/ops/MergeSop.cpp
        src/core/ops/MergeSop.h
        src/core/ops/NullSop.cpp
        src/core/ops/NullSop.h
        src/ViewportWidget.cpp
        src/ViewportWidget.h
        src/ParamPanel.cpp
        src/ParamPanel.h
        src/MainWindow.cpp
        src/MainWindow.h
)

target_include_directories(hypersphere PRIVATE src)

target_link_libraries(hypersphere PRIVATE Qt6::Widgets Qt6::OpenGLWidgets)