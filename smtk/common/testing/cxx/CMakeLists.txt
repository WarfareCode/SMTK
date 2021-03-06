smtk_public_headers(smtkCore helpers.h)

set(commonTests
  unitExtension
  unitPaths
  unitRangeDetector
  unitUnionFind
  unitUUID
)

foreach (test ${commonTests})
  add_executable(${test} MACOSX_BUNDLE  ${test}.cxx)
  target_link_libraries(${test} smtkCore)
  add_test(
    NAME ${test}
    COMMAND $<TARGET_FILE:${test}>)
endforeach()

if (SMTK_ENABLE_PYTHON_WRAPPING)
  add_executable(QuerySMTKPythonForModule QuerySMTKPythonForModule.cxx)
  target_link_libraries(QuerySMTKPythonForModule smtkCore)
  install(TARGETS QuerySMTKPythonForModule
          DESTINATION bin)
  add_test(
    NAME QuerySMTKPython
    COMMAND $<TARGET_FILE:QuerySMTKPythonForModule>)
endif()

################################################################################
# New tests using smtk test harness
################################################################################

set(unit_tests
  TestArchive.cxx
  UnitTestDerivedThreadPool
  UnitTestDateTime.cxx
  UnitTestDateTimeZonePair.cxx
  UnitTestLinks
  UnitTestObservers
  UnitTestProperties
  UnitTestRegistry.cxx
  UnitTestThreadPool
  UnitTestTypeName
)

set(unit_tests_which_require_data
)

smtk_unit_tests(
  Label "Common"
  SOURCES ${unit_tests}
  SOURCES_REQUIRE_DATA ${unit_tests_which_require_data}
  LIBRARIES smtkCore ${Boost_LIBRARIES} Threads::Threads
)
