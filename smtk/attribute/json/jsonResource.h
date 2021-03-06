//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#ifndef smtk_attribute_jsonResource_h
#define smtk_attribute_jsonResource_h

#include "nlohmann/json.hpp"
#include "smtk/PublicPointerDefs.h"
#include "smtk/attribute/Resource.h"
#include "smtk/attribute/json/jsonDefinition.h"
#include "smtk/io/Logger.h"
#include "smtk/view/json/jsonView.h"

#include "smtk/CoreExports.h"

#include <string>

namespace smtk
{
namespace attribute
{
using json = nlohmann::json;

/**\brief Provide a way to serialize Resource. It would stick with attribute
  * V3 format
  */
/// Convert a SelectionManager's currentSelection() to JSON.
SMTKCORE_EXPORT void to_json(json& j, const smtk::attribute::ResourcePtr& col);

SMTKCORE_EXPORT void from_json(const json& j, smtk::attribute::ResourcePtr& col);
}
}

#endif
