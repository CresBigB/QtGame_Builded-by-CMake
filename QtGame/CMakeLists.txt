# CMakeList.txt: DailyNews 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
cmake_minimum_required(VERSION 3.29)
set(Project_name "QtGame")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS "Forms")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}./bin)
include_directories("./Headers")

file(GLOB Headers "Headers/*.*")
file(GLOB Sources "Sources/*.cpp")
file(GLOB Resources "Resources/*.qrc")
file(GLOB Froms "Forms/*.ui")

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui OpenGLWidgets)

add_executable (${Project_name} ${Sources} ${Headers} ${Resources}  "main.cpp")

target_link_libraries(${Project_name} Qt6::Widgets Qt6::Core Qt6::Gui Qt6::OpenGLWidgets)

#target_compile_definitions(${Project_name} PRIVATE QT_OPENGL_LIB)

# 将源代码添加到此项目的可执行文件。

# 设置生成的 exe 文件名称
set_target_properties(${Project_name} PROPERTIES
    OUTPUT_NAME "${Project_name}"
)
