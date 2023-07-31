cmake_minimum_required(VERSION 3.12)
project(gobang)

set(CMAKE_CXX_STANDARD 20)


# ==========================

add_subdirectory(3rdparty)

find_package(Gettext REQUIRED)
find_package(Intl REQUIRED)

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

# ==========================

file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_LIST})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_SOURCE_DIR}/include/
    PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/mixi/include/

    PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/glad/include/
    PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/stb/
    PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/glm/

    PUBLIC ${Intl_INCLUDE_DIR}
    PUBLIC ${Boost_INCLUDE_DIR}
    PUBLIC ${OPENSSL_INCLUDE_DIR}    
)

target_link_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_BINARY_DIR}/3rdparty/
    PUBLIC ${ASSIMP_ARCHIVE_OUTPUT_DIRECTORY}
    PUBLIC ${GLFW_BINARY_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
    Boost::signals2
    Boost::uuid
    ${OPENSSL_LIBRARIES}
    mixi
)


if (APPLE)
    target_link_libraries(${PROJECT_NAME}
        boost_thread
        "-framework Cocoa -framework IOKit"
    )
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME}
        pthread
        dl
        X11
    )
endif()

# ==========================

file(GLOB_RECURSE RESOURCE_LIST CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/resource/*)
foreach(RESOURCE_FILE ${RESOURCE_LIST})
    file(RELATIVE_PATH REL_PATH ${CMAKE_SOURCE_DIR} ${RESOURCE_FILE})
    get_filename_component(REL_DIR ${REL_PATH} DIRECTORY)
    file(COPY ${CMAKE_SOURCE_DIR}/${REL_PATH} DESTINATION ${CMAKE_BINARY_DIR}/${REL_DIR})
endforeach()
