//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

#ifndef smtk_project_Manager_h
#define smtk_project_Manager_h

#include "smtk/CoreExports.h"
#include "smtk/PublicPointerDefs.h"
#include "smtk/SharedFromThis.h"
#include "smtk/SystemConfig.h"

#include "smtk/io/Logger.h"

#include <string>
#include <vector>

namespace smtk
{
namespace project
{
class Project;

/// A project Manager is responsible for creating and managing smtk projects.
/// The current implementation only supports one project being "open" at a time.
class SMTKCORE_EXPORT Manager : smtkEnableSharedPtr(Manager)
{
public:
  smtkTypedefs(smtk::project::Manager);
  static smtk::project::ManagerPtr create(
    smtk::resource::ManagerPtr&, smtk::operation::ManagerPtr&);

  virtual ~Manager();

  /// Create project with 2 methods: (i) get spec, then (ii) use spec to create
  smtk::attribute::AttributePtr getProjectSpecification();

  /// Create project for given specification, which includes filesystem path to store persistent data.
  /// Will not write to existing directory unless replaceExistingDirectory flag is set.
  /// Return value (pointer) is empty on error.
  ProjectPtr createProject(smtk::attribute::AttributePtr specification,
    bool replaceExistingDirectory = false, smtk::io::Logger& logger = smtk::io::Logger::instance());

  /// Open a new project from the filesystem. Returns outcome and project pointer
  /// The path argument can be set to either the project directory or the .cmbproject
  /// contained in the project directory.
  ProjectPtr openProject(
    const std::string& path, smtk::io::Logger& logger = smtk::io::Logger::instance());

  /// Return project instance
  ProjectPtr getCurrentProject() const { return m_project; }

  /// Write project resources & metadata to the filesystem. Returns true on success
  bool saveProject(smtk::io::Logger& logger = smtk::io::Logger::instance());

  /// Copy the current project's contents and make the new project current.
  /// Returns nullptr in case of error.
  ProjectPtr saveAsProject(
    const std::string& directory, smtk::io::Logger& logger = smtk::io::Logger::instance());

  /// Close the project resources. Returns true on success
  bool closeProject(smtk::io::Logger& logger = smtk::io::Logger::instance());

  /// Returns export operator from current project.
  /// If reset flag is true, will create new operator in order to
  /// reset contents to their default values.
  smtk::operation::OperationPtr getExportOperator(
    smtk::io::Logger& logger = smtk::io::Logger::instance(), bool reset = false) const;

  // Future:
  // * introduce "analysis" class to extend project structure
  // * support multiple projects (or multiple project managers?)
  // * support shared resources among projects?
protected:
  // Load and return NewProject template
  smtk::attribute::ResourcePtr getProjectTemplate();

  // Create Project instance
  ProjectPtr initProject(smtk::io::Logger& logger = smtk::io::Logger::instance());

private:
  Manager(smtk::resource::ManagerPtr&, smtk::operation::ManagerPtr&);

  /// Resource manager for the project resources.
  smtk::resource::WeakManagerPtr m_resourceManager;

  /// Operation manager for the project operations.
  smtk::operation::WeakManagerPtr m_operationManager;

  /// Current project
  smtk::project::ProjectPtr m_project;

  /// New project template
  smtk::attribute::ResourcePtr m_template;
}; // class smtk::project::Manager

} // namespace project
} // namespace smtk

#endif // smtk_project_Manager_h
