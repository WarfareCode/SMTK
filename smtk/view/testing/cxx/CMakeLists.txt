set(unit_tests
)

set(unit_tests_which_require_data
  unitDescriptivePhrase.cxx
  unitComponentPhraseModel.cxx
  unitJSONView.cxx
)

if (SMTK_ENABLE_POLYGON_SESSION)
  smtk_unit_tests(
    Label "View"
    SOURCES ${unit_tests}
    SOURCES_REQUIRE_DATA ${unit_tests_which_require_data}
    LIBRARIES
      smtkCore
      smtkCoreResourceTesting
      smtkPolygonSession
      ${Boost_LIBRARIES}
  )
endif()
