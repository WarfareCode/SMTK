#=============================================================================
#
#  Copyright (c) Kitware, Inc.
#  All rights reserved.
#  See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.  See the above copyright notice for more information.
#
#=============================================================================

set (unit_tests
  UnitTestPolygonCreateModel.cxx
  UnitTestPolygonCreateEdgeFromPoints.cxx
  UnitTestPolygonCreateEdgeFromVerts.cxx
  UnitTestPolygonCreateFacesFromEdges.cxx
  UnitTestPolygonDemoteVertex.cxx
  UnitTestPolygonFindOperationAttItems.cxx
  UnitTestPolygonCleanGeometry.cxx)

if(SMTK_ENABLE_VTK_SUPPORT)
  set (unit_tests_which_require_data
    UnitTestPolygonReadWrite.cxx)
endif()

if (SMTK_ENABLE_VTK_SUPPORT AND SMTK_ENABLE_REMUS_SUPPORT)
  list(APPEND unit_tests_which_require_data UnitTestPolygonImport.cxx)
endif ()

set(external_libs ${Boost_LIBRARIES})

smtk_unit_tests(
  LABEL "PolygonSession"
  SOURCES ${unit_tests}
  SOURCES_REQUIRE_DATA ${unit_tests_which_require_data}
  LIBRARIES smtkCore smtkPolygonSession smtkCoreModelTesting ${external_libs}
)
