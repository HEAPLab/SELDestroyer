cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(seldestroyer)

find_package(Boost 1.45.0 COMPONENTS program_options) 


set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE SELDESTROYER_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*.c)

add_executable(seldestroyer ${SELDESTROYER_SOURCE_FILES})

install(TARGETS seldestroyer)

target_include_directories(seldestroyer PUBLIC ${CMAKE_SOURCE_DIR}/include/ ${CMAKE_SOURCE_DIR}/../libseldestroyer/include/ ${Boost_INCLUDE_DIRS})
target_link_libraries(seldestroyer ${Boost_LIBRARIES} ${CMAKE_SOURCE_DIR}/../libseldestroyer/build/libseldestroyer.so)
