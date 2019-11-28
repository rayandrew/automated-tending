cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)

# Set project name, version and languages
project(AUTOMATED_TENDING_MACHINE VERSION 0.0.0.1 LANGUAGES CXX)

# Options: Things you can set via commandline options to cmake (e.g. -DENABLE_LTO=[ON|OFF])
option(ENABLE_WARNINGS_SETTINGS "Allow target_set_warnings to add flags and defines.
                                 Set this to OFF if you want to provide your own warning parameters." ON)
option(ENABLE_LTO "Enable link time optimization" ON)
option(ENABLE_DOCTESTS "Include tests in the library. Setting this to OFF will remove all doctest related code. Tests in tests/*.cpp will still be enabled." OFF)
option(BUILD_SHARED_LIBS "Build shared libs in order for some libraries to work" ON)

# Include stuffs
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(ConfigSafeGuards)
include(Colors)
include(Cotire)
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

set(RASPI_LIB "")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  add_definitions(-DMOCK_GPIO)
else()
  find_package(pigpio REQUIRED)
  LIST(APPEND RASPI_LIB "pigpio")
  LIST(APPEND RASPI_LIB "rt")
endif()


# Check for LTO support.
find_lto(CXX)

# find Qt5 and required components
find_package(Qt5 COMPONENTS REQUIRED
             Core Gui Widgets) 

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# --------------------------------------------------------------------------------
# Locate files 
# --------------------------------------------------------------------------------

# All .cpp files in src/
set(SOURCES
    src/app.cpp
    src/config.cpp
    src/state.cpp
    src/logger.cpp
    src/gpio.cpp

    src/utils/qspdlog.cpp
    src/utils/worker.cpp
    src/utils/simple_worker.cpp
    src/utils/signal_merge.cpp

    src/windows/main_window.cpp

    src/devices/stepper.cpp
    src/devices/device.cpp
    src/devices/output_device.cpp
    src/devices/limit_switch.cpp

    src/mechanisms/movement.cpp) 

# All .ui files in resources/ui
set(UI_SOURCES
    resources/ui/main_window.ui)

# All .h files in include/
set(HEADERS
    include/app.h
    include/config.h
    include/state.h
    include/logger.h
    include/gpio.h
    
    include/utils/qspdlog.h
    include/utils/worker.h
    include/utils/simple_worker.h
    include/utils/signal_merge.h
    
    include/windows/main_window.h

    include/devices/stepper.h
    include/devices/device.h
    include/devices/output_device.h
    include/devices/limit_switch.h

    include/mechanisms/movement.h) 

# All .cpp files in tests/
set(TESTFILES
    tests/main.cpp)

set(LIBRARY_NAME engine)

# --------------------------------------------------------------------------------
# Building
# --------------------------------------------------------------------------------

# Qt5 wrapper
qt5_wrap_ui(ui_sources_headers ${UI_SOURCES})
qt5_wrap_cpp(ui_sources_moc ${HEADERS})

# Compile all sources into a library.
add_library(${LIBRARY_NAME} OBJECT
            ${SOURCES}
            ${ui_sources_headers}
            ${ui_sources_moc})

# Lib needs its header files, and users of the library must also see these (PUBLIC)
target_include_directories(${LIBRARY_NAME} PUBLIC
                           ${PROJECT_SOURCE_DIR}/include
                           ${Qt5Widgets_INCLUDES})

# There's also (probably) doctests within the library, so we need to see this as well.
target_link_libraries(${LIBRARY_NAME} PUBLIC
                      ${RASPI_LIB}
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

add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
add_compile_definitions(BUILD_SHARED_LIBS
                        SPDLOG_MASTER_PROJECT
                        SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG
                        SPDLOG_FMT_EXTERNAL)

# Add an executable for the file app/main.cpp.
add_executable(main app/main.cpp)

# Link the executable to libraries
target_link_libraries(main PRIVATE ${LIBRARY_NAME}
                      ${RASPI_LIB}
                      spdlog::spdlog
                      fmt::fmt
                      fruit
                      yaml::yaml
                      Qt5::Core
                      Qt5::Widgets)

# Precompiled Headers
cotire(main)

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

# Set up tests
add_subdirectory(tests)

# Create logs directory
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")
