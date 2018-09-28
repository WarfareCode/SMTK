//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#ifndef __smtk_session_cgm_CreateBrick_h
#define __smtk_session_cgm_CreateBrick_h

#include "smtk/session/cgm/Operation.h"

namespace smtk
{
namespace session
{
namespace cgm
{

/**\brief Create a brick given width, height, depth **or**
  *       center, axes, and extension.
  *
  * If given a width, height, and depth, the cuboid
  * will have one corner on the origin and reside in
  * the positive octant.
  *
  * If given a center, unit-length \a axes, and
  * positive \a extensions along each axis then
  * the cuboid is constructed as a sheet body,
  * which may be flat (when one entry of the \a extension
  * vector is 0).
  * At most, a single extry of \a extension may be
  * zero. All other entries must be positive and above
  * the modeling kernel's "RESABS" parameter.
  * Note that \a extension values are half-widths along
  * their corresponding axis (i.e., how far the cuboid
  * extends along the axis from the center point).
  */
class SMTKCGMSESSION_EXPORT CreateBrick : public Operation
{
public:
  smtkTypeMacro(CreateBrick);
  smtkCreateMacro(CreateBrick);
  smtkSharedFromThisMacro(Operation);
  smtkDeclareModelOperation();

protected:
  Result operateInternal() override;
};

} // namespace cgm
} // namespace session
} // namespace smtk

#endif // __smtk_session_cgm_CreateBrick_h