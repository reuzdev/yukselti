cmake_minimum_required(VERSION 3.24)
project(yukselti)

set(RAYLIB_VERSION 5.5)

include(FetchContent)
FetchContent_Declare(
	raylib
	DOWNLOAD_EXTRACT_TIMESTAMP OFF
	URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
	FIND_PACKAGE_ARGS NAMES raylib ${RAYLIB_VERSION}
)
set(FETCHCONTENT_QUIET NO)
FetchContent_MakeAvailable(raylib)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME}
  src/main.c
  src/player.c
  src/map.c
  src/projection.c
  src/common.c
  src/app.c
  src/gui.c
)

target_compile_features(${PROJECT_NAME} PRIVATE c_std_11)

target_link_libraries(${PROJECT_NAME} raylib Threads::Threads)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "${CMAKE_BINARY_DIR}/$<CONFIG>/assets"
    COMMENT "Copying assets to output directory"
)
