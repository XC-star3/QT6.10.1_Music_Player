cmake_minimum_required(VERSION 3.16)
project(XC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 支持Ninja构建系统
if(CMAKE_GENERATOR STREQUAL "Ninja")
    message(STATUS "Using Ninja generator")
endif()

# 确保资源被正确嵌入
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

# 让 Qt 自动找 ui 文件目录
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui)

# Qt 模块
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia Network Svg)

# 源文件列表
set(SOURCES
    src/core/main.cpp
    src/ui/mainwindow.cpp
    src/lyrics/lrcwidget.cpp
    src/search/searchwidget.cpp
    src/playlist/playlist_manager.c
    src/playlist/playlist_util.c
    src/playlist/playlist_interface.cpp
)

# UI 文件列表
set(UI_FILES
    ui/mainwindow.ui
    ui/lrcwidget.ui
)

# 资源文件
set(RESOURCE_FILES
    resources/res.qrc
)

# 可执行程序
add_executable(XC
    ${SOURCES}
    ${UI_FILES}
    ${RESOURCE_FILES}
)

# 链接 Qt 库
target_link_libraries(XC PRIVATE    
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Network
    Qt6::Svg  # 添加Svg支持
)

# 设置输出目录和属性
set_target_properties(XC PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    WIN32_EXECUTABLE TRUE  # 创建Windows GUI应用程序（无控制台窗口）
)

# 找到Qt的bin目录
get_target_property(QT6_BIN_DIR Qt6::Core IMPORTED_LOCATION)
get_filename_component(QT6_BIN_DIR "${QT6_BIN_DIR}" DIRECTORY)

# 添加部署目标
add_custom_target(deploy
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/deploy
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:XC> ${CMAKE_BINARY_DIR}/deploy/
    COMMAND "${QT6_BIN_DIR}/windeployqt.exe" --release --no-translations "${CMAKE_BINARY_DIR}/deploy/XC.exe"
    COMMENT "Running windeployqt to deploy Qt dependencies"
    DEPENDS XC
)

# 对于Ninja构建系统，确保依赖关系正确
if(CMAKE_GENERATOR STREQUAL "Ninja")
    add_dependencies(deploy XC)
endif()

# 启用CPack
include(CPack)

# 项目基本信息
set(CPACK_PACKAGE_NAME "XC")
set(CPACK_PACKAGE_VENDOR "Your Name")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simple Music Player using Qt6")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "XC")

# 安装规则 - 使用deploy目录中的内容
install(TARGETS XC
    RUNTIME DESTINATION .
)

# 安装资源文件
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/ DESTINATION resources)

# 如果deploy目录存在，安装其中的所有文件（包含windeployqt生成的依赖）
if(WIN32 AND EXISTS "${CMAKE_BINARY_DIR}/deploy/")
    # 安装deploy目录中的所有DLL文件
    install(DIRECTORY "${CMAKE_BINARY_DIR}/deploy/" DESTINATION .
        PATTERN "XC.exe" EXCLUDE  # 排除可执行文件，由TARGETS安装
    )
    
    # 使用ZIP生成器
    set(CPACK_GENERATOR "ZIP")
    
    # 设置ZIP包的图标和标题
    set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/resources/images/KK.jpg")
    set(CPACK_PACKAGE_VENDOR "XC")
endif()