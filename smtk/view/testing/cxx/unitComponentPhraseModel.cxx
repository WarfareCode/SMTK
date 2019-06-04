//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#include "smtk/view/ComponentPhraseModel.h"
#include "smtk/view/DescriptivePhrase.h"
#include "smtk/view/SubphraseGenerator.h"

#include "smtk/session/polygon/Registrar.h"

#include "smtk/common/Registry.h"

#include "smtk/resource/Manager.h"

#include "smtk/operation/Manager.h"
#include "smtk/operation/operators/ReadResource.h"

#include "smtk/attribute/Attribute.h"
#include "smtk/attribute/FileItem.h"
#include "smtk/attribute/IntItem.h"
#include "smtk/attribute/ModelEntityItem.h"
#include "smtk/attribute/StringItem.h"
#include "smtk/attribute/VoidItem.h"

#include "smtk/model/SessionRef.h"
#include "smtk/model/operators/EntityGroupOperation.h"

#include "smtk/common/testing/cxx/helpers.h"
#include "smtk/model/testing/cxx/helpers.h"

#include "smtk/AutoInit.h"

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include <stdlib.h>
#include <string.h>

using smtk::shared_ptr;
using namespace smtk::common;
using namespace smtk::view;
using namespace smtk::io;

static int maxIndent = 6;
static std::vector<char*> dataArgs;

namespace
{

int printer(DescriptivePhrasePtr p, const std::vector<int>& idx)
{
  int indent = static_cast<int>(idx.size()) * 2;
  if (p)
  {
    std::cout << std::string(indent, ' ') << p->title() << "  (" << p->subtitle() << ")";
    smtk::resource::FloatList rgba = p->relatedColor();
    if (rgba[3] >= 0.)
    {
      std::cout << " rgba(" << rgba[0] << "," << rgba[1] << "," << rgba[2] << "," << rgba[3] << ")";
    }
    auto sub = p->subphrases(); // force subphrases to get built, though we may not visit them
    (void)sub;
    std::cout << "\n";
    std::ostringstream idxStr;
    for (int ii : idx)
    {
      idxStr << " " << ii;
    }
    smtkTest(
      p->at(idx) == p, "Index " << idxStr.str() << " passed to visitor did not produce phrase!");
  }
  return indent > maxIndent ? 1 : 0;
}

} // namespace

