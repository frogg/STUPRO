#
# Find the GTest includes and library
#

set(GTEST_FOUND "NO")

set(GTest_LIB "${CMAKE_BINARY_DIR}/gtest/googletest/"
    CACHE PATH
    "Path to GTest build"
)

set(GTest_INCLUDES "${CMAKE_SOURCE_DIR}/libs/gtest/googletest/include/"
    CACHE PATH
    "Path to GTest includes"
)

if(EXISTS "${GTest_LIB}/libgtest_main.so" AND EXISTS "${GTest_LIB}/libgtest.so")
    set(GTEST_BOTH_LIBRARIES
        "${GTest_LIB}/libgtest_main.so"
        "${GTest_LIB}/libgtest.so"
    )
elseif(EXISTS "${GTest_LIB}/libgtest_main.dll" AND EXISTS "${GTest_LIB}/libgtest.dll")
    set(GTEST_BOTH_LIBRARIES
        "${GTest_LIB}/libgtest_main.dll"
        "${GTest_LIB}/libgtest.dll"
    )
elseif(EXISTS "${GTest_LIB}/libgtest_main.a" AND EXISTS "${GTest_LIB}/libgtest.a")
    set(GTEST_BOTH_LIBRARIES
        "${GTest_LIB}/libgtest_main.a"
        "${GTest_LIB}/libgtest.a"
    )
else()
    message (FATAL_ERROR
        "GTest library not found, enter the path to a built GTest library in GTest_LIB"
    )
endif()

if(EXISTS ${GTest_INCLUDES})
    set(GTEST_INCLUDE_DIR ${GTest_INCLUDES})
else()
    message (FATAL_ERROR
        "GTest includes not found, enter the path to the GTest include directory in GTest_INCLUDES"
    )
endif()

set(GTEST_FOUND "YES")
