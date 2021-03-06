if(SMTK_DATA_DIR)
  add_executable(SessionTest SessionTest.cxx)
  target_link_libraries(SessionTest smtkCore smtkDiscreteSession)
  add_test(
    NAME discreteSessionTest
    COMMAND SessionTest ${SMTK_DATA_DIR}/model/2d/cmb/test2D.cmb ${SMTK_DATA_DIR}/model/3d/cmb/smooth_surface.cmb
  )

  add_executable(Import2dmTest import2dm.cxx)
  target_link_libraries(Import2dmTest
    smtkCore
    smtkDiscreteSession
    vtkSMTKSourceExt
    vtkSMTKMeshExt
    VTK::IOXML
    VTK::RenderingGL2PSOpenGL2
    VTK::TestingRendering
  )
  add_test(
    NAME discreteImport2dmTest
    COMMAND
      $<TARGET_FILE:Import2dmTest>
      "${SMTK_DATA_DIR}/mesh/2d/test2D.2dm"
      -V "${SMTK_DATA_DIR}/baseline/smtk/discrete/import2dm.png"
      -T "${CMAKE_CURRENT_BINARY_DIR}"
  )

# TODO This test uses the old MeshSelectionItem and needs
# to be updated or removed
#  add_executable(EdgeOp2dmTest edgeOp2dm.cxx)
#  target_link_libraries(EdgeOp2dmTest
#    smtkCore
#    smtkDiscreteSession
#    vtkSMTKSourceExt
#    VTK::RenderingGL2PSOpenGL2
#    VTK::TestingRendering
#  )
#  add_test(
#    NAME discreteEdgeOp2dmTest
#    COMMAND
#      $<TARGET_FILE:EdgeOp2dmTest>
#      "${SMTK_DATA_DIR}/mesh/2d/test2D.2dm"
#      -V "${SMTK_DATA_DIR}/baseline/smtk/discrete/edgeOp2dm.png"
#      -T "${CMAKE_CURRENT_BINARY_DIR}"
#  )

  add_executable(smtkModelfrom2dmTest smtkModelfrom2dm.cxx)
  target_link_libraries(smtkModelfrom2dmTest
    smtkCore
    smtkDiscreteSession
    vtkSMTKSourceExt
    VTK::IOXML
    VTK::RenderingGL2PSOpenGL2
    VTK::TestingRendering
    ${Boost_LIBRARIES}
  )
  target_compile_definitions(smtkModelfrom2dmTest PRIVATE "SMTK_SCRATCH_DIR=\"${CMAKE_BINARY_DIR}/Testing/Temporary\"")
  add_test(
    NAME discreteSmtkModelfrom2dmTest
    COMMAND
      $<TARGET_FILE:smtkModelfrom2dmTest>
      "${SMTK_DATA_DIR}/mesh/2d/test2D.2dm"
      -V "${SMTK_DATA_DIR}/baseline/smtk/discrete/smtkModelfrom2dm.png"
      -T "${CMAKE_CURRENT_BINARY_DIR}"
  )

  add_executable(entityGroup2dmTest entityGroup2dm.cxx)
  target_link_libraries(entityGroup2dmTest
    smtkCore
    smtkDiscreteSession
    vtkSMTKOperationsExt
    vtkSMTKSourceExt
    VTK::RenderingGL2PSOpenGL2
    VTK::TestingRendering
    )
  add_test(
    NAME entityGroup2dmTest
    COMMAND
      $<TARGET_FILE:entityGroup2dmTest>
      "${SMTK_DATA_DIR}/mesh/2d/test2D.2dm"
  )

  add_executable(boundingBox2dmTest boundingBox2dm.cxx)
  target_link_libraries(boundingBox2dmTest
    smtkCore
    smtkDiscreteSession
    vtkSMTKOperationsExt
    vtkSMTKSourceExt
    )
  add_test(
    NAME boundingBox2dmTest
    COMMAND
      $<TARGET_FILE:boundingBox2dmTest>
      "${SMTK_DATA_DIR}/mesh/2d/test2D.2dm"
  )

  add_executable(remove2dmTest remove2dm.cxx)
  target_link_libraries(remove2dmTest
    smtkCore
    smtkDiscreteSession
    vtkSMTKSourceExt
    VTK::RenderingGL2PSOpenGL2
    VTK::TestingRendering
  )
  add_test(
    NAME discreteRemove2dmTest
    COMMAND
      $<TARGET_FILE:remove2dmTest>
      "${SMTK_DATA_DIR}/mesh/2d/test2D.2dm"
  )

  add_executable(mergeFaceTest mergeFaceTest.cxx)
  target_link_libraries(mergeFaceTest
    smtkCore
    smtkDiscreteSession
    vtkSMTKSourceExt
    VTK::RenderingGL2PSOpenGL2
    VTK::TestingRendering
  )
  add_test(
    NAME discreteMergeFaceTest
    COMMAND
      $<TARGET_FILE:mergeFaceTest>
      "${SMTK_DATA_DIR}/model/3d/cmb/simplebox.cmb"
  )
endif()
