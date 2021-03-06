//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#ifndef __smtk_mesh_DeleteMesh_h
#define __smtk_mesh_DeleteMesh_h

#include "smtk/operation/XMLOperation.h"

namespace smtk
{
namespace mesh
{

/**\brief Delete a mesh.
  */
class SMTKCORE_EXPORT DeleteMesh : public smtk::operation::XMLOperation
{
public:
  smtkTypeMacro(smtk::mesh::DeleteMesh);
  smtkCreateMacro(DeleteMesh);
  smtkSharedFromThisMacro(smtk::operation::Operation);
  smtkSuperclassMacro(smtk::operation::XMLOperation);

  void suppressOutput(bool choice) { m_suppressOutput = choice; }

protected:
  DeleteMesh();
  Result operateInternal() override;
  virtual const char* xmlDescription() const override;

  void generateSummary(smtk::operation::Operation::Result&) override;

  bool m_suppressOutput;
};

} //namespace mesh
} // namespace smtk

#endif // __smtk_mesh_DeleteMesh_h
