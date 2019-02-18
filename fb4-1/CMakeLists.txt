project(fb)

cmake_minimum_required(VERSION 3.13.1)

find_package(BISON 3.0.4)
find_package(FLEX 2.5.37)

set( PROJECT_VERSION_MAJOR 1 )
set( PROJECT_VERSION_MINOR 0 )
set( PROJECT_VERSION_PATCH 0 )
set( PROJECT_VERSION_REVISION 0 )
set( PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} )

set(TARGET fb)
set(TESTFILE1 test.txt)
set(TESTFILE2 input.txt)

add_definitions(-D_DEBUG)

BISON_TARGET(DefaultParser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.h)
FLEX_TARGET(DefaultScanner scanner.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/scanner.h)
ADD_FLEX_BISON_DEPENDENCY(DefaultScanner DefaultParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${TARGET} ${BISON_DefaultParser_OUTPUTS} ${FLEX_DefaultScanner_OUTPUTS} wingetopt.c main.cpp)

add_custom_target (test
  DEPENDS test0
)

add_custom_target (test0
  COMMAND ${TARGET} -d
  DEPENDS ${TARGET}
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  COMMENT "execute ${TARGET} for test"
)

add_custom_target (test1
  COMMAND ${TARGET} -d ${TESTFILE1}
  DEPENDS ${TARGET}
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  COMMENT "execute ${TARGET} with ${TESTFILE1} for test"
)
