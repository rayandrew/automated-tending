cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)

# Set project name, version and languages
project(AUTOMATED_TENDING_MACHINE VERSION 0.0.0.1 LANGUAGES CXX)

# Options: Things you can set via commandline options to cmake (e.g. -DENABLE_LTO=[ON|OFF])
option(ENABLE_WARNINGS_SETTINGS "Allow target_set_warnings to add flags and defines.
                                 Set this to OFF if you want to provide your own warning parameters." ON)
option(ENABLE_LTO "Enable link time optimization" ON)
option(ENABLE_DOCTESTS "Include tests in the library. Setting this to OFF will remove all doctest related code. Tests in tests/*.cpp will still be enabled." OFF)
option(BUILD_SHARED_LIBS "Build shared libs in order for some libraries to work" ON)
# option(FRUIT_USES_BOOST "Not using any boost library for Fruit dependencies" OFF)
# option(SPDLOG_FMT_EXTERNAL "Using external FMT for SPDLOG" OFF)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-DMOCK_GPIO)
endif()

# Include stuff. No change needed.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(ConfigSafeGuards)
include(Colors)
include(CTest)
include(Doctest)
include(Fruit)
include(Documentation)
include(Fmt)
include(LTO)
include(Misc)
include(Spdlog)
include(Warnings)
include(Yaml)

# Check for LTO support.
find_lto(CXX)

# find Qt5 and required components
find_package(Qt5 COMPONENTS REQUIRED
             Core Gui Widgets) 

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")

# --------------------------------------------------------------------------------
#                         Locate files (change as needed).
# --------------------------------------------------------------------------------

set(SOURCES                     # All .cpp files in src/
    src/app.cpp
    src/config.cpp
    src/logger.cpp
    src/gpio.cpp
    src/windows/main_window.cpp
    src/devices/stepper.cpp
    src/devices/device.cpp
    src/mechanisms/finger/movement.cpp) 

file (GLOB_RECURSE
      UI_SOURCES
      RELATIVE
      ${CMAKE_SOURCE_DIR} "resources/ui/*.ui") # All .ui files in resources/ui

file(GLOB_RECURSE 
     QT5_SOURCES 
     RELATIVE 
     ${CMAKE_SOURCE_DIR} "include/windows/*.h") # All .cpp files in src/

set(TESTFILES                   # All .cpp files in tests/
    tests/main.cpp)

set(LIBRARY_NAME engine)

# Qt5 wrapper
qt5_wrap_ui(ui_sources_headers ${UI_SOURCES})
qt5_wrap_cpp(ui_sources_moc ${QT5_SOURCES})

# --------------------------------------------------------------------------------
#                            Build! (Change as needed)
# --------------------------------------------------------------------------------

# Compile all sources into a library.
add_library(${LIBRARY_NAME} OBJECT
            ${SOURCES}
            ${ui_sources_headers}
            ${ui_sources_moc})

# Lib needs its header files, and users of the library must also see these (PUBLIC). (No change needed)
target_include_directories(${LIBRARY_NAME} PUBLIC
                           ${PROJECT_SOURCE_DIR}/include)

# There's also (probably) doctests within the library, so we need to see this as well.
target_link_libraries(${LIBRARY_NAME} PUBLIC
                      doctest
                      spdlog::spdlog
                      fruit
                      fmt::fmt
                      yaml::yaml
                      Qt5::Core
                      Qt5::Widgets)

# Set the compile options you want (change as needed).
# target_set_warnings(${LIBRARY_NAME} ENABLE ALL AS_ERROR ALL DISABLE Annoying)
# target_compile_definitions(${LIBRARY_NAME} PRIVATE ...)  # For setting manually.

add_compile_definitions(BUILD_SHARED_LIBS
                        SPDLOG_MASTER_PROJECT
                        SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG
                        SPDLOG_FMT_EXTERNAL)

# Create logs directory
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")

# Add an executable for the file app/main.cpp.
add_executable(main app/main.cpp)

# Link the executable to libraries
target_link_libraries(main PRIVATE
                      ${LIBRARY_NAME}
                      spdlog::spdlog
                      fmt::fmt
                      fruit
                      yaml::yaml
                      Qt5::Core
                      Qt5::Widgets)

# Set warnings
spdlog_enable_warnings(main)
# target_set_warnings(main DISABLE Annoying)

# enable link-time-optimization if available for non-debug configurations
target_enable_lto(main optimized)

# Set the properties that applied to library and main
set_target_properties(
    ${LIBRARY_NAME} main 
      PROPERTIES 
        CXX_STANDARD 17 
        CXX_STANDARD_REQUIRED YES 
        CXX_EXTENSIONS NO)

# Set up tests (see tests/CMakeLists.txt).
add_subdirectory(tests)
