cmake_minimum_required(VERSION 3.8)

include("${CMAKE_CURRENT_LIST_DIR}/gitlab_ci.cmake")

# Read the files from the build directory.
ctest_read_custom_files("${CTEST_BINARY_DIRECTORY}")
include("${CMAKE_CURRENT_LIST_DIR}/ctest_submit_multi.cmake")

# Pick up from where the configure left off.
ctest_start(APPEND)

if (CTEST_CMAKE_GENERATOR STREQUAL "Unix Makefiles")
  include(ProcessorCount)
  ProcessorCount(nproc)
  set(CTEST_BUILD_FLAGS "-j${nproc}")
endif ()

ctest_build(
  NUMBER_WARNINGS num_warnings
  RETURN_VALUE build_result)
ctest_submit_multi(PARTS Build)

if (build_result)
  message(FATAL_ERROR
    "Failed to build")
endif ()

if ("$ENV{CTEST_NO_WARNINGS_ALLOWED}" AND num_warnings GREATER 0)
  message(FATAL_ERROR
    "Found ${num_warnings} warnings (treating as fatal).")
endif ()
