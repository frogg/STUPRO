#include <Reader/DataReader/PolyDataSetHelper.hpp>

#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkDataArray.h>
#include <vtkStringArray.h>
#include <vtkTypeInt32Array.h>
#include <vtkTypeFloat32Array.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>

#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/CityDataPoint.hpp>
#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/FlightDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/TemperatureDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/TweetDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/WindDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/CloudCoverageDataPoint.hpp>

#include <math.h>
#include <iostream>

#include <Utils/Config/Configuration.hpp>

vtkSmartPointer<vtkPolyData> PolyDataSetHelper::getPolyDataFromDataPoints(
    PointDataSet dataPoints,
    int zoomLevel,
    Data::Type dataType
) {
	return PolyDataSetHelper::getPolyDataFromDataPoints(
	           dataPoints,
	           zoomLevel,
	           dataType,
	           0,
	           0,
	           0,
	           false
	       );
}

vtkSmartPointer<vtkPolyData> PolyDataSetHelper::getPolyDataFromDataPoints(
    PointDataSet dataPoints,
    int zoomLevel,
    Data::Type dataType,
    int timeResolution,
    int timeStep,
    int startTime
) {
	return PolyDataSetHelper::getPolyDataFromDataPoints(
	           dataPoints,
	           zoomLevel,
	           dataType,
	           timeResolution,
	           timeStep,
	           startTime,
	           true
	       );
}

vtkSmartPointer<vtkPolyData> PolyDataSetHelper::getPolyDataFromDataPoints(
    PointDataSet dataPoints,
    int zoomLevel,
    Data::Type dataType,
    int timeResolution,
    int timeStep,
    int startTime,
    bool temporal
) {
	QList<DataPoint*> relevantDataPoints;

	if (temporal) {
		relevantDataPoints = PolyDataSetHelper::extractRelevantDataPoints(
		                         dataPoints,
		                         zoomLevel,
		                         timeResolution,
		                         timeStep,
		                         startTime
		                     );
	} else {
		relevantDataPoints = PolyDataSetHelper::extractRelevantDataPoints(
		                         dataPoints,
		                         zoomLevel
		                     );
	}

	return PolyDataSetHelper::createPolyDataSet(
	           relevantDataPoints, dataType
	       );
}

QList<DataPoint*> PolyDataSetHelper::extractRelevantDataPoints(
    PointDataSet points,
    int zoomLevel
) {
	QList<DataPoint*> relevantDataPoints = QList<DataPoint*>();

	for (int i = 0; i < points.getDataPoints().size(); i++) {
		DataPoint* point = points.getDataPoints().at(i);
		if (point->getPriority() <= zoomLevel) {
			relevantDataPoints.append(point);
		}
	}

	return relevantDataPoints;
}

QList<DataPoint*> PolyDataSetHelper::extractRelevantDataPoints(
    PointDataSet points,
    int zoomLevel,
    int timeResolution,
    int timeStep,
    int startTime
) {
	QList<DataPoint*> relevantDataPoints = QList<DataPoint*>();

	int timeSpanStart = startTime + (timeStep * timeResolution);
	int timeSpanEnd = timeSpanStart + timeResolution;

	for (int i = 0; i < points.getDataPoints().size(); i++) {
		DataPoint* point = points.getDataPoints().at(i);

		if (point->getPriority() <= zoomLevel) {
			const TemporalDataPoint* dataPoint
			    = dynamic_cast<const TemporalDataPoint*>(point);

			if (
			    dataPoint->getTimestamp() >= timeSpanStart &&
			    dataPoint->getTimestamp() <= timeSpanEnd
			) {
				relevantDataPoints.append(point);
			}
		}
	}

	return relevantDataPoints;
}