int unitComponentPhraseModel(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::string testFile;
    testFile = SMTK_DATA_DIR;
    testFile += "/model/2d/smtk/epic-trex-drummer.smtk";
    dataArgs.push_back(argv[0]);
    dataArgs.push_back(strdup(testFile.c_str()));
    dataArgs.push_back(nullptr);
    argc = 2;
    argv = &dataArgs[0];
  }
  auto rsrcMgr = smtk::resource::Manager::create();
  auto operMgr = smtk::operation::Manager::create();
  operMgr->registerResourceManager(rsrcMgr);

  auto registry = smtk::common::Registry<smtk::session::polygon::Registrar, smtk::resource::Manager,
    smtk::operation::Manager>(rsrcMgr, operMgr);

  // I. Construct a ComponentPhraseModel that displays edges and faces. Load some geometry.
  auto phraseModel = smtk::view::ComponentPhraseModel::create();
  std::multimap<std::string, std::string> filters;
  filters.insert(
    std::make_pair(std::string("smtk::session::polygon::Resource"), std::string("edge")));
  filters.insert(
    std::make_pair(std::string("smtk::session::polygon::Resource"), std::string("face")));
  phraseModel->setComponentFilters(filters);
  phraseModel->addSource(rsrcMgr, operMgr, nullptr);
  smtk::resource::ResourceArray rsrcs;
  for (int i = 1; i < argc; i++)
  {
    auto rdr = operMgr->create<smtk::operation::ReadResource>();
    rdr->parameters()->findFile("filename")->setValue(argv[i]);
    rdr->operate();
    // rsrcs.push_back(rsrcMgr->read<smtk::session::polygon::Resource>(argv[1]));
  }
  smtk::resource::ResourcePtr rsrc = nullptr;
  std::for_each(rsrcMgr->resources().begin(), rsrcMgr->resources().end(),
    [&rsrc](const smtk::resource::ResourcePtr& rr) {
      if (rr && !rsrc)
      {
        rsrc = rr;
      }
    });
  smtkTest(!!rsrc, "Unable to discern that a resource was loaded.");
  auto firstSize = phraseModel->root()->subphrases().size();

  test(phraseModel->root()->root() == phraseModel->root(),
    "Model's root phrase was not root of tree.");
  phraseModel->root()->visitChildren(printer);

  // II. Now change the filters to a reduced set and verify there are fewer phrases.
  filters.erase(filters.begin());
  std::cout << "---\n";

  phraseModel->setComponentFilters(filters);
  auto reducedSize = phraseModel->root()->subphrases().size();
  std::cout << "initial size " << firstSize << " reduced to " << reducedSize << "\n";
  smtkTest(reducedSize < firstSize, "Expected fewer phrases without the edge filter.");
  phraseModel->root()->visitChildren(printer);

  if (!dataArgs.empty())
  {
    // III. Test that we can set an explicit "active" resource that
    //      serves as the only source of components.
    phraseModel->setOnlyShowActiveResourceComponents(true);
    std::cout << "---\n";
    // III.a. A null "active" resource
    smtkTest(phraseModel->root()->subphrases().empty(),
      "Expected an empty list with a null active resource.");
    // III.b. A non-null "active" resource
    phraseModel->setActiveResource(rsrc);
    std::cout << "---\n";
    phraseModel->root()->visitChildren(printer);
    smtkTest(!phraseModel->root()->subphrases().empty(),
      "Expected a non-empty list with a non-null active resource.");
    smtkTest(phraseModel->root()->subphrases().size() == reducedSize,
      "Expected the same number of phrases as earlier.");

    // IV. Test updates due to an operation.

    // Grab the faces as reported to us:
    smtk::resource::ComponentArray faces;
    smtk::model::EntityPtr fmod;
    phraseModel->root()->visitChildren(
      [&faces, &fmod](DescriptivePhrasePtr p, const std::vector<int>&) {
        auto comp = p->relatedComponent();
        faces.push_back(comp);
        auto ment = std::dynamic_pointer_cast<smtk::model::Entity>(comp);
        fmod = ment ? ment->owningModel() : nullptr;
        return 0;
      });
    smtkTest(!fmod || !faces.empty(), "Cannot test grouping without groupees.");

    // Add a filter so groups will be listed at the top level:
    filters.insert(
      std::make_pair(std::string("smtk::session::polygon::Resource"), std::string("group")));
    phraseModel->setComponentFilters(filters);
    auto sizeBeforeAdd = phraseModel->root()->subphrases().size();
    // Create a new group
    auto op = operMgr->create<smtk::model::EntityGroupOperation>();
    auto pm = op->parameters();
    pm->findString("Operation")->setValue("Create");
    pm->findString("group name")->setValue("everything");
    pm->findVoid("Face")->setIsEnabled(true);
    // pm->findModelEntity("cell to add")->setObjectValues(faces.begin(), faces.end());
    pm->associations()->appendObjectValue(fmod);
    auto res = op->operate();
    std::cout << "add group success "
              << (res->findInt("outcome")->value(0) ==
                       static_cast<int>(smtk::operation::Operation::Outcome::SUCCEEDED)
                     ? "T"
                     : "F")
              << "\n";
    (void)res;
    auto sizeAfterAdd = phraseModel->root()->subphrases().size();
    std::cout << "---\n";
    phraseModel->root()->visitChildren(printer);
    smtkTest(
      sizeAfterAdd == sizeBeforeAdd + 1, "Adding a group should increase number of phrases by 1.");

    // Don't leak
    free(dataArgs[1]);
  }

  return 0;
}
