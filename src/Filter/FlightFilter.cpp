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
    this->modeAirline = CONTAINING;
}

FlightFilter::~FlightFilter() { }



vtkStandardNewMacro(FlightFilter);

QList<Data::Type> FlightFilter::getCompatibleDataTypes() {
    return (QList<Data::Type>() << Data::FLIGHTS);
}


bool FlightFilter::evaluatePoint(int pointIndex, Coordinate coordinate,
                                            vtkPointData* pointData) {
    if(this->visibleAirlines.count() == 0){
      return true;
    }else{
        vtkSmartPointer<vtkStringArray> airlines = vtkStringArray::SafeDownCast(pointData->GetAbstractArray("airlines"));
        QString airline = QString::fromStdString(airlines->GetValue(pointIndex));
        airline.remove(' ');
        
        for(int i=0; i<visibleAirlines.count();i++){
            QString temp = visibleAirlines.at(i);
            //check airline contains substring temp
            if(this->modeAirline == CONTAINING){
                //containing Mode
                if(airline.contains(temp,Qt::CaseInsensitive)){
                    return true;
                }
            }else if(this->modeAirline == MATCHING){
                //exact match
                if(QString::compare(airline, temp, Qt::CaseInsensitive) == 0){
                    return true;
                }
            }
        }
        return false;
    }
    
}

void setAirportCodeOrigin(const char* airportCodeOrigin){
    this->Modified();
}


void setAirportCodeDestination(const char* airportCodeDestinatioin){
    this->Modified();
}




void FlightFilter::setAirlineMatchingMode(int matchingMode){
    this->modeAirline = static_cast<Mode>(matchingMode);
    this->Modified();
}

void FlightFilter::setAirline(const char* airline){
    QString airlines = QString::fromStdString(airline);
    airlines.remove(' ');
    
    if(QString::compare(airlines, "", Qt::CaseInsensitive) == 0){
        //might be improved later
        this->visibleAirlines.clear();
    }else{
        this->visibleAirlines = airlines.split( "," );
    }
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

