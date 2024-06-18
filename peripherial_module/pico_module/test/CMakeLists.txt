cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
################################################
#set(PICO_SDK_PATH ${USERHOME}/.pico-sdk/sdk/1.5.1)
#    set(pico-sdk-tools_DIR ${USERHOME}/.pico-sdk/tools/1.5.1)
  #  include(${pico-sdk-tools_DIR}/pico-sdk-tools-config.cmake)
  #  include(${pico-sdk-tools_DIR}/pico-sdk-tools-config-version.cmake)
# include(pico_sdk_import.cmake)
# pico_sdk_init()
    ##################

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  run_test
  test_keypad.cpp
  ../src/main.c
  ../src/keypad.c
)


# Link against GTest and pthread
target_link_libraries(run_test ${GTEST_LIBRARIES} GTest::gtest_main )

include(GoogleTest)
gtest_discover_tests(run_test)