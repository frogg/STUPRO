#include <Filter/TwitterFilter.h>

#include <Reader/DataReader/Data.hpp>

#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkIntArray.h>
#include <vtkCompositeDataSet.h>
#include <vtkCompositeDataIterator.h>
#include <vtkHierarchicalBoxDataIterator.h>
#include <vtkTable.h>
#include <vtkGraph.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPointSet.h>


TwitterFilter::TwitterFilter() {
    std::cout << "mama" << this->visibleAuthorName.count();
}

TwitterFilter::~TwitterFilter() { }



vtkStandardNewMacro(TwitterFilter);

QList<Data::Type> TwitterFilter::getCompatibleDataTypes() {
    return (QList<Data::Type>() << Data::TWEETS);
}

bool TwitterFilter::evaluatePoint(int pointIndex, Coordinate coordinate,
                                            vtkPointData* pointData) {
   /* vtkSmartPointer<vtkIntArray> precipitationTypeArray = vtkIntArray::SafeDownCast(
                                                                                    pointData->GetArray("precipitationTypes"));
    
    // For the point that should be checked, get its data type from the VTK data array and look up
    // in the QMap `precipitationTypeVisibilities` whether points with this data type should be
    // visible or not.
    return this->precipitationTypeVisibilities[static_cast<PrecipitationDataPoint::PrecipitationType>
                                               (precipitationTypeArray->GetTuple1(pointIndex))];
    */
    
    if(this->visibleAuthorName.count() == 0){
        //vielleicht auch count == 1, weil leerer string auch reingespeichert wird
        return true;
    }else{
        vtkSmartPointer<vtkIntArray> twitterArray = vtkStringArray::SafeDownCast(pointData->GetArray("authors"));
        std::cout << twitterArray->GetTuple1(pointIndex);
        //adapt it
        return true;
    }
}

void TwitterFilter::setAuthorName(const char* authorName){
    std::cout << "nana" << authorName;
    QString str = QString::fromStdString(authorName);
    str.remove(' ');
    
    visibleAuthorName = str.split( ";" );
    std::cout << "lala" << str.toLatin1().data() << ": " << visibleAuthorName.count();
//    std::cout << "hi" << QString::compare(str, "", Qt::CaseInsensitive) << "bla" << QString::compare(str, " ",Qt::CaseInsensitive);
    this->Modified();
}


void TwitterFilter::setMatchingMode(int mode){
    std::cout << "test12345566" << mode;
    this->Modified();
}

void TwitterFilter::SetInputConnection(vtkAlgorithmOutput* input) {
    this->Superclass::SetInputConnection(input);
}

