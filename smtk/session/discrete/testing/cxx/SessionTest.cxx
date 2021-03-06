//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

#include "smtk/attribute/Attribute.h"
#include "smtk/attribute/ComponentItem.h"
#include "smtk/attribute/DoubleItem.h"
#include "smtk/attribute/FileItem.h"
#include "smtk/attribute/IntItem.h"
#include "smtk/attribute/ReferenceItem.h"

#include "smtk/model/json/jsonResource.h"
#include "smtk/session/discrete/Resource.h"
#include "smtk/session/discrete/Session.h"
#include "smtk/session/discrete/operators/ReadOperation.h"
#include "smtk/session/discrete/operators/SplitFaceOperation.h"

#include "smtk/model/EntityRef.h"
#include "smtk/model/Face.h"
#include "smtk/model/Group.h"
#include "smtk/model/Model.h"
#include "smtk/model/Resource.h"
#include "smtk/model/Tessellation.h"

#include "smtk/operation/Manager.h"

#include <fstream>

using namespace smtk::model;

int main(int argc, char* argv[])
{
  if (argc < 2)
    return 1;

  // Create a resource manager
  smtk::resource::Manager::Ptr resourceManager = smtk::resource::Manager::create();

  // Register the resource to the resource manager
  {
    resourceManager->registerResource<smtk::session::discrete::Resource>();
  }

  // Create an operation manager
  smtk::operation::Manager::Ptr operationManager = smtk::operation::Manager::create();

  // Register the operators to the operation manager
  {
    operationManager->registerOperation<smtk::session::discrete::ReadOperation>(
      "smtk::session::discrete::ReadOperation");
    operationManager->registerOperation<smtk::session::discrete::SplitFaceOperation>(
      "smtk::session::discrete::SplitFaceOperation");
  }

  // Register the resource manager to the operation manager (newly created
  // resources will be automatically registered to the resource manager).
  operationManager->registerResourceManager(resourceManager);

  smtk::model::Entity::Ptr model;

  {
    // Create a read operator
    smtk::session::discrete::ReadOperation::Ptr readOp =
      operationManager->create<smtk::session::discrete::ReadOperation>();
    if (!readOp)
    {
      std::cerr << "No read operator\n";
      return 1;
    }

    // Set the file path
    readOp->parameters()->findFile("filename")->setValue(std::string(argv[1]));

    // Execute the operation
    smtk::operation::Operation::Result readOpResult = readOp->operate();

    // Retrieve the resulting model
    smtk::attribute::ComponentItemPtr componentItem =
      std::dynamic_pointer_cast<smtk::attribute::ComponentItem>(
        readOpResult->findComponent("model"));

    // Access the generated model
    model = std::dynamic_pointer_cast<smtk::model::Entity>(componentItem->value());

    // Test for success
    if (readOpResult->findInt("outcome")->value() !=
      static_cast<int>(smtk::operation::Operation::Outcome::SUCCEEDED))
    {
      std::cerr << "Read operator failed\n";
      return 1;
    }
  }

  smtk::session::discrete::Resource::Ptr resource =
    std::dynamic_pointer_cast<smtk::session::discrete::Resource>(model->resource());
  resource->assignDefaultNames(); // should force transcription of every entity, but doesn't yet.

  // Test a model operator (if some argument beyond filename is given)
  if (argc > 2)
  {
    // Find a face with more than 2 triangles
    smtk::model::Faces allFaces;
    smtk::model::EntityRef::EntityRefsFromUUIDs(
      allFaces, resource, resource->entitiesMatchingFlags(smtk::model::FACE));
    smtk::model::Face f;
    for (smtk::model::Faces::iterator it = allFaces.begin(); it != allFaces.end(); ++it)
    {
      f = *it;
      const smtk::model::Tessellation* tess = f.hasTessellation();
      if (tess && tess->conn().size() > 8)
        break;
    }
    if (f.isValid() && f.hasTessellation()->conn().size() > 8)
    {
      std::cout << "Attempting face split\n";

      // Create a split face operator
      smtk::session::discrete::SplitFaceOperation::Ptr splitFaceOp =
        operationManager->create<smtk::session::discrete::SplitFaceOperation>();
      if (!splitFaceOp)
      {
        std::cerr << "No split face operator\n";
        return 1;
      }

      auto faceToSplit = splitFaceOp->parameters()->associations();
      faceToSplit->setNumberOfValues(1);
      faceToSplit->setValue(f.component());
      splitFaceOp->parameters()->findComponent("model")->setValue(
        resource->entitiesMatchingFlagsAs<Models>(smtk::model::MODEL_ENTITY).front().component());
      splitFaceOp->parameters()->findDouble("feature angle")->setValue(15.0);
      smtk::session::discrete::SplitFaceOperation::Result result = splitFaceOp->operate();
      std::cout << "  Face is " << f.name() << " (" << f.entity() << ")\n";
      std::cout << "  " << (result->findInt("outcome")->value() ==
                                 static_cast<int>(smtk::operation::Operation::Outcome::SUCCEEDED)
                               ? "OK"
                               : "Failed")
                << "\n";
    }
    else if (f.isValid())
    {
      std::cout << "No faces to split\n";
    }

    smtk::model::EntityRefArray exports;
    exports.push_back(model);
    resource->discreteSession()->ExportEntitiesToFileOfNameAndType(
      exports, "sessionTest.cmb", "cmb");
    std::cout << "  done\n";
  }

  nlohmann::json json = resource;
  if (!json.empty())
  {
    std::ofstream jsonFile("sessionTest.json");
    jsonFile << json;
    jsonFile.close();
  }

  return 0;
}
