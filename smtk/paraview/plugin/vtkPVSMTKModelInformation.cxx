#include "vtkPVSMTKModelInformation.h"

#include "vtkTransform.h"
#include "vtkCellData.h"
#include "vtkClientServerStream.h"
#include "vtkDataObject.h"
#include "vtkDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkDoubleArray.h"
#include "vtkStringArray.h"
#include "vtkFieldData.h"
#include "vtkAlgorithmOutput.h"
#include "vtkAlgorithm.h"
#include "vtkCompositeDataIterator.h"

#include "vtkIntArray.h"

vtkStandardNewMacro(vtkPVSMTKModelInformation);

//----------------------------------------------------------------------------
vtkPVSMTKModelInformation::vtkPVSMTKModelInformation()
{
  this->Transform = vtkTransform::New();
  this->Translation[0] = this->Translation[1] = this->Translation[2] = 0;
  this->Orientation[0] = this->Orientation[1] = this->Orientation[2] = 0;
  this->Scale = 1;
  this->Color[0] = this->Color[1] = this->Color[2] = 1;
  this->NumberOfPoints = -1;
  this->ModelFaceInfoArray = NULL;
  this->SplitModelFaces = NULL;
  this->CellIdMapArray = NULL;
}

//----------------------------------------------------------------------------
vtkPVSMTKModelInformation::~vtkPVSMTKModelInformation()
{
  this->Transform->Delete();
  this->EnityIdsMap.clear();
}

//----------------------------------------------------------------------------
void vtkPVSMTKModelInformation::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  this->Transform->PrintSelf(os, indent);
  os << indent << "Translation: " << this->Translation[0] << ", " <<
    this->Translation[1] << ", " << this->Translation[2]  << endl;
  os << indent << "Orientation: " << this->Orientation[0] << ", " <<
    this->Orientation[1] << ", " << this->Orientation[2]  << endl;
  os << indent << "Scale: " << this->Scale << endl;
  os << indent << "Color: " << this->Color[0] << ", " <<
    this->Color[1] << ", " << this->Color[2]  << endl;
  os << indent << "NumberOfPoints: " << this->NumberOfPoints << endl;
}

//----------------------------------------------------------------------------
void vtkPVSMTKModelInformation::CopyFromObject(vtkObject* obj)
{
  this->EnityIdsMap.clear();
  vtkDataObject *dataObject = vtkDataObject::SafeDownCast( obj );

  // Handle the case where the a vtkAlgorithmOutput is passed instead of
  // the data object. vtkSMPart uses vtkAlgorithmOutput.
  if (!dataObject)
    {
    vtkAlgorithmOutput* algOutput = vtkAlgorithmOutput::SafeDownCast( obj );
    if (algOutput && algOutput->GetProducer())
      {
      dataObject = algOutput->GetProducer()->GetOutputDataObject(
        algOutput->GetIndex() );
      }
    vtkAlgorithm* alg = vtkAlgorithm::SafeDownCast( obj );
    if (alg)
      {
      dataObject = alg->GetOutputDataObject( 0 );
      }
    if (!dataObject)
      {
      vtkErrorMacro("Unable to get data object from object!");
      return;
      }
    }
/*
  // if composite, grab transform from 1st block
  vtkDiscreteModelWrapper* modelData = vtkDiscreteModelWrapper::SafeDownCast(dataObject);
  if (modelData)
    {
    vtkCompositeDataIterator* iter = modelData->NewIterator();
    iter->InitTraversal();
    dataObject = iter->GetCurrentDataObject();
    vtkDataSet* modelEntPoly=NULL;
    while (!iter->IsDoneWithTraversal())
      {
      modelEntPoly = vtkDataSet::SafeDownCast(iter->GetCurrentDataObject());
      if (modelEntPoly)
        {
        unsigned int curr_idx = iter->GetCurrentFlatIndex();
        vtkIdType entId;
        if (modelData->GetEntityIdByChildIndex(curr_idx-1, entId))
          {
          this->EnityIdsMap[curr_idx] = entId;
          }
        }
      iter->GoToNextItem();
      }
    iter->Delete();
    }

  this->SplitModelFaces = vtkIntArray::SafeDownCast(
    dataObject->GetFieldData()->GetArray(
    vtkMultiBlockWrapper::GetSplitFacesTagName()) );

  this->ModelFaceInfoArray = vtkIntArray::SafeDownCast(
    dataObject->GetFieldData()->GetArray(
      vtkMultiBlockWrapper::GetModelFaceDataName()) );
  this->CellIdMapArray =vtkIntArray::SafeDownCast(
    vtkDataSet::SafeDownCast(dataObject)->GetCellData()->GetArray(
      vtkMultiBlockWrapper::GetReverseClassificationTagName()));

*/

  if (vtkDataSet::SafeDownCast(dataObject))
    {
    this->NumberOfPoints = vtkDataSet::SafeDownCast(dataObject)->GetNumberOfPoints();
    this->NumberOfCells = vtkDataSet::SafeDownCast(dataObject)->GetNumberOfCells();
    }
  else
    {
    this->NumberOfPoints = -1;
    this->NumberOfCells = -1;
    }
}

