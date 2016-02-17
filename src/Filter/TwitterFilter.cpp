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
#include <vtkStringArray.h>



TwitterFilter::TwitterFilter() {
    //std::cout << "contains 0 elements:" << this->visibleAuthorName.count();
    //default is containingMode
    this->matchingMode = 0;
}

TwitterFilter::~TwitterFilter() { }



vtkStandardNewMacro(TwitterFilter);

QList<Data::Type> TwitterFilter::getCompatibleDataTypes() {
    return (QList<Data::Type>() << Data::TWEETS);
}

bool TwitterFilter::evaluatePoint(int pointIndex, Coordinate coordinate,
                                            vtkPointData* pointData) {

    //if no author is in visibleAuthorName, return this point to be visible
    if(this->visibleAuthorName.count() == 0){
        return true;
    }else{
        vtkSmartPointer<vtkStringArray> twitterArray = vtkStringArray::SafeDownCast(pointData->GetAbstractArray("authors"));
            QString author = QString::fromStdString(twitterArray->GetValue(pointIndex));
        author.remove(' ');
        for(int i=0; i<visibleAuthorName.length();i++){
            QString temp = visibleAuthorName.at(i);
            //check author contains substring temp
            
            if(matchingMode == 0){
                //containing Mode
                if(author.contains(temp,Qt::CaseInsensitive)){
                    return true;
                }
            }else if(matchingMode == 1){
                //exact match
                if(QString::compare(author, "", Qt::CaseInsensitive) == 0){
                    return true;
                }
            }
            }
        return false;
    }
}

void TwitterFilter::setAuthorName(const char* authorNames){

    QString authors = QString::fromStdString(authorNames);
    authors.remove(' ');
    //nothing is entered or just a whitespace -> empty QList
    if(QString::compare(authors, "", Qt::CaseInsensitive) == 0){
        for(int i=0; i<visibleAuthorName.length();i++){
            visibleAuthorName.removeFirst();
        }
    }else{
        visibleAuthorName = authors.split( ";" );
    }
    
    //std::cout << "Content" << authors.toLatin1().data() << " ,number of elements: " << visibleAuthorName.count() << std::endl;
    this->Modified();
}


void TwitterFilter::setMatchingMode(int mode){
    matchingMode = mode;
    this->Modified();
}

void TwitterFilter::SetInputConnection(vtkAlgorithmOutput* input) {
    this->Superclass::SetInputConnection(input);
}

