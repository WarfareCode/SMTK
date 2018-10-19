//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

#ifndef pybind_smtk_io_ImportMesh_h
#define pybind_smtk_io_ImportMesh_h

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "smtk/io/ImportMesh.h"

namespace py = pybind11;

PySharedPtrClass< smtk::io::ImportMesh > pybind11_init_smtk_io_ImportMesh(py::module &m)
{
  PySharedPtrClass< smtk::io::ImportMesh > instance(m, "ImportMesh");
  instance
    .def(py::init<>())
    .def_static("ExtensionIsSupported", &smtk::io::ImportMesh::ExtensionIsSupported)
    .def("__call__", (smtk::mesh::CollectionPtr (smtk::io::ImportMesh::*)(::std::string const &, const ::smtk::mesh::InterfacePtr&, ::std::string) const) &smtk::io::ImportMesh::operator())
    .def("__call__", (bool (smtk::io::ImportMesh::*)(::std::string const &, ::smtk::mesh::CollectionPtr, ::std::string) const) &smtk::io::ImportMesh::operator())
    ;
  return instance;
}

void pybind11_init__ZN4smtk2io10importMeshERKNSt3__112basic_stringIcNS1_11char_traitsIcEENS1_9allocatorIcEEEENS1_10shared_ptrINS_4mesh7InterfaceEEE(py::module &m)
{
  m.def("importMesh", (smtk::mesh::CollectionPtr (*)(::std::string const &, const ::smtk::mesh::InterfacePtr&)) &smtk::io::importMesh, "", py::arg("filePath"), py::arg("interface"));
}

void pybind11_init__ZN4smtk2io10importMeshERKNSt3__112basic_stringIcNS1_11char_traitsIcEENS1_9allocatorIcEEEENS1_10shared_ptrINS_4mesh7InterfaceEEES9_(py::module &m)
{
  m.def("importMesh", (smtk::mesh::CollectionPtr (*)(::std::string const &, const ::smtk::mesh::InterfacePtr&, ::std::string const &)) &smtk::io::importMesh, "", py::arg("filePath"), py::arg("interface"), py::arg("domainPropertyName"));
}

void pybind11_init__ZN4smtk2io10importMeshERKNSt3__112basic_stringIcNS1_11char_traitsIcEENS1_9allocatorIcEEEENS1_10shared_ptrINS_4mesh10CollectionEEE(py::module &m)
{
  m.def("importMesh", (bool (*)(::std::string const &, ::smtk::mesh::CollectionPtr)) &smtk::io::importMesh, "", py::arg("filePath"), py::arg("collection"));
}

void pybind11_init__ZN4smtk2io10importMeshERKNSt3__112basic_stringIcNS1_11char_traitsIcEENS1_9allocatorIcEEEENS1_10shared_ptrINS_4mesh10CollectionEEES9_(py::module &m)
{
  m.def("importMesh", (bool (*)(::std::string const &, ::smtk::mesh::CollectionPtr, ::std::string const &)) &smtk::io::importMesh, "", py::arg("filePath"), py::arg("collection"), py::arg("domainPropertyName"));
}

#endif
