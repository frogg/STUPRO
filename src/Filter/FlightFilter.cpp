#include <Filter/FlightFilter.h>

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
#include <vtkStringArray.h>



FlightFilter::FlightFilter() {
    //std::cout << "contains 0 elements:" << this->visibleAuthorName.count();
    this->mode = CONTAINING;
}

FlightFilter::~FlightFilter() { }



vtkStandardNewMacro(FlightFilter);

QList<Data::Type> FlightFilter::getCompatibleDataTypes() {
    return (QList<Data::Type>() << Data::FLIGHTS);
}


bool FlightFilter::evaluatePoint(int pointIndex, Coordinate coordinate,
                                            vtkPointData* pointData) {
    return true;
    /*
    //if no author is in visibleAuthorName and no content is in visibleContent (everything should be visible by default), return this point to be visible
    if(this->visibleAuthorName.count() == 0 && this->visibleContent.count() == 0){
        return true;
    }else{
        vtkSmartPointer<vtkStringArray> twitterArray = vtkStringArray::SafeDownCast(pointData->GetAbstractArray("authors"));
            QString author = QString::fromStdString(twitterArray->GetValue(pointIndex));
        author.remove(' ');
        
        vtkSmartPointer<vtkStringArray> contents = vtkStringArray::SafeDownCast(pointData->GetAbstractArray("contents"));
        QString content = QString::fromStdString(contents->GetValue(pointIndex));
        
        
        for(int i=0; i<visibleAuthorName.count();i++){
            QString temp = visibleAuthorName.at(i);
            //check author contains substring temp
            
            if(this->mode == CONTAINING){
                //containing Mode
                if(author.contains(temp,Qt::CaseInsensitive) && this->shouldDisplayTweetContent(content)){
                    return true;
                }
            }else if(this->mode == MATCHING){
                //exact match
                if(QString::compare(author, "", Qt::CaseInsensitive) == 0 && shouldDisplayTweetContent(content)){
                    return true;
                }
            }
            }
        return false;
    }
 */
}

void FlightFilter::setMatchingMode(int matchingMode){
    this->mode = static_cast<Mode>(matchingMode);
    this->Modified();
}

void FlightFilter::setAirline(const char* airline){
    this->Modified();
}

void FlightFilter::SetInputConnection(vtkAlgorithmOutput* input) {
    this->Superclass::SetInputConnection(input);
}

/*
void TwitterFilter::setContent(const char* content){
    QString contentKeyWords = QString::fromStdString(content);
    contentKeyWords.remove(' ');

    if(QString::compare(contentKeyWords, "", Qt::CaseInsensitive) == 0){
        //might be improved later
        this->visibleContent = QStringList();
    }else{
        this->visibleContent = contentKeyWords.split( ";" );
    }
    this->Modified();
}
 */

