# if (NOT TARGET spdlog)
#     set(SPDLOG_BUILD_TESTING OFF)
#     set(SPDLOG_BUILD_EXAMPLES OFF)
#     set(SPDLOG_BUILD_BENCH OFF)

#     add_subdirectory(${PROJECT_SOURCE_DIR}/external/spdlog EXCLUDE_FROM_ALL)
#     SET_PROPERTY(GLOBAL APPEND PROPERTY imported_libraries spdlog)
# endif()

# add_library(spdlog INTERFACE)
# target_include_directories(spdlog INTERFACE 
#                            ${PROJECT_SOURCE_DIR}/external/spdlog/include)

set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/spdlog EXCLUDE_FROM_ALL)
