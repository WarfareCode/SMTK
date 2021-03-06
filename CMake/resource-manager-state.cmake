# This file is provided with SMTK as an example of how to configure a plugin
# contract file. In general, SMTK should not contain these files. Instead, the
# dashboard computer that runs SMTK tests should be passed a list of URLs for
# plugin contract test files during its configuration.

cmake_minimum_required(VERSION 2.8)
project(resource-manager-state)

include(ExternalProject)

ExternalProject_Add(resource-manager-state
  GIT_REPOSITORY "https://gitlab.kitware.com/cmb/plugins/read-and-write-resource-manager-state.git"
  GIT_TAG "origin/master"
  PREFIX plugin
  STAMP_DIR plugin/stamp
  SOURCE_DIR plugin/src
  BINARY_DIR plugin/build
  CMAKE_ARGS
    -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
    -DENABLE_TESTING=ON
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -Dsmtk_DIR=${smtk_DIR}
    ${response_file}
  INSTALL_COMMAND ""
  TEST_BEFORE_INSTALL True
)
