cmake_minimum_required (VERSION 3.8)
project(NukePluginTest)

set(CMAKE_CXX_FLAGS "/arch:AVX /EHsc")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

option(NUKE_11.3 "Nuke 11.3" OFF)
option(NUKE_12.0 "Nuke 12.0" OFF)
option(NUKE_12.1 "Nuke 12.1" ON)
option(NUKE_12.2 "Nuke 12.2" OFF)


if (NUKE_11.3)
    include_directories($("C:/Program Files/Nuke11.3v4/include"))
    link_directories($("C:/Program Files/Nuke11.3v4"))
endif()

if (NUKE_12.0)
    include_directories($("C:/Program Files/Nuke12.0v1/include"))
    link_directories($("C:/Program Files/Nuke12.0v1"))
endif()


if (NUKE_12.1)
    include_directories($("C:/Program Files/Nuke12.1v2/include"))
    link_directories($("C:/Program Files/Nuke12.1v2"))
endif()

if (NUKE_12.2)
    include_directories($("C:/Program Files/Nuke12.2v11/include"))
    link_directories($("C:/Program Files/Nuke12.2v11"))
endif()

# Auto Moc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 1.- Download first the nuke binaries in order to integrate Qt5 in your plugin
#     Nuke 13 binaries will also work for nuke 12
#     Binaries: https://www.foundry.com/products/nuke-family/developers
# 2.- Create a folder in your system disk "C:\NukeQt"
# 3.- Add "C:\NukeQt" to your system variables with name "QTDIR" and variable value "C:\NukeQt", save it!
# 4.- Happy coding!

# Find Qt
find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Add libraries
add_library(NukePluginTest SHARED 
    src/NukePluginTest.cpp
    src/TimeCam.cpp
    src/NukePluginTest.h
    src/ToggleButton.h
)
target_link_libraries(NukePluginTest Qt5::Widgets)

if (NOT NUKE_10.5)
    target_link_libraries(NukePluginTest DDImage glew32 legacy_stdio_definitions Qt5::Widgets)
endif()


# Generate a .dll plugin in my plugin folder then open nukeX app
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "C:/Users/rojas/.nuke/gonz/plugins/Other"
    COMMAND "C:/Program Files/Nuke12.1v2/Nuke12.1.exe" --nukex
)
