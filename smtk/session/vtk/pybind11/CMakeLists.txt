set(library_name "_smtkPybindVTKSession")
set(module_path "session/vtk")
set(build_path "${CMAKE_BINARY_DIR}/${SMTK_PYTHON_MODULEDIR}/smtk/${module_path}")
set(install_path "${SMTK_PYTHON_MODULEDIR}/smtk/${module_path}")

pybind11_add_module(${library_name} PybindVTKSession.cxx)
target_include_directories(${library_name} PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  )
set(import_vtk_depends
    VTK::FiltersGeneral
    VTK::FiltersGeometry
    VTK::ImagingCore
    VTK::IOExodus
    VTK::IOGeometry
    VTK::IOLegacy
    VTK::IONetCDF
    VTK::IOParallel
    VTK::IOParallelExodus
    VTK::IOXML
    )
target_link_libraries(${library_name}
  LINK_PUBLIC
    smtkCore
    smtkVTKSession
    VTK::CommonDataModel
  LINK_PRIVATE
    ${import_vtk_depends}
)
vtk_module_autoinit(
  TARGETS ${library_name}
  MODULES VTK::CommonDataModel
          ${import_vtk_depends})
set_target_properties(${library_name}
  PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    COMPILE_FLAGS ${SMTK_PYBIND11_FLAGS}
    LIBRARY_OUTPUT_DIRECTORY "${build_path}"
  )

# Install library
install(TARGETS ${library_name} DESTINATION "${install_path}")

# Create and install module __init__.py
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/__init__.py.in"
  "${build_path}/__init__.py" @ONLY
  )

install(
  FILES "${build_path}/__init__.py"
  DESTINATION "${install_path}"
  )