//----------------------------------------------------------------------------
vtkIdType vtkPVSMTKModelInformation::GetModelEntityId(
  unsigned int flatidx)
{
  if(this->EnityIdsMap.find(flatidx) != this->EnityIdsMap.end())
    {
    return this->EnityIdsMap[flatidx];
    }
  return -1;
}

//----------------------------------------------------------------------------
int vtkPVSMTKModelInformation::GetModelFaceId()
{
  if(this->ModelFaceInfoArray &&
    this->ModelFaceInfoArray->GetNumberOfComponents()>0 &&
    this->ModelFaceInfoArray->GetNumberOfTuples()>2)
    {
    return this->ModelFaceInfoArray->GetValue(2);
    }
  else
    {
    return -1;
    }
}

//----------------------------------------------------------------------------
int vtkPVSMTKModelInformation::GetMaterialId()
{
  if(this->ModelFaceInfoArray &&
    this->ModelFaceInfoArray->GetNumberOfTuples()>1)
    {
    return this->ModelFaceInfoArray->GetValue(1);
    }
  else
    {
    return -1;
    }
}

//----------------------------------------------------------------------------
int vtkPVSMTKModelInformation::GetMasterCellId(int idx)
{
  if(this->CellIdMapArray &&
    this->CellIdMapArray->GetNumberOfTuples()>idx)
    {
    return this->CellIdMapArray->GetValue(idx);
    }
  else
    {
    return -1;
    }
}

//----------------------------------------------------------------------------
int vtkPVSMTKModelInformation::GetInfoArrayBCStartIndex()
{
  if(this->ModelFaceInfoArray &&
    this->ModelFaceInfoArray->GetNumberOfTuples()>3)
    {
    return 3;
    }
  else
    {
    return -1;
    }
}

//----------------------------------------------------------------------------
int vtkPVSMTKModelInformation::GetShellId()
{
  if(this->ModelFaceInfoArray &&
    this->ModelFaceInfoArray->GetNumberOfTuples()>0)
    {
    return this->ModelFaceInfoArray->GetValue(0);
    }
  else
    {
    return -1;
    }
}

//----------------------------------------------------------------------------
void vtkPVSMTKModelInformation::AddInformation(vtkPVInformation* info)
{
  vtkPVSMTKModelInformation *modelFaceInfo =
    vtkPVSMTKModelInformation::SafeDownCast(info);
  if (modelFaceInfo)
    {
    this->Transform->DeepCopy( modelFaceInfo->GetTransform() );
    modelFaceInfo->GetTranslation(this->Translation);
    modelFaceInfo->GetOrientation(this->Orientation);
    this->Scale = modelFaceInfo->GetScale();
    modelFaceInfo->GetColor(this->Color);
    this->NumberOfPoints = modelFaceInfo->GetNumberOfPoints();
    this->NumberOfCells = modelFaceInfo->GetNumberOfCells();

    this->ObjectType = modelFaceInfo->GetObjectType();
    }
}

//----------------------------------------------------------------------------
void
vtkPVSMTKModelInformation::CopyToStream(vtkClientServerStream* css)
{
  css->Reset();
  *css << vtkClientServerStream::Reply;
  vtkMatrix4x4 *matrix = this->Transform->GetMatrix();
  *css << vtkClientServerStream::InsertArray(matrix[0][0], 16) <<
    vtkClientServerStream::InsertArray(this->Translation, 3) <<
    vtkClientServerStream::InsertArray(this->Orientation, 3) <<
    this->Scale <<
    vtkClientServerStream::InsertArray(this->Color, 3) <<
    this->NumberOfPoints <<
    this->ObjectType.c_str() <<
    vtkClientServerStream::End;
}

//----------------------------------------------------------------------------
void
vtkPVSMTKModelInformation::CopyFromStream(const vtkClientServerStream* css)
{
  double elements[16];
  css->GetArgument(0, 0, elements, 16);
  this->Transform->SetMatrix( elements );
  css->GetArgument(0, 1, this->Translation, 3);
  css->GetArgument(0, 2, this->Orientation, 3);
  css->GetArgument(0, 3, &this->Scale);
  css->GetArgument(0, 4, this->Color, 3);
  css->GetArgument(0, 5, &this->NumberOfPoints);
  char buffer[64];
  css->GetArgument(0, 6, buffer);
  this->ObjectType = buffer;
}
