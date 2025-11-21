
cmake_minimum_required(VERSION 3.16)

# 设置项目名称和版本
project(TestConstraints VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(Qt5_DIR "C:/Qt/Qt5.12.12/5.12.12/msvc2017_64/lib/cmake/Qt5")
set(EIGEN3_INCLUDE_DIR "C:/Users/liuwb29/Downloads/eigen-3.4.1")
set(BOOST_INCLUDE_DIR "D:/3rdparty/boost_1_89_0")

# 查找所需的Qt模块
find_package(Qt5 COMPONENTS Core Widgets Gui Concurrent REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${BOOST_INCLUDE_DIR})

# 启用自动处理moc、uic、rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 设置源文件
set(SOURCES
    main.cpp
    "MainWindow.cpp"
    "GraphicsView.cpp"
    "GLine.cpp"
    "Connect.cpp"
)

# 设置头文件
set(HEADERS
    "MainWindow.h"
    "GraphicsView.h"
    "GLine.h"
    "Connect.h"
)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/PlaneGCS GCS_SOURCE_FILES)


# 创建可执行文件
add_executable(TestConstraints
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
    ${FORMS}
    ${GCS_SOURCE_FILES}
)

# 链接Qt库
target_link_libraries(TestConstraints Qt5::Core Qt5::Widgets)

# 使用静态链接
#set(CMAKE_EXE_LINKER_FLAGS "-static")

# 设置目标属性
target_compile_definitions(TestConstraints PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
