//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

#include "smtk/io/ModelToMesh.h"
#include "smtk/io/WriteMesh.h"

#include "smtk/mesh/core/Collection.h"

#include "smtk/model/EntityIterator.h"
#include "smtk/model/Resource.h"
#include "smtk/model/Volume.h"
#include "smtk/model/json/jsonResource.h"
#include "smtk/model/json/jsonTessellation.h"

#include "smtk/mesh/testing/cxx/helpers.h"
#include "smtk/model/testing/cxx/helpers.h"

#include <fstream>
#include <sstream>

//force to use filesystem version 3
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

namespace
{

//SMTK_DATA_DIR is a define setup by cmake
std::string data_root = SMTK_DATA_DIR;

void create_simple_mesh_model(smtk::model::ResourcePtr resource)
{
  std::string file_path(data_root);
  file_path += "/model/2d/smtk/test2D.json";

  std::ifstream file(file_path.c_str());

  std::string json_str((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
  nlohmann::json json = nlohmann::json::parse(json_str);

  smtk::model::from_json(json, resource);
  for (auto& tessPair : json["tessellations"])
  {
    smtk::common::UUID id = tessPair[0];
    smtk::model::Tessellation tess = tessPair[1];
    resource->setTessellation(id, tess);
  }

  resource->assignDefaultNames();

  file.close();
}

void verify_empty_model()
{
  smtk::model::ResourcePtr modelResource = smtk::model::Resource::create();

  smtk::io::ModelToMesh convert;
  smtk::mesh::CollectionPtr c = convert(modelResource);
  test(!c, "collection should be invalid for an empty model");
}

void verify_model_association()
{
  smtk::model::ResourcePtr modelResource = smtk::model::Resource::create();

  create_simple_mesh_model(modelResource);

  smtk::io::ModelToMesh convert;
  smtk::mesh::CollectionPtr c = convert(modelResource);

  //we need to verify that the collection is now has an associated model
  test(c->hasAssociations(), "collection should have associations");
  test((c->associatedModel() != smtk::common::UUID()),
    "collection should be associated to a real model");
  test((c->isAssociatedToModel()), "collection should be associated to a real model");

  //verify the MODEL_ENTITY is correct
  smtk::model::EntityRefs currentModels =
    modelResource->entitiesMatchingFlagsAs<smtk::model::EntityRefs>(smtk::model::MODEL_ENTITY);
  if (currentModels.size() > 0)
  { //presuming only a single model in the model resource
    test((c->associatedModel() == currentModels.begin()->entity()),
      "collection associated model should match model resource");
  }
}

void verify_cell_conversion()
{
  smtk::model::ResourcePtr modelResource = smtk::model::Resource::create();

  create_simple_mesh_model(modelResource);

  smtk::io::ModelToMesh convert;
  smtk::mesh::CollectionPtr c = convert(modelResource);
  test(c->isValid(), "collection should be valid");
  test(c->numberOfMeshes() == 21, "collection should have a mesh per tet");

  //confirm that we have the proper number of volume cells
  smtk::mesh::CellSet tri_cells = c->cells(smtk::mesh::Dims2);
  test(tri_cells.size() == 45);

  smtk::mesh::CellSet edge_cells = c->cells(smtk::mesh::Dims1);
  test(edge_cells.size() == 32);

  smtk::mesh::CellSet vert_cells = c->cells(smtk::mesh::Dims0);
  test(vert_cells.size() == 7);
}

void verify_vertex_conversion()
{
  smtk::model::ResourcePtr modelResource = smtk::model::Resource::create();

  create_simple_mesh_model(modelResource);

  smtk::io::ModelToMesh convert;
  smtk::mesh::CollectionPtr c = convert(modelResource);
  test(c->isValid(), "collection should be valid");
  test(c->numberOfMeshes() == 21, "collection should have a mesh per tet");

  //make sure the merging points from ModelToMesh works properly
  smtk::mesh::PointSet points = c->points();
  test(points.size() == 32, "We should have 32 points");

  //verify that after merging points we haven't deleted any of the cells
  //that represent a model vert
  smtk::mesh::CellSet vert_cells = c->cells(smtk::mesh::Dims0);
  test(vert_cells.size() == 7);
}
}

int UnitTestModelToMesh2D(int, char** const)
{
  verify_empty_model();
  verify_model_association();
  verify_cell_conversion();
  verify_vertex_conversion();
  return 0;
}
