//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#include "smtk/attribute/Definition.h"
#include "smtk/attribute/FileItem.h"
#include "smtk/attribute/GroupItemDefinition.h"
#include "smtk/attribute/IntItem.h"
#include "smtk/attribute/ReferenceItem.h"
#include "smtk/attribute/Resource.h"
#include "smtk/attribute/ResourceItem.h"
#include "smtk/attribute/StringItemDefinition.h"
#include "smtk/attribute/VoidItemDefinition.h"
#include "smtk/attribute/operators/Read.h"
#include "smtk/attribute/operators/Write.h"
#include "smtk/io/AttributeReader.h"
#include "smtk/io/AttributeWriter.h"
#include "smtk/io/Logger.h"

#include "smtk/common/testing/cxx/helpers.h"

using namespace smtk::attribute;
using namespace smtk::common;
using namespace smtk;

namespace
{
bool compareSets(const std::set<std::string>& test, const std::set<std::string>& truth)
{
  if (test == truth)
  {
    std::cerr << "Passed\n";
    return true;
  }
  bool first = true;
  for (const auto& s : truth)
  {
    if (test.find(s) == test.end())
    {
      if (first)
      {
        std::cerr << " Missing:";
        first = false;
      }
      std::cerr << " " << s;
    }
  }
  if (!first)
  {
    std::cerr << std::endl;
  }
  first = true;
  for (const auto& s : test)
  {
    if (truth.find(s) == truth.end())
    {
      if (first)
      {
        std::cerr << " Extra:";
        first = false;
      }
      std::cerr << " " << s;
    }
  }
  if (!first)
  {
    std::cerr << std::endl;
  }
  return false;
}

bool testCategories(const ItemDefinitionPtr& idef, const std::string& prefix,
  const std::map<std::string, std::set<std::string> >& answers)
{
  bool status = true;
  auto gidef = std::dynamic_pointer_cast<GroupItemDefinition>(idef);
  if (gidef != nullptr)
  {
    int i, n = static_cast<int>(gidef->numberOfItemDefinitions());
    for (i = 0; i < n; i++)
    {
      if (!testCategories(gidef->itemDefinition(i), prefix, answers))
      {
        status = false;
      }
    }
  }
  else
  {
    auto videf = std::dynamic_pointer_cast<ValueItemDefinition>(idef);
    if (videf != nullptr)
    {
      for (const auto& cidef : videf->childrenItemDefinitions())
      {
        if (!testCategories(cidef.second, prefix, answers))
        {
          status = false;
        }
      }
      // Additional Checks for s1
      if (videf->name() == "s1")
      {
        const std::vector<std::set<std::string> > enumCats = { { "ec1" }, { "ec2" }, {} };
        const std::vector<bool> enumAL = { true, false, true };
        std::size_t i, n = videf->numberOfDiscreteValues();
        if (n != 3)
        {
          std::cerr << prefix << " Testing Item Definition:" << idef->name()
                    << " number of enums = " << n << " not 3\n";
          status = false;
        }
        for (i = 0; i < n; i++)
        {
          std::string e = videf->discreteEnum(i);
          std::cerr << prefix << " Testing Item Definition:" << idef->name() << " enum = " << e
                    << "'s Categories: ";
          if (!compareSets(videf->enumCategories(e).categoryNames(), enumCats[i]))
          {
            status = false;
          }
          if (videf->hasEnumAdvanceLevel(e) != enumAL[i])
          {
            std::cerr << prefix << " Testing Item Definition:" << idef->name() << " enum = " << e
                      << "'s Advance Level: Failed!\n";
            status = false;
          }

          if (videf->hasEnumAdvanceLevel(e) && (videf->enumAdvanceLevel(e) != 10))
          {
            std::cerr << prefix << " Testing Item Definition:" << idef->name() << " enum = " << e
                      << "'s Advance Level Value: " << videf->enumAdvanceLevel(e)
                      << " should be 10!\n";
            status = false;
          }
        }
      }
    }
  }

  std::cerr << prefix << " Testing Item Definition:" << idef->name() << "'s Categories: ";
  if (!compareSets(idef->categories().categoryNames(), answers.at(idef->name())))
  {
    status = false;
  }
  return status;
}

bool testCategories(const DefinitionPtr& def, const std::string& prefix,
  const std::map<std::string, std::set<std::string> >& answers)
{
  int i, n = static_cast<int>(def->numberOfItemDefinitions());
  bool status = true;
  for (i = 0; i < n; i++)
  {
    if (!testCategories(def->itemDefinition(i), prefix, answers))
    {
      status = false;
    }
  }

  std::cerr << prefix << " Testing Definition:" << def->type() << "'s Categories: ";
  if (!compareSets(def->categories().categoryNames(), answers.at(def->type())))
  {
    status = false;
  }
  return status;
}

bool testCategories(const attribute::ResourcePtr& attRes, const std::string& prefix,
  const std::map<std::string, std::set<std::string> >& answers)
{
  std::vector<smtk::attribute::DefinitionPtr> defs;
  attRes->definitions(defs);
  bool status = true;
  for (const auto& def : defs)
  {
    if (!testCategories(def, prefix, answers))
    {
      status = false;
    }
  }

  std::cerr << prefix << " Testing Resource's Categories: ";
  if (!compareSets(attRes->categories(), answers.at("resource")))
  {
    status = false;
  }
  return status;
}

void setupAttributeResource(attribute::ResourcePtr& attRes)
{
  DefinitionPtr A = attRes->createDefinition("A");
  A->localCategories().insert("A");
  GroupItemDefinitionPtr gItemDef0 = A->addItemDefinition<GroupItemDefinition>("g1");
  gItemDef0->localCategories().insert("g1");
  // Lets create a Discrete String Item with Enums having category and advance level info
  // assigned to some of them
  StringItemDefinitionPtr sItemDef0 = gItemDef0->addItemDefinition<StringItemDefinition>("s1");
  sItemDef0->localCategories().insert("s1");
  sItemDef0->addDiscreteValue("a", "e1");
  sItemDef0->addDiscreteValue("b", "e2");
  sItemDef0->addDiscreteValue("c", "e3");
  sItemDef0->addEnumCategory("e1", "ec1");
  sItemDef0->addEnumCategory("e2", "ec2");
  sItemDef0->setEnumAdvanceLevel("e1", 10);
  sItemDef0->setEnumAdvanceLevel("e3", 10);
  StringItemDefinitionPtr sItemDef1 = sItemDef0->addItemDefinition<StringItemDefinition>("s2");
  sItemDef1->localCategories().insert("s2");
  sItemDef1->setIsOkToInherit(false);
  VoidItemDefinitionPtr vItemDef = sItemDef1->addItemDefinition<VoidItemDefinition>("v1");
  vItemDef->localCategories().insert("v1");
  GroupItemDefinitionPtr gItemDef1 = gItemDef0->addItemDefinition<GroupItemDefinition>("g2");
  gItemDef1->localCategories().insert("g2");
  gItemDef1->setIsOkToInherit(false);
  sItemDef0 = gItemDef1->addItemDefinition<StringItemDefinition>("s3");
  sItemDef0->localCategories().insert("s3");
  DefinitionPtr B = attRes->createDefinition("B", A);
  B->localCategories().insert("B");
  vItemDef = B->addItemDefinition<VoidItemDefinition>("v2");
  vItemDef->localCategories().insert("v2");
  DefinitionPtr C = attRes->createDefinition("C", A);
  C->localCategories().insert("C");
  DefinitionPtr D = attRes->createDefinition("D", A);
  vItemDef = D->addItemDefinition<VoidItemDefinition>("v3");
  vItemDef->localCategories().insert("v3");
  DefinitionPtr E = attRes->createDefinition("E");
  E->localCategories().insert("E");
  DefinitionPtr F = attRes->createDefinition("F");
  vItemDef = F->addItemDefinition<VoidItemDefinition>("v4");
  attRes->finalizeDefinitions();
}
}

