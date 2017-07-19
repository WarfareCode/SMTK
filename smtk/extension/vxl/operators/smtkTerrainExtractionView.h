//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
// .NAME smtkAssignColorsView - UI component for extract terrain out of point clouds
// .SECTION Description
// .SECTION See Also
// qtSection

#ifndef smtkTerrainExtractionView_h
#define smtkTerrainExtractionView_h

#include "Exports.h"
#include "smtk/extension/qt/qtBaseView.h"
#include <vtk_jsoncpp.h> // for Json::Value; must be in header due to VTK mangling

#include <QtCore/QObject>
#include <QtWidgets/QLayout>

class smtkTerrainExtractionViewInternals;

/// A view for exporting SMTK "packages" (SMTK files with data saved to the same directory).
class SMTKVXLOPERATORVIEWSEXT_EXPORT smtkTerrainExtractionView : public smtk::extension::qtBaseView
{
  Q_OBJECT

public:
  static smtk::extension::qtBaseView* createViewWidget(const smtk::extension::ViewInfo& info);

  smtkTerrainExtractionView(const smtk::extension::ViewInfo& info);
  virtual ~smtkTerrainExtractionView();

  // virtual bool displayItem(smtk::attribute::ItemPtr);

public slots:
  virtual void updateUI() {} // NB: Subclass implementation causes crashes.
  virtual void requestModelEntityAssociation();
  virtual void onShowCategory() { this->updateAttributeData(); }
  // This will be triggered by selecting different type
  // of construction method in create-edge op.
  virtual void valueChanged(smtk::attribute::ItemPtr optype);

protected slots:
  virtual void requestOperation(const smtk::model::OperatorPtr& op);
  virtual void cancelOperation(const smtk::model::OperatorPtr&);

  // This slot is used to indicate that the underlying attribute
  // for the operation should be checked for validity
  virtual void attributeModified();

  virtual void updateAttributeData();

protected:
  virtual void createWidget();
  virtual void setInfoToBeDisplayed() override;

private:
  smtkTerrainExtractionViewInternals* Internals;
};

#endif // smtkTerrainExtractionView_h