vtkSmartPointer<vtkPolyData> PolyDataSetHelper::createPolyDataSet(
    QList<DataPoint*> relevantDataPoints,
    Data::Type dataType
) {
	// Create an empty data set and a point array the data set will contain
	vtkSmartPointer<vtkPolyData> dataSet = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

	// Check whether the data type is time-sensitive
	bool temporal = Data::isTemporal(dataType);

	// Create an empty vert data set
	vtkCellArray* verts = vtkCellArray::New();

	// Add a cell containing all points
	verts->Allocate(verts->EstimateSize(1, relevantDataPoints.size()));
	verts->InsertNextCell(relevantDataPoints.size());

	// Create some data arrays all temporal data sets have in common. The timestamp array will not
	// be used if the data set is non-temporal.
	vtkIdType numberOfTuples[1];
	numberOfTuples[0] = relevantDataPoints.size();

	// An integer array containing the priority of each data point. This is added as a convenience
	// measure for potentially using it later on with `vtkPointSetToLabelHierarchy`.
	vtkSmartPointer<vtkTypeInt32Array> priorities = vtkSmartPointer<vtkTypeInt32Array>::New();
	priorities->SetName("priorities");
	priorities->SetNumberOfComponents(1);
	priorities->SetNumberOfTuples(*numberOfTuples);

	// An integer array containing the timestamp of each data point. 32-bit integers are sufficient
	// since UNIX timestamps are being used.
	vtkSmartPointer<vtkTypeInt32Array> timestamps = vtkSmartPointer<vtkTypeInt32Array>::New();
	timestamps->SetName("timestamps");
	timestamps->SetNumberOfComponents(1);
	timestamps->SetNumberOfTuples(*numberOfTuples);

	// The current tuple index in the iteration
	int tupleNumber = 0;

	// Iterate through all relevant data points, add their coordinates as new points and fill the
	// array of priority values.
	for (QList<DataPoint*>::iterator iterator = relevantDataPoints.begin();
	        iterator != relevantDataPoints.end(); ++iterator) {
		// Insert the new point with its longitude, latitude and a height of zero
		// and add point to verts
		verts->InsertCellPoint(points->InsertNextPoint(
		                           (*iterator)->getCoordinate().lon(),
		                           (*iterator)->getCoordinate().lat(),
		                           0
		                       ));

		// Invert the priority before adding it since with `vtkPointSetToLabelHierarchy`,
		// higher priority values are more visible, which is the other way around than
		// the definition in these `DataReader` classes.
		double priority[1] = {
			(double) Configuration::getInstance().getInteger("dataReader.maximumPriority")
			- (*iterator)->getPriority()
		};
		priorities->SetTuple(tupleNumber, priority);

		// Add timestamps if they exist
		if (temporal) {
			const TemporalDataPoint* temporalDataPoint = dynamic_cast<const TemporalDataPoint*>(
			            (*iterator)
			        );

			double timestamp[1] = {
				(double) temporalDataPoint->getTimestamp()
			};
			timestamps->SetTuple(tupleNumber, timestamp);
		}

		tupleNumber++;
	}

	// Assign points and vertices to the output data set
	dataSet->SetPoints(points);
	dataSet->SetVerts(verts);

	// Add relevant data arrays depending on the data type
	switch (dataType) {
	case Data::CITIES: {
		vtkSmartPointer<vtkStringArray> cityNames = vtkSmartPointer<vtkStringArray>::New();
		cityNames->SetNumberOfComponents(relevantDataPoints.size());
		cityNames->SetName("names");

		for (QList<DataPoint*>::iterator iterator = relevantDataPoints.begin();
		        iterator != relevantDataPoints.end(); ++iterator) {
			const CityDataPoint* dataPoint = dynamic_cast<const CityDataPoint*>(
			                                     (*iterator)
			                                 );

			cityNames->InsertNextValue(dataPoint->getName().toStdString());
		}

		dataSet->GetPointData()->AddArray(cityNames);
		break;
	}

	case Data::FLIGHTS: {
		vtkSmartPointer<vtkDoubleArray> destinations = vtkSmartPointer<vtkDoubleArray>::New();
		vtkIdType numberOfTuples[1];
		numberOfTuples[0] = relevantDataPoints.size();
		destinations->SetNumberOfComponents(2);
		destinations->SetNumberOfTuples(*numberOfTuples);
		destinations->SetName("destinations");

		int tupleNumber = 0;
		for (QList<DataPoint*>::iterator iterator = relevantDataPoints.begin();
		        iterator != relevantDataPoints.end(); ++iterator) {
			const FlightDataPoint* dataPoint = dynamic_cast<const FlightDataPoint*>(
			                                       (*iterator)
			                                   );

			// Insert the new destination as a new tuple of latitude and longitude
			double coordinates[2] = {
				dataPoint->getDestination().lat(),
				dataPoint->getDestination().lon()
			};
			destinations->SetTuple(tupleNumber, coordinates);

			tupleNumber++;
		}

		dataSet->GetPointData()->AddArray(destinations);
		break;
	}

	case Data::TWEETS: {
		vtkSmartPointer<vtkStringArray> authors = vtkSmartPointer<vtkStringArray>::New();
		authors->SetNumberOfComponents(relevantDataPoints.size());
		authors->SetName("authors");

		vtkSmartPointer<vtkStringArray> contents = vtkSmartPointer<vtkStringArray>::New();
		contents->SetNumberOfComponents(relevantDataPoints.size());
		contents->SetName("contents");

		for (QList<DataPoint*>::iterator iterator = relevantDataPoints.begin();
		        iterator != relevantDataPoints.end(); ++iterator) {
			const TweetDataPoint* dataPoint = dynamic_cast<const TweetDataPoint*>(
			                                      (*iterator)
			                                  );

			authors->InsertNextValue(dataPoint->getAuthor().toStdString());
			contents->InsertNextValue(dataPoint->getContent().toStdString());
		}

		dataSet->GetPointData()->AddArray(authors);
		dataSet->GetPointData()->AddArray(contents);
		break;
	}

	case Data::PRECIPITATION: {
		vtkSmartPointer<vtkTypeFloat32Array> precipitationRates
		    = vtkSmartPointer<vtkTypeFloat32Array>::New();
		precipitationRates->SetNumberOfComponents(1);
		precipitationRates->SetNumberOfTuples(relevantDataPoints.size());
		precipitationRates->SetName("precipitationRates");

		vtkSmartPointer<vtkTypeInt32Array> precipitationTypes
		    = vtkSmartPointer<vtkTypeInt32Array>::New();
		precipitationTypes->SetNumberOfComponents(1);
		precipitationTypes->SetNumberOfTuples(relevantDataPoints.size());
		precipitationTypes->SetName("precipitationTypes");

		int tupleNumber = 0;
		for (QList<DataPoint*>::iterator iterator = relevantDataPoints.begin();
		        iterator != relevantDataPoints.end(); ++iterator) {
			const PrecipitationDataPoint* dataPoint
			    = dynamic_cast<const PrecipitationDataPoint*>((*iterator));

			double precipitationRate[1] = {
				(double) dataPoint->getPrecipitationRate()
			};
			precipitationRates->SetTuple(tupleNumber, precipitationRate);

			double precipitationType[1] = {
				(double) dataPoint->getPrecipitationType()
			};
			precipitationTypes->SetTuple(tupleNumber, precipitationType);

			tupleNumber++;
		}

		dataSet->GetPointData()->AddArray(precipitationRates);
		dataSet->GetPointData()->AddArray(precipitationTypes);
		break;
	}

	case Data::TEMPERATURE: {
		vtkSmartPointer<vtkTypeFloat32Array> temperatures
		    = vtkSmartPointer<vtkTypeFloat32Array>::New();
		temperatures->SetNumberOfComponents(1);
		temperatures->SetNumberOfTuples(relevantDataPoints.size());
		temperatures->SetName("temperatures");

		int tupleNumber = 0;
		for (QList<DataPoint*>::iterator iterator = relevantDataPoints.begin();
		        iterator != relevantDataPoints.end(); ++iterator) {
			const TemperatureDataPoint* dataPoint
			    = dynamic_cast<const TemperatureDataPoint*>((*iterator));

			double temperature[1] = {
				(double) dataPoint->getTemperature()
			};
			temperatures->SetTuple(tupleNumber, temperature);

			tupleNumber++;
		}

		dataSet->GetPointData()->AddArray(temperatures);
		break;
	}

	case Data::WIND: {
		vtkSmartPointer<vtkTypeFloat32Array> speeds
		    = vtkSmartPointer<vtkTypeFloat32Array>::New();
		speeds->SetNumberOfComponents(1);
		speeds->SetNumberOfTuples(relevantDataPoints.size());
		speeds->SetName("speeds");

		vtkSmartPointer<vtkTypeFloat32Array> directions
		    = vtkSmartPointer<vtkTypeFloat32Array>::New();
		directions->SetNumberOfComponents(1);
		directions->SetNumberOfTuples(relevantDataPoints.size());
		directions->SetName("directions");

		vtkSmartPointer<vtkTypeFloat32Array> velocities
		    = vtkSmartPointer<vtkTypeFloat32Array>::New();
		velocities->SetNumberOfComponents(3);
		velocities->SetNumberOfTuples(relevantDataPoints.size());
		velocities->SetName("velocity");

		int tupleNumber = 0;
		for (QList<DataPoint*>::iterator iterator = relevantDataPoints.begin();
		        iterator != relevantDataPoints.end(); ++iterator) {
			const WindDataPoint* dataPoint
			    = dynamic_cast<const WindDataPoint*>((*iterator));

			double speed[1] = {
				(double) dataPoint->getSpeed()
			};
			speeds->SetTuple(tupleNumber, speed);

			double direction[1] = {
				(double) dataPoint->getDirection()
			};
			directions->SetTuple(tupleNumber, direction);

			// Calculate each point's velocity for use in flow visualisation
			float windBearingRadian = dataPoint->getDirection() * (float)(KRONOS_PI / 180);

			double velocity[3] = {
				(double) dataPoint->getSpeed()* sin(windBearingRadian),
				(double) dataPoint->getSpeed()* cos(windBearingRadian),
				0.0
			};
			velocities->SetTuple(tupleNumber, velocity);

			tupleNumber++;
		}

		dataSet->GetPointData()->AddArray(speeds);
		dataSet->GetPointData()->AddArray(directions);
		dataSet->GetPointData()->AddArray(velocities);
		break;
	}

	case Data::CLOUD_COVERAGE: {
		vtkSmartPointer<vtkTypeFloat32Array> coverageValues
		    = vtkSmartPointer<vtkTypeFloat32Array>::New();
		coverageValues->SetNumberOfComponents(1);
		coverageValues->SetNumberOfTuples(relevantDataPoints.size());
		coverageValues->SetName("cloudCovers");

		int tupleNumber = 0;
		for (QList<DataPoint*>::iterator iterator = relevantDataPoints.begin();
		        iterator != relevantDataPoints.end(); ++iterator) {
			const CloudCoverageDataPoint* dataPoint
			    = dynamic_cast<const CloudCoverageDataPoint*>((*iterator));

			double coverage[1] = {
				(double) dataPoint->getCloudCoverage()
			};
			coverageValues->SetTuple(tupleNumber, coverage);

			tupleNumber++;
		}

		dataSet->GetPointData()->AddArray(coverageValues);
		break;
	}
	}

	// Finally, add the aforementioned common arrays if necessary
	if (temporal) {
		dataSet->GetPointData()->AddArray(timestamps);
	}

	dataSet->GetPointData()->AddArray(priorities);

	return dataSet;
}