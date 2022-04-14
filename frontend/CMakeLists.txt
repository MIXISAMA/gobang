cmake_minimum_required(VERSION 3.12)
project(gobang)

set(CMAKE_CXX_STANDARD 17)


# ==========================

add_subdirectory(3rdparty)

find_package(glfw3 REQUIRED)

find_package(Gettext REQUIRED)
find_package(Intl REQUIRED)

find_package(Boost REQUIRED COMPONENTS system)

# ==========================

file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_LIST})

target_include_directories(${PROJECT_NAME}
    PUBLIC 3rdparty/glad/include
    PUBLIC ${Intl_INCLUDE_DIR}
    PUBLIC ${Boost_INCLUDE_DIR}
    PUBLIC include
)

target_link_directories(${PROJECT_NAME}
    PUBLIC build/3rdparty/glad
    PUBLIC build/3rdparty/imgui
)

target_link_libraries(${PROJECT_NAME}
    glad
    glfw3
    imgui
    ${Intl_LIBRARIES}
    ${Boost_SYSTEM_LIBRARY_RELEASE}
)


if (${CMAKE_HOST_UNIX})
    target_link_libraries(${PROJECT_NAME}
        pthread
        dl
        X11
    )
endif()

if (${CMAKE_HOST_APPLE})
  target_link_libraries(${PROJECT_NAME} "-framework Cocoa -framework IOKit")
endif()

# ==========================

file(GLOB_RECURSE RESOURCE_LIST CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/resource/*)
foreach(RESOURCE_FILE ${RESOURCE_LIST})
    file(RELATIVE_PATH REL_PATH ${CMAKE_SOURCE_DIR} ${RESOURCE_FILE})
    get_filename_component(REL_DIR ${REL_PATH} DIRECTORY)
    file(COPY ${CMAKE_SOURCE_DIR}/${REL_PATH} DESTINATION ${CMAKE_BINARY_DIR}/${REL_DIR})
endforeach()
