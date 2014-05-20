/*=========================================================================

Copyright (c) 1998-2003 Kitware Inc. 469 Clifton Corporate Parkway,
Clifton Park, NY, 12065, USA.

All rights reserved. No part of this software may be reproduced, distributed,
or modified, in any form or by any means, without permission in writing from
Kitware Inc.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================*/

#include "smtk/Qt/qtGroupView.h"

#include "smtk/Qt/qtUIManager.h"

#include "smtk/view/Base.h"
#include "smtk/view/Group.h"

#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSize>
#include <QTabWidget>
#include <QFile>
#include <QApplication>
#include <QVariant>
#include <QFrame>
#include <QLabel>
#include <QFont>

using namespace smtk::attribute;

//----------------------------------------------------------------------------
class qtGroupViewInternals
{
public:
  QList<smtk::attribute::qtBaseView*> ChildViews;
};

//----------------------------------------------------------------------------
qtGroupView::
qtGroupView(smtk::view::BasePtr dataObj, QWidget* p,
  qtUIManager* uiman) : qtBaseView(dataObj, p, uiman)
{
  this->Internals = new qtGroupViewInternals;
  this->createWidget( );
}

//----------------------------------------------------------------------------
qtGroupView::~qtGroupView()
{
  this->clearChildViews();
  delete this->Internals;
}
//----------------------------------------------------------------------------
void qtGroupView::createWidget( )
{
  if(!this->getObject())
    {
    return;
    }
  smtk::view::GroupPtr gview =
    smtk::dynamic_pointer_cast<smtk::view::Group>(this->getObject());
  if(!gview)
    {
    return;
    }

  this->clearChildViews();
  if(gview->style() == smtk::view::Group::TILED)
    {
    this->Widget = new QFrame(this->parentWidget());
    }
  else
    {
    QTabWidget *tab = new QTabWidget(this->parentWidget());
    tab->setUsesScrollButtons( true );
    this->Widget = tab;
    }

  //create the layout for the tabs area
  QVBoxLayout* layout = new QVBoxLayout(this->Widget);
  layout->setMargin(0);
  this->Widget->setLayout( layout );

  this->parentWidget()->layout()->addWidget(this->Widget);
}
//----------------------------------------------------------------------------
void qtGroupView::getChildView(
  smtk::view::Base::Type viewType, QList<qtBaseView*>& views)
{
  foreach(qtBaseView* childView, this->Internals->ChildViews)
    {
    if(childView->getObject()->type() == viewType)
      {
      views.append(childView);
      }
    else if(childView->getObject()->type() == smtk::view::Base::GROUP)
      {
      qobject_cast<qtGroupView*>(childView)->getChildView(
        viewType, views);
      }
    }
}
//----------------------------------------------------------------------------
qtBaseView* qtGroupView::getChildView(int pageIndex)
{
  if(pageIndex >= 0 && pageIndex < this->Internals->ChildViews.count())
    {
    return this->Internals->ChildViews.value(pageIndex);
    }
  return NULL;
}
//----------------------------------------------------------------------------
void qtGroupView::addChildView(qtBaseView* child)
{
  if(!this->Internals->ChildViews.contains(child))
    {
    this->Internals->ChildViews.append(child);
    smtk::view::GroupPtr gview =
      smtk::dynamic_pointer_cast<smtk::view::Group>(this->getObject());
    if(gview->style() == smtk::view::Group::TILED)
      {
      this->addTileEntry(child);
      }
    else
      {
      this->addTabEntry(child);
      }
    }
}
//----------------------------------------------------------------------------
QList<qtBaseView*>& qtGroupView::childViews() const
{
  return this->Internals->ChildViews;
}
//----------------------------------------------------------------------------
void qtGroupView::clearChildViews()
{
  foreach(qtBaseView* childView, this->Internals->ChildViews)
    {
    delete childView;
    }
  this->Internals->ChildViews.clear();
}

//----------------------------------------------------------------------------
void qtGroupView::updateUI()
{
  foreach(qtBaseView* childView, this->Internals->ChildViews)
    {
    childView->updateUI();
    }
}
//----------------------------------------------------------------------------
void qtGroupView::addTabEntry(qtBaseView* child)
{
  QTabWidget* tabWidget = static_cast<QTabWidget*>(this->Widget);
  if(!tabWidget || !child || !child->getObject())
    {
    return;
    }
  QWidget* tabPage = new QWidget();
  tabPage->setSizePolicy(QSizePolicy::Preferred,
    QSizePolicy::Expanding);

  QScrollArea* s = new QScrollArea(tabWidget);
  s->setWidgetResizable(true);
  s->setFrameShape(QFrame::NoFrame);
  QString secTitle = child->getObject()->title().c_str();
  QString name = "tab" + QString(secTitle);
  s->setObjectName(name);
  s->setWidget(tabPage);

  QVBoxLayout* vLayout = new QVBoxLayout(tabPage);
  vLayout->setMargin(0);
  vLayout->addWidget(child->widget());

  int index = tabWidget->addTab(s, secTitle);
  tabWidget->setTabToolTip( index, secTitle);

  //using the ui label name find if we have an icon resource
  QString resourceName = QApplication::applicationDirPath().append("/../Resources/Icons/");
  //QString resourceName = ":/SimBuilder/Icons/";
  resourceName.append(child->getObject()->iconName().c_str());
  resourceName.append(".png");

  // If user specified icons are not found, use default ones
  if ( !QFile::exists( resourceName ) )
    {
    resourceName = QString(":/SimBuilder/Icons/").
      append(secTitle).append(".png");
    }

  if ( QFile::exists( resourceName ) )
    {
    QPixmap image(resourceName);
    QIcon icon;
    QMatrix transform;
    if ( tabWidget->tabPosition() == QTabWidget::East )
      {
      transform.rotate(-90);
      }
    else if( tabWidget->tabPosition() == QTabWidget::West )
      {
      transform.rotate(90);
      }
    icon = image.transformed( transform );
    tabWidget->setTabIcon( index, icon );
    tabWidget->setTabText( index, "");
    }

  vLayout->setAlignment(Qt::AlignTop);
}

//----------------------------------------------------------------------------
void qtGroupView::addTileEntry(qtBaseView* child)
{
  QFrame* frame = static_cast<QFrame*>(this->Widget);
  if(!frame || !child || !child->getObject())
    {
    return;
    }
  QLabel* label = new QLabel(child->getObject()->title().c_str(),
    this->Widget);
  QFont titleFont;
  titleFont.setBold(true);
  titleFont.setItalic(true);
  label->setFont(titleFont);

  QLayout* vLayout = this->Widget->layout();
  vLayout->setMargin(0);
  vLayout->addWidget(label);
  vLayout->addWidget(child->widget());
  vLayout->setAlignment(Qt::AlignTop);
}