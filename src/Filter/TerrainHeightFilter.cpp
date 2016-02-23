#include <TerrainHeightFilter.h>
#include <TerrainHeightTransform.hpp>
#include <vtkDataObject.h>
#include <vtkIndent.h>
#include <vtkInformation.h>
#include <vtkObjectFactory.h>

vtkStandardNewMacro(TerrainHeightFilter)

void TerrainHeightFilter::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	os << indent << "Terrain height adjustment, Kronos Project" << endl;
}

int TerrainHeightFilter::FillOutputPortInformation(int, vtkInformation* info)
{
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPointSet");
	return 1;
}

int TerrainHeightFilter::FillInputPortInformation(int, vtkInformation* info)
{
	//make sure that we only accept vtkPointSet, vtkImageData and vtkRectilinearGrid as input
	info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
	info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
	info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
	info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkRectilinearGrid");
	return 1;
}

void TerrainHeightFilter::setClampingEnabled(bool clamp)
{
	TerrainHeightTransform * trans = getTransform();
	if (trans)
	{
		trans->setClampingEnabled(clamp);
	}
}

bool TerrainHeightFilter::isClampingEnabled() const
{
	TerrainHeightTransform * trans = getTransform();
	if (trans)
	{
		return trans->isClampingEnabled();
	}
	return false;
}

TerrainHeightFilter::TerrainHeightFilter()
{
	this->Transform = TerrainHeightTransform::New(true);
}

TerrainHeightFilter::~TerrainHeightFilter()
{
}

TerrainHeightTransform* TerrainHeightFilter::getTransform() const
{
	return dynamic_cast<TerrainHeightTransform*>(this->Transform);
}

TerrainHeightFilter::TerrainHeightFilter(const TerrainHeightFilter&)
{
}

void TerrainHeightFilter::operator =(const TerrainHeightFilter&)
{
}
