set(smtkViewPythonTests
  modifyViewComponents
)

# Additional tests that require SMTK_DATA_DIR
if (SMTK_ENABLE_POLYGON_SESSION)
  set(smtkViewPythonDataTests
    unitSelection
  )
endif()

foreach (test ${smtkViewPythonTests})
  smtk_add_test_python(${test}Py ${test}.py --temp-dir=${CMAKE_BINARY_DIR}/Testing/Temporary)
endforeach()

if (SMTK_DATA_DIR)
  foreach (test ${smtkViewPythonDataTests})
    smtk_add_test_python(${test}Py ${test}.py
      --data-dir=${SMTK_DATA_DIR}
      --temp-dir=${CMAKE_BINARY_DIR}/Testing/Temporary)
  endforeach()
endif()
