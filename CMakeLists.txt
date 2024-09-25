cmake_minimum_required(VERSION 3.5.0)
project(Logger VERSION 0.1.0 LANGUAGES C CXX)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(TestLogger test/TestLogger.cpp)

target_link_libraries(
    TestLogger
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(TestLogger)