//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#ifndef smtk_extension_paraview_server_vtkSMTKModelImporter_h
#define smtk_extension_paraview_server_vtkSMTKModelImporter_h

#include "smtk/extension/paraview/server/vtkSMTKResourceReader.h"

#include "smtk/PublicPointerDefs.h"

#include "smtk/resource/Resource.h"

#include "vtkMultiBlockDataSetAlgorithm.h"
#include "vtkNew.h"
#include "vtkSmartPointer.h"

class vtkModelMultiBlockSource;
class vtkSMTKWrapper;

/**\brief Use SMTK to import a model as a ParaView-friendly model source.
  *
  * If the SMTK wrapper object is set, then the wrapper's resource and operation
  * manager are used to import the file (or perhaps in the future to create a
  * new resource). Otherwise SMTK's default environment is used.
  */
class SMTKPVSERVEREXT_EXPORT vtkSMTKModelImporter : public vtkSMTKResourceReader
{
public:
  vtkTypeMacro(vtkSMTKModelImporter, vtkSMTKResourceReader);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  static vtkSMTKModelImporter* New();

  /// Set the import mode to only import resources of a given type. The resource
  /// is identified by its class type.
  template <typename ResourceType>
  bool SetImporterResourceScope();

  /// Set the import mode to only import resources of a given type. The resource
  /// is identified by its unique name.
  bool SetImporterResourceScope(const std::string&);

  /// Set the import mode to only import resources of a given type. THe resource
  /// is identified by its type index.
  bool SetImporterResourceScope(const smtk::resource::Resource::Index&);

  /// Return the name of the resource type to which this importer is scoped. If
  /// the resource type has not been set, we accept all resource types. If the
  /// resource type was selected and this type was subsequently removed from the
  /// list of supported resource types, we accept no resource types.
  const std::string& GetImporterResourceScope() const;

  /// Return a string describing supported file filters in the Qt format. For
  /// example: "Ext1 (*.ex1);;Ext2 or 3 (*.ex2 *.ex3)"
  std::string SupportedExtensions() const;

  /// Return the VTK algorithm used to import the SMTK file.
  vtkModelMultiBlockSource* GetModelSource() { return this->ModelSource.GetPointer(); }

  /// Return the SMTK resource that holds data imported from \a FileName.
  smtk::resource::ResourcePtr GetResource() const override;

  /// Return the SMTK model resource that holds data imported from \a FileName.
  smtk::model::ManagerPtr GetSMTKResource() const;

protected:
  vtkSMTKModelImporter();
  ~vtkSMTKModelImporter() override;

  int RequestData(
    vtkInformation* request, vtkInformationVector** inInfo, vtkInformationVector* outInfo) override;

  bool LoadFile();

  vtkNew<vtkModelMultiBlockSource> ModelSource;

  std::string ResourceName;

private:
  vtkSMTKModelImporter(const vtkSMTKModelImporter&) = delete;
  void operator=(const vtkSMTKModelImporter&) = delete;
};

template <typename ResourceType>
bool vtkSMTKModelImporter::SetImporterResourceScope()
{
  return this->SetImporterResourceScope(std::type_index(typeid(ResourceType)).hash_code());
}

#endif