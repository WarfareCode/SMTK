//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#ifndef __smtk_attribute_MeshSelectionItem_h
#define __smtk_attribute_MeshSelectionItem_h

#include "smtk/CoreExports.h"
#include "smtk/PublicPointerDefs.h"
#include "smtk/attribute/Item.h"
#include <map>
#include <set>
#include <string>

namespace smtk
{
namespace attribute
{

class MeshSelectionItemDefinition;

/// Enumeration of mesh values modification type.
enum MeshModifyMode
{
  NONE,              //!< Cancel current operation mode
  RESET,             //!< Reset the existing list)
  MERGE,             //!< Append to the existing list
  SUBTRACT,          //!< Subtract from existing list
  ACCEPT,            //!< Accept the existing list)
  NUM_OF_MODIFYMODES //!< The number of mesh modify modes.
};

/**\brief Provide a way for an attribute to refer to mesh entities.
  *
  */
class SMTKCORE_EXPORT MeshSelectionItem : public Item
{
public:
  typedef std::map<smtk::common::UUID, std::set<int> >::const_iterator const_sel_map_it;

  smtkTypeMacro(smtk::attribute::MeshSelectionItem);
  ~MeshSelectionItem() override;
  Item::Type type() const override;

  using Item::isValid;
  bool isValid(const std::set<std::string>& cats) const override;

  void setValues(const smtk::common::UUID&, const std::set<int>&);
  void unionValues(const smtk::common::UUID&, const std::set<int>&);
  void removeValues(const smtk::common::UUID&, const std::set<int>&);
  void setModifyMode(MeshModifyMode mode) { m_modifyMode = mode; }
  MeshModifyMode modifyMode() const { return m_modifyMode; }
  void setCtrlKeyDown(bool val) { m_isCtrlKeyDown = val; }
  bool isCtrlKeyDown() const { return m_isCtrlKeyDown; }

  std::size_t numberOfValues() const;
  const std::set<int>& values(const smtk::common::UUID&);
  void reset() override;
  // Assigns this item to be equivalent to another.  Options are currently not used.
  // Returns true if success and false if a problem occured
  bool assign(smtk::attribute::ConstItemPtr& sourceItem, unsigned int options = 0) override;

  const_sel_map_it begin() const;
  const_sel_map_it end() const;

  static std::string modifyMode2String(MeshModifyMode m);
  static MeshModifyMode string2ModifyMode(const std::string& s);

protected:
  friend class MeshSelectionItemDefinition;

  MeshSelectionItem(Attribute* owningAttribute, int itemPosition);
  MeshSelectionItem(Item* owningItem, int position, int subGroupPosition);
  bool setDefinition(smtk::attribute::ConstItemDefinitionPtr vdef) override;
  std::map<smtk::common::UUID, std::set<int> > m_selectionValues;
  MeshModifyMode m_modifyMode;
  bool m_isCtrlKeyDown;
};

} // namespace attribute
} // namespace smtk

#endif // __smtk_attribute_MeshSelectionItem_h
