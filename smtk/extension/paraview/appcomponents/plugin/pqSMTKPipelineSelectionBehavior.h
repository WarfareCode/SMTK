//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#ifndef smtk_extension_paraview_appcomponents_pqSMTKPipelineSelectionBehavior_h
#define smtk_extension_paraview_appcomponents_pqSMTKPipelineSelectionBehavior_h

#include "smtk/PublicPointerDefs.h"
#include "smtk/model/EntityTypeBits.h"

#include "smtk/view/SelectionObserver.h"

#include <QObject>

#include <string>

class vtkSMSMTKWrapperProxy;
class pqPipelineSource;
class pqServer;

/**\brief Keep the ParaView pipeline browser and SMTK selections in sync.
  *
  * When the SMTK selection is updated and contains resources (as opposed
  * to components), select those resources in the pipeline browser.
  * Similarly, when pipeline sources in ParaView are selected, replace
  * the SMTK selection with the related resources.
  */
class pqSMTKPipelineSelectionBehavior : public QObject
{
  Q_OBJECT
  using Superclass = QObject;

public:
  pqSMTKPipelineSelectionBehavior(QObject* parent = nullptr);
  ~pqSMTKPipelineSelectionBehavior() override;

  /// This behavior is a singleton.
  static pqSMTKPipelineSelectionBehavior* instance(QObject* parent = nullptr);

  /// Set which integer bit(s) to modify in the SMTK selection when a pipeline source is selected.
  void setSelectionValue(const std::string& selectionValue);
  const std::string& selectionValue() const { return m_selectionValue; }

protected slots:
  virtual void onActiveSourceChanged(pqPipelineSource* source);
  virtual void observeSelectionOnServer(vtkSMSMTKWrapperProxy* mgr, pqServer* server);
  virtual void unobserveSelectionOnServer(vtkSMSMTKWrapperProxy* mgr, pqServer* server);

protected:
  bool m_changingSource;
  std::string m_selectionValue;
  std::map<smtk::view::SelectionPtr, smtk::view::SelectionObservers::Key> m_selectionObservers;

private:
  Q_DISABLE_COPY(pqSMTKPipelineSelectionBehavior);
};

#endif