int unitCategories(int /*unused*/, char* /*unused*/ [])
{
  //
  // I. Let's create an attribute resource and some definitions
  attribute::ResourcePtr attRes = attribute::Resource::create();
  setupAttributeResource(attRes);

  // Lets define what the categories should be for this resource
  std::map<std::string, std::set<std::string> > answers = {
    { "A", { "A", "g1", "g2", "s1", "s2", "s3", "v1" } },
    { "B", { "A", "g1", "g2", "s1", "s2", "s3", "v1", "B", "v2" } },
    { "C", { "A", "g1", "g2", "s1", "s2", "s3", "v1", "C" } },
    { "D", { "A", "g1", "g2", "s1", "s2", "s3", "v1", "v3" } }, { "E", { "E" } }, { "F", {} },
    { "g1", { "A", "g1", "g2", "s1", "s2", "s3", "v1" } }, { "g2", { "g2", "s3" } },
    { "s1", { "A", "g1", "s1", "s2", "v1", "ec1", "ec2" } }, { "s2", { "s2", "v1" } },
    { "s3", { "g2", "s3" } }, { "v1", { "s2", "v1" } }, { "v2", { "A", "B", "v2" } },
    { "v3", { "A", "v3" } }, { "v4", {} },
    { "resource", { "A", "g1", "g2", "s1", "s2", "s3", "v1", "B", "v2", "C", "v3", "E" } }
  };
  smtkTest(
    testCategories(attRes, "First Pass - ", answers), "Failed checking Categories in First Pass");
  io::AttributeWriter writer;
  io::AttributeReader reader;
  io::Logger logger;
  std::string writeRroot(SMTK_SCRATCH_DIR);
  std::string fname = writeRroot + "/unitAttributeCategoriesTest.sbi";
  std::string rname = writeRroot + "/unitAttributeCategoriesTest.smtk";

  //Test JSON File I/O
  attRes->setLocation(rname);
  smtk::attribute::Write::Ptr writeOp = smtk::attribute::Write::create();
  writeOp->parameters()->associate(attRes);
  auto opresult = writeOp->operate();

  smtkTest(opresult->findInt("outcome")->value() ==
      static_cast<int>(smtk::operation::Operation::Outcome::SUCCEEDED),
    "JSON Write operation failed\n"
      << writeOp->log().convertToString());
  attRes = nullptr;
  smtk::attribute::Read::Ptr readOp = smtk::attribute::Read::create();
  readOp->parameters()->findFile("filename")->setValue(rname);
  opresult = readOp->operate();
  smtkTest(opresult->findInt("outcome")->value() ==
      static_cast<int>(smtk::operation::Operation::Outcome::SUCCEEDED),
    "JSON Read operation failed\n"
      << writeOp->log().convertToString());
  attRes = std::dynamic_pointer_cast<smtk::attribute::Resource>(
    opresult->findResource("resource")->value());
  //Test the resource created using JSON
  smtkTest(
    testCategories(attRes, "JSON Pass - ", answers), "Failed checking Categories in JSON Pass");

  //Test XML File I/O
  writer.write(attRes, fname, logger);
  smtkTest(!logger.hasErrors(), "Error Generated when XML writing file ("
      << fname << "):\n"
      << logger.convertToString());

  attRes = attribute::Resource::create();
  reader.read(attRes, fname, logger);
  smtkTest(!logger.hasErrors(), "Error Generated when XML reading file ("
      << fname << "):\n"
      << logger.convertToString());
  //Test the resource created using XML
  smtkTest(
    testCategories(attRes, "XML Pass - ", answers), "Failed checking Categories in XML Pass");

  return 0;
}
