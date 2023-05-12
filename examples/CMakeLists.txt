cmake_minimum_required(VERSION 3.15)
project(demos)

STRING(REPLACE "examples" "lib/cmake" APPENDED_DIR ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND CMAKE_PREFIX_PATH ${APPENDED_DIR})

find_package(phawd REQUIRED)

set(CMAKE_CXX_STANDARD 11)
add_executable(shm_demo shm_demo/shm_demo.cpp)
add_executable(socket_demo socket_demo/socket_demo.cpp)

target_include_directories(shm_demo PUBLIC ${PHAWD_INCLUDE_DIR})
target_include_directories(socket_demo PUBLIC ${PHAWD_INCLUDE_DIR})

target_link_libraries(shm_demo phawd::phawd-shared)
target_link_libraries(socket_demo phawd::phawd-shared)
#                   or
# target_link_libraries(shm_demo ${PHAWD_SHARED_LIB})
# target_link_libraries(socket_demo ${PHAWD_SHARED_LIB})

# target_link_libraries(shm_demo phawd::phawd-static)
# target_link_libraries(socket_demo phawd::phawd-static)
#                   or 
# target_link_libraries(shm_demo ${PHAWD_STATIC_LIB})
# target_link_libraries(socket_demo ${PHAWD_STATIC_LIB})
# target_compile_definitions(shm_demo PRIVATE PHAWD_STATIC)
# target_compile_definitions(socket_demo PRIVATE PHAWD_STATIC)
