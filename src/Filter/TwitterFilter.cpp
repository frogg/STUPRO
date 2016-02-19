#include <Filter/TwitterFilter.h>

#include <Reader/DataReader/Data.hpp>

#include <vtkObjectFactory.h>
#include <vtkStringArray.h>

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

	if (!contentData || !authorData) {
		this->fail("The string arrays containing tweet contents or authors seem to be invalid.");
		return false;
	}

	// Extract the actual content and author of the tweet we are currently looking at
	QString author = QString::fromStdString(authorData->GetValue(pointIndex));
	author.remove(' ');
	QString content = QString::fromStdString(contentData->GetValue(pointIndex));

	//if no author is in authors and no content is in visibleContent (everything should be visible by default), return this point to be visible
	if (this->authors.count() == 0) {
		if (this->hashtags.count() == 0) {
			return true;
		} else {
			return this->shouldDisplayTweetContent(content);
		}
	} else {

		for (int i = 0; i < this->authors.count(); i++) {

			if (this->authorMatchingMode == CONTAINING) {
				//containing Mode
				if (author.contains(this->authors.at(i), Qt::CaseInsensitive)
				        && this->shouldDisplayTweetContent(content)) {
					return true;
				}
			} else if (this->authorMatchingMode == MATCHING) {
				//exact match
				if (QString::compare(author, this->authors.at(i), Qt::CaseInsensitive) == 0
				        && shouldDisplayTweetContent(content)) {
					return true;
				}
			}
		}
		return false;
	}
}

bool TwitterFilter::shouldDisplayTweetContent(QString content) {
	if (this->hashtags.count() == 0) {
		return true;
	}
	for (int i = 0; i < hashtags.count(); i++) {
		//only filter for hashtags
		if (content.contains(QString("#%1").arg(this->hashtags[i]), Qt::CaseInsensitive)) {
		//if (content.contains(this->hashtags[i], Qt::CaseInsensitive)) {
			return true;
		}
	}
	return false;
}

void TwitterFilter::setAuthorMatchingMode(int matchingMode) {
	this->authorMatchingMode = static_cast<Mode>(matchingMode);
	this->Modified();
}

void TwitterFilter::setAuthors(const char* authors) {
	if (QString::fromStdString(authors).trimmed() == "") {
		this->authors.clear();
	} else {

		QString authorList = QString::fromStdString(authors).remove(" ");

		this->authors = authorList.split(",");

		for (int i = 0; i < this->authors.size(); i++) {
			// Remove trailing @ symbols if necessary
			if (this->authors[i].startsWith("@")) {
				this->authors[i] = this->authors[i].remove(0, 1);
			}
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

