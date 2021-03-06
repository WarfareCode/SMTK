//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

#include "smtk/io/ImportMesh.h"
#include "smtk/mesh/core/Resource.h"
#include "smtk/mesh/core/TypeSet.h"

#include "smtk/mesh/testing/cxx/helpers.h"

namespace
{

//SMTK_DATA_DIR is a define setup by cmake
std::string data_root = SMTK_DATA_DIR;

smtk::mesh::ResourcePtr load_mesh()
{
  std::string file_path(data_root);
  file_path += "/mesh/3d/twoassm_out.h5m";

  smtk::mesh::ResourcePtr mr = smtk::mesh::Resource::create();
  smtk::io::importMesh(file_path, mr);
  test(mr->isValid(), "resource should be valid");

  return mr;
}

void verify_constructors(const smtk::mesh::ResourcePtr& mr)
{
  smtk::mesh::MeshSet all_meshes = mr->meshes();

  smtk::mesh::PointConnectivity all_cells_from_ms = all_meshes.pointConnectivity();
  const smtk::mesh::PointConnectivity& copy_of_all_cells(all_cells_from_ms);

  test(!all_cells_from_ms.is_empty());
  test(all_cells_from_ms.size() != 0);
  test(all_cells_from_ms.size() == copy_of_all_cells.size());

  //The connectivity of the zeroth dimension is itself
  smtk::mesh::PointConnectivity zeroDim = mr->cells(smtk::mesh::Dims0).pointConnectivity();
  smtk::mesh::PointConnectivity equalToZeroDim = copy_of_all_cells;
  equalToZeroDim = zeroDim; //test assignment operator
  test(equalToZeroDim.size() == zeroDim.size());
  test(equalToZeroDim.numberOfCells() == zeroDim.numberOfCells());
  test(equalToZeroDim.size() == zeroDim.size());
  test(equalToZeroDim.size() == mr->cells(smtk::mesh::Dims0).size());

  //The connectivity of the first dimension
  smtk::mesh::PointConnectivity oneDim = mr->cells(smtk::mesh::Dims1).pointConnectivity();
  smtk::mesh::PointConnectivity equalToOneDim = copy_of_all_cells;
  equalToOneDim = oneDim; //test assignment operator
  test(equalToOneDim.numberOfCells() == oneDim.numberOfCells());
  test(equalToOneDim.size() != oneDim.numberOfCells());
  test(!equalToOneDim.is_empty());
}

void verify_empty(const smtk::mesh::ResourcePtr& mr)
{
  smtk::mesh::MeshSet no_mesh = mr->meshes("bad name string");

  smtk::mesh::PointConnectivity no_cells_a = no_mesh.pointConnectivity();
  smtk::mesh::PointConnectivity no_cells_b =
    no_mesh.cells(smtk::mesh::Hexahedron).pointConnectivity();
  smtk::mesh::PointConnectivity no_cells_c = no_mesh.cells(smtk::mesh::Dims2).pointConnectivity();
  smtk::mesh::PointConnectivity no_cells_d =
    no_mesh.cells(smtk::mesh::CellTypes()).pointConnectivity();

  test(no_cells_a.is_empty());
  test(no_cells_b.is_empty());
  test(no_cells_c.is_empty());
  test(no_cells_d.is_empty());

  test(no_cells_a.size() == 0);
  test(no_cells_b.size() == 0);
  test(no_cells_c.size() == 0);
  test(no_cells_d.size() == 0);

  test(no_cells_a.numberOfCells() == 0);
  test(no_cells_b.numberOfCells() == 0);
  test(no_cells_c.numberOfCells() == 0);
  test(no_cells_d.numberOfCells() == 0);
}

void verify_all_connecitivity(const smtk::mesh::ResourcePtr& mr)
{
  smtk::mesh::MeshSet all_meshes = mr->meshes();
  smtk::mesh::PointConnectivity all_cells_from_collec = mr->pointConnectivity();
  smtk::mesh::PointConnectivity all_cells_from_ms = all_meshes.pointConnectivity();

  test(!all_cells_from_collec.is_empty());
  test(!all_cells_from_ms.is_empty());

  test(all_cells_from_collec.size() != 0);
  test(all_cells_from_ms.size() != 0);

  test(all_cells_from_collec.size() == all_cells_from_ms.size());
}

void verify_simple_equiv(const smtk::mesh::ResourcePtr& mr)
{
  smtk::mesh::PointConnectivity twoDim = mr->cells(smtk::mesh::Dims2).pointConnectivity();
  smtk::mesh::PointConnectivity oneDim = mr->cells(smtk::mesh::Dims1).pointConnectivity();

  test(twoDim == twoDim);
  test(!(twoDim != twoDim));
  test(oneDim != twoDim);
  test(!(oneDim == twoDim));

  const smtk::mesh::PointConnectivity& twoDim_a(twoDim);
  test(twoDim_a == twoDim);

  smtk::mesh::PointConnectivity oneDim_b = twoDim_a;
  oneDim_b = oneDim; //test assignment operator
  test(oneDim_b == oneDim);

  test(twoDim_a != oneDim_b);
}

void verify_complex_equiv(const smtk::mesh::ResourcePtr& mr)
{
  smtk::mesh::PointConnectivity twoDim = mr->cells(smtk::mesh::Dims2).pointConnectivity();
  smtk::mesh::PointConnectivity twoDimV2 = mr->cells(smtk::mesh::Dims2).pointConnectivity();
  test(twoDim == twoDimV2);

  smtk::mesh::CellTypes ctypes_only_2d(std::string("000011100"));
  smtk::mesh::PointConnectivity twoDim_from_ctype = mr->cells(ctypes_only_2d).pointConnectivity();
  test(twoDim == twoDim_from_ctype);

  smtk::mesh::MeshSet meshes = mr->meshes();
  test(twoDim != meshes.pointConnectivity());
  test(twoDim == meshes.cells(smtk::mesh::Dims2).pointConnectivity());
  test(twoDim == meshes.cells(ctypes_only_2d).pointConnectivity());

  //now try by appending multiple CellSets together to form

  smtk::mesh::CellSet cs_tri = meshes.cells(smtk::mesh::Triangle);
  smtk::mesh::CellSet cs_qua = meshes.cells(smtk::mesh::Quad);
  smtk::mesh::CellSet cs_pol = meshes.cells(smtk::mesh::Polygon);

  smtk::mesh::CellSet cs_all(cs_tri);
  cs_all.append(cs_qua);
  cs_all.append(cs_pol);
  test(twoDim == cs_all.pointConnectivity());

  //try appending the same cells twice and verify the connectivity is the same
  cs_all.append(cs_qua);
  cs_all.append(cs_pol);
  test(twoDim == cs_all.pointConnectivity());
}

void verify_iteration(const smtk::mesh::ResourcePtr& mr)
{

  //fetch all 2d cells.
  //grab the number of cells, and total size.
  //iterate all the cells and count how many we visit and sum the length
  //verify all number match
  smtk::mesh::PointConnectivity twoDim = mr->cells(smtk::mesh::Dims2).pointConnectivity();

  const std::size_t reportedNumCells = twoDim.numberOfCells();
  const std::size_t reportedNumVerts = twoDim.size();

  std::size_t actualNumCells = 0;
  std::size_t actualNumVerts = 0;

  smtk::mesh::CellType cellType;
  smtk::mesh::CellTypes allCellTypesSeen;

  int size = 0;
  const smtk::mesh::Handle* points;
  for (twoDim.initCellTraversal(); twoDim.fetchNextCell(cellType, size, points);)
  {
    ++actualNumCells;
    actualNumVerts += static_cast<std::size_t>(size);

    allCellTypesSeen[cellType] = true;
    // mr->debugDump( points );
  }
  smtk::mesh::TypeSet typeSet(allCellTypesSeen, false, true);

  //verify that the cell types that are reported are only 2D cells.
  test(!typeSet.hasDimension(smtk::mesh::Dims1));
  test(typeSet.hasDimension(smtk::mesh::Dims2));
  test(!typeSet.hasDimension(smtk::mesh::Dims3));

  test(reportedNumCells == actualNumCells);
  test(reportedNumVerts == actualNumVerts);
}

void verify_shared_iteration(const smtk::mesh::ResourcePtr& mr)
{

  //fetch all 2d cells.
  //make a copy of the connectivity
  //reset iteration on both objects
  //using the copy iterate over 10 cells

  //verify we iterate over all cells with the original
  //verify we iterate over the rest of the cells with the copy

  //this shows that the iterator info isn't stored in the shared_ptr between
  //the two connectivity objects, but is unique for both.
  smtk::mesh::PointConnectivity twoDim = mr->cells(smtk::mesh::Dims2).pointConnectivity();
  smtk::mesh::PointConnectivity twoDimCopy = twoDim;

  const std::size_t reportedNumCells = twoDim.numberOfCells();
  const std::size_t reportedNumVerts = twoDim.size();

  twoDim.initCellTraversal();
  twoDimCopy.initCellTraversal();

  int size = 0;
  const smtk::mesh::Handle* points;
  for (int i = 0; i < 10; twoDimCopy.fetchNextCell(size, points), ++i)
    ;

  std::size_t actualNumCells = 0;
  std::size_t actualNumVerts = 0;
  while (twoDim.fetchNextCell(size, points))
  {
    ++actualNumCells;
    actualNumVerts += static_cast<std::size_t>(size);
  }

  //this verifies that twoDim isn't using the iterator state from twoDimCopy
  test(reportedNumCells == actualNumCells);
  test(reportedNumVerts == actualNumVerts);

  //make sure the iterator on twoDimCopy keeps going
  for (std::size_t i = 10; i < reportedNumCells; ++i)
  {
    //if we iterate too far this will through an exception
    test(twoDimCopy.fetchNextCell(size, points));
  }
}
}

int UnitTestPointConnectivity(int /*unused*/, char** const /*unused*/)
{
  smtk::mesh::ResourcePtr mr = load_mesh();

  verify_constructors(mr);
  verify_empty(mr);

  verify_all_connecitivity(mr);

  verify_simple_equiv(mr);
  verify_complex_equiv(mr);

  verify_iteration(mr);
  verify_shared_iteration(mr);

  return 0;
}
