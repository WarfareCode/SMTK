set(TESTS_WITH_BASELINES
  ${CMAKE_CURRENT_SOURCE_DIR}/BoxWidget.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/ConeWidget.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/LineWidget.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/PlaneWidget.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/PointWidget.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/SphereWidget.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/SplineWidget.xml
)

include(SMTKPVTestingMacros)

#TODO: this should be set for the entire project's xml tests
set(ConeWidget_USES_DIRECT_DATA ON)
set(BoxWidget_USES_DIRECT_DATA ON)
set(LineWidget_USES_DIRECT_DATA ON)
set(PlaneWidget_USES_DIRECT_DATA ON)
set(PointWidget_USES_DIRECT_DATA ON)
set(SphereWidget_USES_DIRECT_DATA ON)
set(SplineWidget_USES_DIRECT_DATA ON)

if (SMTK_DATA_DIR)
smtk_add_client_tests(
#  LABEL "pv_meshsession"
  TEST_SCRIPTS ${TESTS_WITH_BASELINES}
  LOAD_PLUGINS
    smtkPVServerExtPlugin
    smtkPQComponentsPlugin
    smtkPQWidgetsPlugin
  PLUGIN_PATHS
    $<TARGET_FILE_DIR:smtkPQComponentsPlugin>/..
    $<TARGET_FILE_DIR:smtkPVServerExtPlugin>/..
    $<TARGET_FILE_DIR:smtkPQWidgetsPlugin>/..
  DATA_DIRECTORY ${PROJECT_SOURCE_DIR}/data
  BASELINE_DIR ${PROJECT_SOURCE_DIR}/data/baseline/smtk/widgets
  )
endif()
