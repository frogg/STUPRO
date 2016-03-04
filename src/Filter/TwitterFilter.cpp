#include <Filter/TwitterFilter.h>

#include <Reader/DataReader/Data.hpp>

#include <vtkObjectFactory.h>
#include <vtkStringArray.h>
#include <vtkIntArray.h>

TwitterFilter::TwitterFilter() {
	this->authorMatchingMode = CONTAINING;
}
TwitterFilter::~TwitterFilter() { }

vtkStandardNewMacro(TwitterFilter);

QList<Data::Type> TwitterFilter::getCompatibleDataTypes() {
	return (QList<Data::Type>() << Data::TWEETS);
}

bool TwitterFilter::evaluatePoint(int pointIndex, Coordinate coordinate,
                                  vtkPointData* pointData) {

	vtkSmartPointer<vtkStringArray> authorData = vtkStringArray::SafeDownCast(
	            pointData->GetAbstractArray("authors"));
	vtkSmartPointer<vtkStringArray> contentData = vtkStringArray::SafeDownCast(
	            pointData->GetAbstractArray("contents"));
	vtkSmartPointer<vtkIntArray> numberOfRetweets = vtkIntArray::SafeDownCast(
	            pointData->GetAbstractArray("numberOfRetweets"));


	if (!contentData || !authorData || !numberOfRetweets) {
		this->fail("The string arrays containing tweet contents, authors or the number of retweets seem to be invalid.");
		return false;
	}

	// Extract the actual content and author of the tweet we are currently looking at
	QString author = QString::fromStdString(authorData->GetValue(pointIndex));
	author.remove(' ');
	QString content = QString::fromStdString(contentData->GetValue(pointIndex));
	int retweets = numberOfRetweets->GetValue(pointIndex);

	// First of all, check if the tweet is visible based on its number of retweets
	if (!shouldDisplayBasedOnRetweets(retweets)) {
		return false;
	}

	// Check the visibility of this point based on the tweet's author and content
	if (this->visibleAuthors.count() == 0) {
		if (this->visibleKeywords.count() == 0) {
			return true;
		} else {
			return this->shouldDisplayBasedOnTweetContent(content);
		}
	} else {
		for (int i = 0; i < this->visibleAuthors.count(); i++) {
			if (this->authorMatchingMode == CONTAINING) {
				// Search for a contained author name
				if (author.contains(this->visibleAuthors.at(i), Qt::CaseInsensitive)
				        && this->shouldDisplayBasedOnTweetContent(content)) {
					return true;
				}
			} else if (this->authorMatchingMode == MATCHING) {
				// Search for an exact author name match
				if (QString::compare(author, this->visibleAuthors.at(i), Qt::CaseInsensitive) == 0
				        && shouldDisplayBasedOnTweetContent(content)) {
					return true;
				}
			}
		}
		return false;
	}
}

bool TwitterFilter::shouldDisplayBasedOnTweetContent(QString content) {
	if (this->visibleKeywords.count() == 0) {
		return true;
	}

	for (int i = 0; i < visibleKeywords.count(); i++) {
		// Only filter for hashtags
		if (content.contains(this->visibleKeywords.at(i), Qt::CaseInsensitive)) {
			return true;
		}
	}

	return false;
}

bool TwitterFilter::shouldDisplayBasedOnRetweets(int retweetNumber) {
	return this->lowerRetweetLimit <= retweetNumber && retweetNumber <= this->upperRetweetLimit;
}

void TwitterFilter::setAuthorMatchingMode(int matchingMode) {
	this->authorMatchingMode = static_cast<Mode>(matchingMode);
	this->Modified();
}

void TwitterFilter::setAuthors(const char* authors) {
	if (QString::fromStdString(authors).trimmed() == "") {
		this->visibleAuthors.clear();
	} else {

		QString authorList = QString::fromStdString(authors).remove(" ");

		this->visibleAuthors = authorList.split(",");

		for (int i = 0; i < this->visibleAuthors.size(); i++) {
			// Remove trailing @ symbols if necessary
			if (this->visibleAuthors[i].startsWith("@")) {
				this->visibleAuthors[i] = this->visibleAuthors[i].remove(0, 1);
			}
		}
	}

	this->Modified();
}

void TwitterFilter::setKeywords(const char* keywords) {
	if (QString::fromStdString(keywords).replace("#", "").trimmed() == "") {
		this->visibleKeywords.clear();
	} else {
		this->visibleKeywords = QString::fromStdString(keywords).split(",");
	}
	this->Modified();
}

void TwitterFilter::SetInputConnection(vtkAlgorithmOutput* input) {
	this->Superclass::SetInputConnection(input);
}

void TwitterFilter::setRetweetThreshold(double lowerLimit, double upperLimit) {
	this->lowerRetweetLimit = lowerLimit;
	this->upperRetweetLimit = upperLimit;
	this->Modified();
}

