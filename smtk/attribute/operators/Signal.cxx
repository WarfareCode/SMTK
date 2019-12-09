//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

#include "smtk/attribute/operators/Signal.h"

#include "smtk/attribute/Signal_xml.h"

#include "smtk/attribute/Attribute.h"
#include "smtk/attribute/ComponentItem.h"
#include "smtk/attribute/Resource.h"
#include "smtk/attribute/StringItem.h"
#include "smtk/attribute/VoidItem.h"

#include "smtk/io/Logger.h"

namespace smtk
{
namespace attribute
{

Signal::Result Signal::operateInternal()
{
  auto params = this->parameters();
  auto result = this->createResult(smtk::operation::Operation::Outcome::SUCCEEDED);
  auto modIn = params->findComponent("modified");
  auto itemsIn = params->findString("items");
  auto creIn = params->findComponent("created");
  auto expIn = params->findComponent("expunged");
  auto updateIn = params->findVoid("update");
  auto modOut = result->findComponent("modified");
  auto itemsOut = result->findString("items");
  auto creOut = result->findComponent("created");
  auto expOut = result->findComponent("expunged");
  auto updateOut = params->findVoid("update");

  // Copy the inputs to the output.
  creOut->setObjectValues(creIn->begin(), creIn->end());
  modOut->setObjectValues(modIn->begin(), modIn->end());
  itemsOut->setValues(itemsIn->begin(), itemsIn->end());
  expOut->setObjectValues(expIn->begin(), expIn->end());
  updateOut->setIsEnabled(updateIn->isEnabled());

  return result;
}

void Signal::generateSummary(Operation::Result& /*unused*/)
{
}

const char* Signal::xmlDescription() const
{
  return Signal_xml;
}
}
}
