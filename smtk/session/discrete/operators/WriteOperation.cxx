//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

#include "WriteOperation.h"

#include "smtk/session/discrete/Resource.h"
#include "smtk/session/discrete/Session.h"

#include "smtk/attribute/Attribute.h"
#include "smtk/attribute/ComponentItem.h"
#include "smtk/attribute/FileItem.h"
#include "smtk/attribute/ResourceItem.h"
#include "smtk/attribute/StringItem.h"

#include "smtk/model/Model.h"

#include "smtk/session/discrete/kernel/Model/vtkModel.h"
#include "smtk/session/discrete/kernel/Model/vtkModelItem.h"
#include "vtkDiscreteModelWrapper.h"
#include <vtksys/SystemTools.hxx>

#include "WriteOperation_xml.h"

// #define SMTK_DISCRETE_SESSION_DEBUG

#if defined(SMTK_DISCRETE_SESSION_DEBUG)
#include "smtk/io/SaveJSON.h"
#endif

using namespace smtk::model;

namespace smtk
{
namespace session
{

namespace discrete
{

WriteOperation::WriteOperation()
{
  m_currentversion = 5;
}

bool WriteOperation::ableToOperate()
{
  if (!this->Superclass::ableToOperate())
  {
    return false;
  }

  smtk::model::Models models = this->parameters()->associatedModelEntities<smtk::model::Models>();

  // The SMTK model must be valid
  if (models.empty() || !models[0].isValid())
  {
    return false;
  }

  smtk::session::discrete::Resource::Ptr resource =
    std::static_pointer_cast<smtk::session::discrete::Resource>(models[0].component()->resource());

  // The CMB model must exist:
  if (!resource->discreteSession()->findModelEntity(models[0].entity()))
  {
    return false;
  }

  std::string filename = this->parameters()->findFile("filename")->value();
  return !filename.empty();
}

WriteOperation::Result WriteOperation::operateInternal()
{
  std::string fname = this->parameters()->findFile("filename")->value();
  if (fname.empty())
    return this->createResult(smtk::operation::Operation::Outcome::FAILED);

  // make sure the filename has .cmb extension
  std::string ext = vtksys::SystemTools::GetFilenameLastExtension(fname);
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
  if (ext != ".cmb")
  {
    std::string tmpname = vtksys::SystemTools::GetFilenameWithoutLastExtension(fname);
    fname = vtksys::SystemTools::GetFilenamePath(fname);
    fname.append("/").append(tmpname).append(".cmb");
  }

  m_op->SetFileName(fname.c_str());

  // ableToOperate should have verified that the model exists
  smtk::model::Models models = this->parameters()->associatedModelEntities<smtk::model::Models>();
  if (models.empty())
    return this->createResult(smtk::operation::Operation::Outcome::FAILED);

  smtk::model::Model model = models[0];

  smtk::session::discrete::Resource::Ptr resource =
    std::static_pointer_cast<smtk::session::discrete::Resource>(model.component()->resource());

  SessionPtr opsession = resource->discreteSession();

  vtkDiscreteModelWrapper* modelWrapper = opsession->findModelEntity(model.entity());

  // write the file out.
  m_op->SetVersion(m_currentversion);
  m_op->Operate(modelWrapper, opsession.get());

  if (!m_op->GetOperateSucceeded())
  {
    std::cerr << "Could not write file \"" << fname << "\".\n";
    return this->createResult(smtk::operation::Operation::Outcome::FAILED);
  }

  Result result = this->createResult(smtk::operation::Operation::Outcome::SUCCEEDED);
  this->parameters()->findFile("filename")->setValue(fname);
  // The model was not modified while writing cmb file.
  // this->addEntityToResult(result, model, MODIFIED);

  return result;
}

const char* WriteOperation::xmlDescription() const
{
  return WriteOperation_xml;
}

} // namespace discrete
} // namespace session
} // namespace smtk
