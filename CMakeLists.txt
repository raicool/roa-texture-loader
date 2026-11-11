cmake_minimum_required(VERSION 3.12)
project("roa-texture-loader" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

file(READ version ROA_TEXTURE_LOADER_VERSION)
add_compile_definitions("VERSION_STR=\"${ROA_TEXTURE_LOADER_VERSION}\"")

find_package(Git)
if(Git_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} --no-pager log -n 1 HEAD "--pretty=format:%h"
    OUTPUT_VARIABLE output
    RESULT_VARIABLE result
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(result EQUAL 0)
    add_compile_definitions("GIT_COMMIT_HASH_STR=\"${output}\"")
    add_compile_definitions("GIT_COMMIT_HASH=0x${output}")
  endif()
endif()

file(GLOB_RECURSE CXX_SOURCE "src/*.cpp")
file(GLOB IMGUI_SOURCE 
    "vendor/imgui/imgui.cpp"
    "vendor/imgui/imgui_widgets.cpp"
    "vendor/imgui/imgui_tables.cpp"
    "vendor/imgui/imgui_draw.cpp"
    "vendor/imgui/imgui_demo.cpp"
    "vendor/imgui/backends/imgui_impl_win32.cpp"
    "vendor/imgui/backends/imgui_impl_dx11.cpp"
)

add_library(${PROJECT_NAME} SHARED ${CXX_SOURCE} ${IMGUI_SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
target_include_directories(${PROJECT_NAME} PRIVATE "src" "include")

# vendors
# roa-mod-loader
add_subdirectory("vendor/roa-mod-loader")
target_link_libraries(${PROJECT_NAME} PRIVATE roa-mod-loader)
target_include_directories(${PROJECT_NAME} PRIVATE "vendor/roa-mod-loader/loader/include")

# imgui
target_include_directories(${PROJECT_NAME} PRIVATE "vendor/imgui")
target_include_directories(${PROJECT_NAME} PRIVATE "vendor/imgui/backends")

# yaml-cpp
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp::yaml-cpp)

# nlohmann/json
#add_subdirectory("vendor/nlohmann_json")
#target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

# libpng
#add_subdirectory("vendor/libpng")
#target_link_libraries(${PROJECT_NAME} PRIVATE PNG::PNG)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/details.yaml" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/info.yaml"
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "py" "${CMAKE_SOURCE_DIR}/script/launch.py"
)