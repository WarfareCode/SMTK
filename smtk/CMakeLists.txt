################################################################################
# Each subdirectory exports variables naming header and source files
# to its parent scope (this CMakeLists.txt) which we combine into
# the smtkCore library.

add_subdirectory(common)
smtk_source_group(common)

add_subdirectory(resource)
smtk_source_group(resource)

add_subdirectory(geometry)
smtk_source_group(geometry)

add_subdirectory(attribute)
smtk_source_group(attribute)

add_subdirectory(operation)
smtk_source_group(operation)

add_subdirectory(project)
smtk_source_group(project)

add_subdirectory(mesh)
smtk_source_group(mesh)

add_subdirectory(model)
smtk_source_group(model)

add_subdirectory(simulation)
smtk_source_group(simulation)

add_subdirectory(io)
smtk_source_group(io)

add_subdirectory(view)
smtk_source_group(view)

add_subdirectory(workflow)
smtk_source_group(workflow)

set(smtk_headers
  ${commonHeaders}
  ${resourceHeaders}
  ${geometryHeaders}
  ${attributeHeaders}
  ${operationHeaders}
  ${meshHeaders}
  ${modelHeaders}
  ${projectHeaders}
  ${viewHeaders}
  ${workflowHeaders}
  ${simulationHeaders}
  ${ioHeaders}
)
set(smtk_srcs
  ${commonSrcs}
  ${resourceSrcs}
  ${geometrySrcs}
  ${attributeSrcs}
  ${operationSrcs}
  ${projectSrcs}
  ${meshSrcs}
  ${modelSrcs}
  ${viewSrcs}
  ${workflowSrcs}
  ${simulationSrcs}
  ${ioSrcs}
)

set(smtkCore_public_include_directories
  ${MOAB_INCLUDE_DIRS}
  $<BUILD_INTERFACE:${smtk_SOURCE_DIR}>
  $<BUILD_INTERFACE:${smtk_BINARY_DIR}>
  $<INSTALL_INTERFACE:include/smtk/${SMTK_VERSION}>
  )

set(moab_libs ${MOAB_LIBRARIES})
if(WIN32)
  set(moab_libs "MOAB")
endif()
set(smtkCore_public_link_libraries
  ${moab_libs}
  nlohmann_json
  taocpp::pegtl
  )

set(smtkCore_private_link_libraries
  Boost::boost
  Boost::date_time
  Boost::filesystem
  Boost::system
  LibArchive::LibArchive
  )

if(SMTK_ENABLE_PYTHON_WRAPPING)
  # For code to optionally include ImportPythonOperator
  add_definitions(-DSMTK_PYTHON_ENABLED)

  list(APPEND smtkCore_private_link_libraries
    pybind11::embed
    Threads::Threads
    )
  add_subdirectory(pybind11)
endif()

add_library(smtkCore ${smtk_srcs})
# dependencies on generated files from subdirectories
add_dependencies(smtkCore attibuteGenHeaders meshGenHeaders modelGenHeaders operationGenHeaders)

target_include_directories(smtkCore
  PUBLIC
  ${smtkCore_public_include_directories}
)
target_link_libraries(smtkCore
  LINK_PUBLIC ${smtkCore_public_link_libraries}
  LINK_PRIVATE ${smtkCore_private_link_libraries})

smtk_precompile_headers(smtkCore
  PUBLIC
    common/Properties.h
    common/Links.h
    common/Observers.h
    common/TypeName.h
    TupleTraits.h
    )

if (WIN32)
  # Boost 1.70 and above have CMake configuration provided by upstream. Usage
  # requirements are not quite resolved yet.
  # https://github.com/boostorg/boost_install/issues/18
  target_compile_definitions(smtkCore
    PRIVATE
      BOOST_UUID_FORCE_AUTO_LINK)
endif ()

smtk_export_header(smtkCore CoreExports.h)
if (NOT BUILD_SHARED_LIBS AND SMTK_ENABLE_PYTHON_WRAPPING)
  # Set position-independent flag when other project libs are shared.
  set_target_properties(smtkCore PROPERTIES POSITION_INDEPENDENT_CODE True)
endif()

# On Mac OS X, link to the Foundation framework
if (APPLE)
  find_library(SMTK_MACOS_FOUNDATION_FMWK
    NAMES Foundation
    PATHS ${CMAKE_OSX_SYSROOT}/System/Library
    PATH_SUFFIXES Frameworks
    NO_DEFAULT_PATH)
  if (SMTK_MACOS_FOUNDATION_FMWK)
    target_link_libraries(smtkCore
      LINK_PRIVATE ${SMTK_MACOS_FOUNDATION_FMWK})
  endif()
  mark_as_advanced(SMTK_MACOS_FOUNDATION_FMWK)
endif()

#install the library and exports the library when used from a build tree
smtk_install_library(smtkCore)

################################################################################
# setup install rules
################################################################################
install(
  FILES
    AutoInit.h
    PublicPointerDefs.h
    SharedFromThis.h
    SharedPtr.h
    SystemConfig.h
    TupleTraits.h
  DESTINATION
    include/smtk/${SMTK_VERSION}/smtk
)

################################################################################
# Generate Python wrapping of the smtkCore library

if (SMTK_ENABLE_PYTHON_WRAPPING)

  #only install PythonAutoInit.h if pybind11 is enabled, since it directly
  #includes pybind11 headers
  install(
    FILES
      PythonAutoInit.h
    DESTINATION
      include/smtk/${SMTK_VERSION}/smtk
    )

  set(SMTK_PYTHON_MODULE_SRCS
    testing
    )
  foreach(pyfile ${SMTK_PYTHON_MODULE_SRCS})
    configure_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/smtk/${pyfile}.py"
      "${CMAKE_BINARY_DIR}/${SMTK_PYTHON_MODULEDIR}/smtk/${pyfile}.py" @ONLY
      )
    list(APPEND SMTK_PYTHON_MODULEFILES "smtk/${pyfile}.py")
  endforeach()
    install(
	FILES ${SMTK_PYTHON_MODULEFILES}
	DESTINATION "${SMTK_PYTHON_MODULEDIR}/smtk"
	)

  # This variable needs to be set for vtk_module to find its python module
  # output dir
  #
  # TODO: these variables must be treated differently when building with
  # VisualStudio; see VTK's treatment of these variables for the correct
  # handling of that case.
  set(VTK_BUILD_PYTHON_MODULES_DIR ${SMTK_PYTHON_MODULEDIR})
  set(VTK_INSTALL_PYTHON_MODULES_DIR ${SMTK_PYTHON_MODULEDIR})
endif()

################################################################################
# Build smtk extensions
# Add libraries that extend other projects to have support for SMTK.
# For example extension/vtk allows you to show smtk models in VTK.
################################################################################
add_subdirectory(extension)

################################################################################
# Build model sessions
# Now that the main components of smtk have been built we can build
# any custom model session sessions that the user has enabled.
################################################################################
add_subdirectory(session)

################################################################################
# Build smtk default plugin initialization
# At this point, all plugins that are built by smtk core have been
# configured. We then aggregate the plugin targets and construct a
# single library for initializing them in a ParaView-derived application.
################################################################################
if (SMTK_ENABLE_PARAVIEW_SUPPORT)
  add_subdirectory(plugin)
endif()
