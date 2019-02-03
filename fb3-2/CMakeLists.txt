project(fb)

cmake_minimum_required(VERSION 3.13.4)

find_package(BISON 3.0.5)
find_package(FLEX 2.6.4)

set(TARGET fb)

add_definitions(-D_DEBUG)

BISON_TARGET(DefaultParser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.h)
FLEX_TARGET(DefaultScanner scanner.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/scanner.h)
ADD_FLEX_BISON_DEPENDENCY(DefaultScanner DefaultParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${TARGET} ${BISON_DefaultParser_OUTPUTS} ${FLEX_DefaultScanner_OUTPUTS} main.cpp)

add_custom_target (test
  COMMAND ${TARGET}
  DEPENDS ${TARGET}
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  COMMENT "execute ${TARGET} for test"
)