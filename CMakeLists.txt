cmake_minimum_required(VERSION 3.15)
include(FetchContent)

project(ZeroCopyMessagePack)
set(CMAKE_BUILD_TYPE DEBUG)
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
endif()
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})

file(GLOB ${PROJECT_NAME}_SRC ${PROJECT_SOURCE_DIR}/*.c*)
add_library(${PROJECT_NAME}
  ${${PROJECT_NAME}_SRC}
)
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  )
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# The following will build unit-tests and also pull in Catch2 as a dependency.
if(CMAKE_TESTING_ENABLED)
    FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG        v3.0.0-preview3
    )
    FetchContent_MakeAvailable(Catch2)

    set(TARGET_NAME "ZeroCopyMessagePackTests")
    file(GLOB ${PROJECT_NAME}_TEST_SRC ${PROJECT_SOURCE_DIR}/test/*.c*)
    add_executable(${TARGET_NAME}
        ${${PROJECT_NAME}_TEST_SRC}
        )
    target_link_libraries(${TARGET_NAME} PRIVATE ${PROJECT_NAME} Catch2::Catch2WithMain)

    add_test(${TARGET_NAME} ${PROJECT_BINARY_DIR}/${TARGET_NAME})
    message("Building unit-test. Executable=${PROJECT_BINARY_DIR}/${TARGET_NAME}")
  
    FetchContent_Declare(
      cmake-codecov
      GIT_REPOSITORY https://github.com/callalilychen/CMake-codecov.git
      GIT_TAG        9d810304424e3bf0b0d8d051c59786765f534568
    )
    FetchContent_Populate(cmake-codecov)
    set(CMAKE_MODULE_PATH "${cmake-codecov_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
    find_package(codecov)
    add_coverage(${PROJECT_NAME})
    add_coverage(${TARGET_NAME})
    list(APPEND LCOV_REMOVE_PATTERNS "'/usr/*'")
    coverage_evaluate()

endif()
