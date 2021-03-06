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

import os
import smtk
import smtk.common
import smtk.io
import smtk.mesh
import smtk.testing
import sys


def test_create_cell_field():

    # Load the mesh file
    mesh_path = os.path.join(smtk.testing.DATA_DIR, 'mesh', '2d/twoMeshes.h5m')
    c = smtk.mesh.Resource.create()
    smtk.io.importMesh(mesh_path, c)
    if not c.isValid():
        raise RuntimeError("Failed to read valid mesh")

    mesh = c.meshes()
    field = [i for i in range(c.meshes().cells().size())]
    mesh.createCellField('cell field', 1, field)

    write_path = ''
    write_path = os.path.join(smtk.testing.TEMP_DIR,
                              str(smtk.common.UUID.random()) + ".h5m")

    smtk.io.exportMesh(write_path, c)
    return write_path


def test_read_cell_field(mesh_path):

    # Load the mesh file
    c = smtk.mesh.Resource.create()
    smtk.io.importMesh(mesh_path, c)
    if not c.isValid():
        raise RuntimeError("Failed to read back valid mesh")

    mesh = c.meshes()
    cellfields = mesh.cellFields()

    if not cellfields:
        raise RuntimeError("No cell fields associated with the mesh")

    cellfield = next(iter(cellfields))
    data = cellfield.get()

    for i in range(cellfield.size()):
        if i != data[i]:
            raise RuntimeError(
                "cell field was not correctly saved and retrieved")

    os.remove(mesh_path)

if __name__ == '__main__':
    smtk.testing.process_arguments()
    resource_url = test_create_cell_field()
    test_read_cell_field(resource_url)
