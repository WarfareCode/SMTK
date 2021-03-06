//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#ifndef __smtk_session_discrete_Resource_h
#define __smtk_session_discrete_Resource_h

#include "smtk/session/discrete/Exports.h"
#include "smtk/session/discrete/Session.h"

#include "smtk/resource/DerivedFrom.h"

#include "smtk/model/Resource.h"

#include "smtk/resource/Manager.h"

class vtkModelItem;

namespace smtk
{
namespace session
{
namespace discrete
{

class Session;
typedef smtk::shared_ptr<Session> SessionPtr;
typedef smtk::shared_ptr<const Session> ConstSessionPtr;

class SMTKDISCRETESESSION_EXPORT Resource
  : public smtk::resource::DerivedFrom<Resource, smtk::model::Resource>
{
public:
  smtkTypeMacro(smtk::session::discrete::Resource);
  smtkSharedPtrCreateMacro(smtk::resource::PersistentObject);

  virtual ~Resource() {}

  void setSession(const Session::Ptr&);

  SessionPtr discreteSession();
  ConstSessionPtr discreteSession() const;

  vtkModelItem* discreteEntity(const smtk::model::EntityRef& smtkEntity);
  template <typename T>
  T discreteEntityAs(const smtk::model::EntityRef& smtkEntity);

protected:
  Resource(const smtk::common::UUID&, smtk::resource::Manager::Ptr manager = nullptr);
  Resource(smtk::resource::Manager::Ptr manager = nullptr);

  Session::Ptr m_session;
};

/// A convenience method for returning the discrete counterpart of an SMTK entity already cast to a subtype.
template <typename T>
T Resource::discreteEntityAs(const smtk::model::EntityRef& smtkEntity)
{
  return dynamic_cast<T>(this->discreteEntity(smtkEntity));
}

} // namespace discrete
} // namespace session
} // namespace smtk

#endif // __smtk_session_discrete_Resource_h
