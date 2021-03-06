# set up sources to build
set(viewSrcs
  AvailableOperations.cxx
  BaseView.cxx
  ComponentPhraseContent.cxx
  ComponentPhraseModel.cxx
  DescriptivePhrase.cxx
  EmptySubphraseGenerator.cxx
  IconFactory.cxx
  Information.cxx
  json/jsonView.cxx
  Manager.cxx
  ObjectGroupPhraseContent.cxx
  PhraseListContent.cxx
  PhraseModel.cxx
  QueryFilterSubphraseGenerator.cxx
  ReferenceItemPhraseModel.cxx
  Registrar.cxx
  ResourcePhraseContent.cxx
  ResourcePhraseModel.cxx
  Selection.cxx
  SelectionPhraseModel.cxx
  SubphraseGenerator.cxx
  SVGIconConstructor.cxx
  TwoLevelSubphraseGenerator.cxx
  Configuration.cxx
  VisibilityContent.cxx
)

set (jsonViewHeaders
  json/jsonView.h
  )
set(viewHeaders
  AvailableOperations.h
  BaseView.h
  ComponentPhraseContent.h
  ComponentPhraseModel.h
  DescriptivePhrase.h
  EmptySubphraseGenerator.h
  IconFactory.h
  Information.h
  Manager.h
  ObjectGroupPhraseContent.h
  PhraseListContent.h
  PhraseContent.h
  PhraseModel.h
  PhraseModelObserver.h
  QueryFilterSubphraseGenerator.h
  ReferenceItemPhraseModel.h
  Registrar.h
  ResourcePhraseContent.h
  ResourcePhraseModel.h
  Selection.h
  SelectionAction.h
  SelectionObserver.h
  SelectionPhraseModel.h
  SubphraseGenerator.h
  SubphraseGenerator.txx
  SVGIconConstructor.h
  TwoLevelSubphraseGenerator.h
  Configuration.h
  VisibilityContent.h
  ${jsonViewHeaders}
)

if (SMTK_ENABLE_PYTHON_WRAPPING)
  add_subdirectory(pybind11)
endif()

#install the headers
smtk_public_headers(smtkCore ${viewHeaders})

set(iconFiles
  icons/attribute.svg
  icons/attributeResource.svg
  icons/edge.svg
  icons/face.svg
  icons/mesh.svg
  icons/model.svg
  icons/vertex.svg
  icons/volume.svg
)

include(EncodeStringFunctions)

foreach (iconFile ${iconFiles})
  get_filename_component(genFileBase "${iconFile}" NAME_WE)
  set(genFile "${CMAKE_CURRENT_BINARY_DIR}/icons/${genFileBase}_svg.h")
  configureFileAsCVariable("${CMAKE_CURRENT_SOURCE_DIR}/${iconFile}" "${genFile}" "${genFileBase}_svg" "")
endforeach()

if (SMTK_ENABLE_PARAVIEW_SUPPORT)
  set_property(GLOBAL APPEND
    PROPERTY _smtk_plugin_files "${CMAKE_CURRENT_SOURCE_DIR}/plugin/paraview.plugin")
endif()

if (SMTK_ENABLE_TESTING)
  add_subdirectory(testing)
endif()
