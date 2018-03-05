//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

#include "smtk/extension/vtk/operators/vtkSMTKOperation.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkSMTKOperation);

vtkSMTKOperation::vtkSMTKOperation()
{
}

vtkSMTKOperation::~vtkSMTKOperation()
{
}

void vtkSMTKOperation::SetSMTKOperation(smtk::operation::Operation::Ptr op)
{
  if (this->m_smtkOp.lock() != op)
  {
    this->m_smtkOp = op;
    this->Modified();
  }
}

smtk::operation::Operation::Ptr vtkSMTKOperation::GetSMTKOperation()
{
  return this->m_smtkOp.lock();
}

bool vtkSMTKOperation::AbleToOperate()
{
  return this->m_smtkOp.lock() ? this->m_smtkOp.lock()->ableToOperate() : false;
}

smtk::operation::Operation::Result vtkSMTKOperation::Operate()
{
  return this->m_smtkOp.lock() ? this->m_smtkOp.lock()->operate()
                               : smtk::operation::Operation::Result();
}

void vtkSMTKOperation::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent
     << "smtk op: " << (this->m_smtkOp.lock() ? this->m_smtkOp.lock()->uniqueName() : "(none)")
     << endl;

  this->Superclass::PrintSelf(os, indent);
}