cmake_minimum_required(VERSION 3.10)

# Set the project name
project(EVA_Test VERSION 1.0 LANGUAGES CXX)

# Source files
file(GLOB SOURCES
	source/EVA/Test/*
)

# Create library
add_library(EVA_Test STATIC ${SOURCES})

set_target_properties(
    EVA_Test
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Include paths
target_include_directories(EVA_Test 
	PUBLIC "source"
)

# Tests
option(${PROJECT_NAME}_ENABLE_TESTS "Enable the tests" ON)
if(${PROJECT_NAME}_ENABLE_TESTS)
    file(GLOB_RECURSE TESTSOURCES
	    test/*
    )
    add_executable(EVA_Test_Test ${TESTSOURCES})
    set_target_properties(
        EVA_Test_Test
        PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
    target_link_libraries(EVA_Test_Test PRIVATE EVA_Test)
endif()