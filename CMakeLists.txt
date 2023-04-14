cmake_minimum_required(VERSION 3.12)
project(phawd
        VERSION 0.3
        DESCRIPTION "phawd: Parameter Handler and Waveform Displayer"
        )
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

set(PHAWD_INCLUDES ${PROJECT_SOURCE_DIR}/phawd/include)
set(PHAWD_EXECUTABLE_INCLUDE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/third-party/yaml-cpp/include
    ${PROJECT_SOURCE_DIR}/third-party/qcustomplot/include)

add_subdirectory(third-party/yaml-cpp)
file(GLOB PHAWD_HEADERS ${PROJECT_SOURCE_DIR}/phawd/include/phawd/*.h)
file(GLOB PHAWD_SRCS ${PROJECT_SOURCE_DIR}/phawd/src/*.cpp)

add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_library(phawd-shared SHARED ${PHAWD_SRCS})
# if you are using ninja generator, change the OUTPUT_NAME to another name
set_target_properties(phawd-shared PROPERTIES OUTPUT_NAME phawd)
target_include_directories(phawd-shared PRIVATE ${PHAWD_INCLUDES})
target_compile_definitions(phawd-shared PRIVATE PHAWD_EXPORTS)

add_library(phawd-static STATIC ${PHAWD_SRCS})
set_target_properties(phawd-static PROPERTIES OUTPUT_NAME phawd)
target_include_directories(phawd-static PRIVATE ${PHAWD_INCLUDES})
target_compile_definitions(phawd-static PRIVATE PHAWD_STATIC)

set_target_properties(phawd-shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(phawd-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

if (UNIX)
    target_link_libraries(phawd-shared rt)
    target_link_libraries(phawd-static rt)
elseif (WINDOWS)
    target_link_libraries(phawd-shared ws2_32)
    target_link_libraries(phawd-static ws2_32)
endif ()

add_definitions(-DQCUSTOMPLOT_USE_OPENGL)
set(QCUSTOMPLOT_SRC third-party/qcustomplot/src/qcustomplot.cpp
                    third-party/qcustomplot/include/qcustomplot.h)

file(GLOB UI_LOGIC_SRCS src/*cpp)
file(GLOB UI_LOGIC_INC include/*.h)
file(GLOB UI_FILES ui_include/*.ui)

if (WIN32)
    set(QT_DIR "C:/Qt/Qt5.14.2/5.14.2")
    set(MINGW_PREFIX_PATH "mingw73_64")
    set(EXTRA_LIBS opengl32)
elseif (UNIX)
    set(QT_DIR "~/Qt5.14.2/5.14.2")
    set(MINGW_PREFIX_PATH "gcc_64")
    set(EXTRA_LIBS GL)
endif ()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)  # msvc
    message(STATUS "-- CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER_ID}")
    list(APPEND CMAKE_PREFIX_PATH "${QT_DIR}/msvc2017_64")
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)  # gcc
    message(STATUS "-- CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER_ID}")
    list(APPEND CMAKE_PREFIX_PATH "${QT_DIR}/${MINGW_PREFIX_PATH}")
endif()

find_package(Qt5 COMPONENTS
                 Widgets
                 LinguistTools
                 PrintSupport
                 Network
                 Gui
                 OpenGL
                 Gamepad
                 REQUIRED)
add_executable(phawd-executable 
                main.cpp 
                ${UI_FILES} 
                ${UI_LOGIC_INC} 
                ${UI_LOGIC_SRCS} 
                ${QCUSTOMPLOT_SRC}
                phawd.qrc
                phawd.rc)
target_include_directories(phawd-executable PRIVATE ${PHAWD_INCLUDES} ${PHAWD_EXECUTABLE_INCLUDE})
target_link_libraries(phawd-executable PRIVATE
                      Qt5::Widgets
                      Qt5::Gui
                      Qt5::PrintSupport
                      Qt5::Network
                      Qt5::OpenGL
                      Qt5::Gamepad
                      phawd-static
                      yaml-cpp
                      ${EXTRA_LIBS})
target_compile_definitions(phawd-executable PRIVATE PHAWD_STATIC)
set_target_properties(phawd-executable PROPERTIES OUTPUT_NAME phawd)

### -------- Installation --------- ####
install(TARGETS phawd-static phawd-shared phawd-executable
        EXPORT ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION include
        )

install(FILES ${PHAWD_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/phawd")
include(CMakePackageConfigHelpers)
export(EXPORT ${PROJECT_NAME}
       FILE ${CMAKE_CURRENT_BINARY_DIR}/phawd-targets.cmake
       NAMESPACE phawd::)

configure_file(phawdConfig.cmake.in phawdConfig.cmake @ONLY)

set(ConfigPackageLocation ${CMAKE_INSTALL_LIBDIR}/cmake/phawd)

install(EXPORT ${PROJECT_NAME}
        FILE phawd-targets.cmake
        NAMESPACE phawd::
        DESTINATION ${ConfigPackageLocation}
        )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/phawdConfig.cmake
        DESTINATION ${ConfigPackageLocation}
        )
