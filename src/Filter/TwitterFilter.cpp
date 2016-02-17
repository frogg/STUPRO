#include <Filter/TwitterFilter.h>

#include <Reader/DataReader/Data.hpp>

#include <vtkObjectFactory.h>
#include <vtkStringArray.h>

TwitterFilter::TwitterFilter() { }
TwitterFilter::~TwitterFilter() { }

vtkStandardNewMacro(TwitterFilter);

QList<Data::Type> TwitterFilter::getCompatibleDataTypes() {
	return (QList<Data::Type>() << Data::TWEETS);
}

bool TwitterFilter::evaluatePoint(int pointIndex, Coordinate coordinate, vtkPointData* pointData) {
	vtkSmartPointer<vtkStringArray> contents = vtkStringArray::SafeDownCast(
	            pointData->GetAbstractArray("contents"));
	vtkSmartPointer<vtkStringArray> authors = vtkStringArray::SafeDownCast(
	            pointData->GetAbstractArray("authors"));

	if (!contents || !authors) {
		this->fail("The string arrays containing tweet contents or authors seem to be invalid.");
		return false;
	}

	// Extract the actual content and author of the tweet we are currently looking at
	QString content = QString::fromStdString(contents->GetValue(pointIndex));
	QString author = QString::fromStdString(authors->GetValue(pointIndex));

	// Decide whether to display this tweet depending on the user's input
	if (this->authors.contains(author) || this->authors.size() == 0) {
		if (this->hashtags.size() == 0) {
			return true;
		} else {
			for (int i = 0; i < this->hashtags.size(); i++) {
				if (content.contains(QString("#%1").arg(this->hashtags[i]))) {
					return true;
				}
			}

			return false;
		}
	} else {
		return false;
	}
}

void TwitterFilter::setAuthors(const char* authors) {
	if (QString::fromStdString(authors).trimmed() == "") {
		this->authors.clear();
	} else {
		this->authors = QString::fromStdString(authors).split(",");

		for (int i = 0; i < this->authors.size(); i++) {
			this->authors[i] = this->authors[i].trimmed();
		}
	}

	this->Modified();
}

void TwitterFilter::setHashtags(const char* hashtags) {
	if (QString::fromStdString(hashtags).replace("#", "").trimmed() == "") {
		this->hashtags.clear();
	} else {
		this->hashtags = QString::fromStdString(hashtags).split(",");

		for (int i = 0; i < this->hashtags.size(); i++) {
			this->hashtags[i] = this->hashtags[i].trimmed();
			// Remove trailing hashtag symbols if necessary
			if (this->hashtags[i].startsWith("#")) {
				this->hashtags[i] = this->hashtags[i].remove(0, 1).trimmed();
			}
		}
	}

	this->Modified();
}

void TwitterFilter::SetInputConnection(vtkAlgorithmOutput* input) {
	this->Superclass::SetInputConnection(input);
}

