#include <gtest/gtest.h>

#include <Utils/Misc/Exceptions.hpp>
#include <Utils/Misc/KronosLogger.hpp>
#include <Utils/Misc/MakeUnique.hpp>
#include <Utils/Misc/QtUtils.hpp>
#include <Utils/TileDownload/ClientTileRequestWorker.hpp>
#include <Utils/TileDownload/ServerTileRequestWorker.hpp>
#include <Utils/TileDownload/TileRequestWorker.hpp>

#include <QCoreApplication>
#include <QProcess>
#include <QRegExp>
#include <QTimer>

#include <exception>
#include <future>

#include <thread>

class TestTileDownload : public ::testing::Test {
public:
	TestTileDownload();
	~TestTileDownload();

	static bool nodeAvailable();

protected:
	QProcess testServer;
};

TestTileDownload::TestTileDownload() {
	if (!TestTileDownload::nodeAvailable()) {
		throw KronosException("Node not found");
	}
	QProcess npmInstall;
	npmInstall.setWorkingDirectory("./test-server");
	npmInstall.start("npm install");
	if (!npmInstall.waitForFinished()) {
		throw KronosException("Error executing `npm install`");
	}

	this->testServer.setWorkingDirectory("./test-server");
	this->testServer.start("npm start");
	if (!this->testServer.waitForStarted()) {
		throw KronosException("Error running `npm start`");
	}
}

TestTileDownload::~TestTileDownload() {
	this->testServer.kill();
}

bool TestTileDownload::nodeAvailable() {
	QProcess nodeVersion;
	nodeVersion.start("node --version");
	if (!nodeVersion.waitForStarted()) {
		return false;
	}
	if (!nodeVersion.waitForFinished()) {
		return false;
	}
	QString output = nodeVersion.readAllStandardOutput();
	return QRegExp("^v\\d\\.\\d\\.\\d\n$").exactMatch(output);
}

TEST_F(TestTileDownload, Client_RequestTile) {
	std::promise<ImageTile> promise;
	auto future = promise.get_future();

	TileRequestWorker::OnTileFetched onTileFetched = [&](ImageTile tile) {
		KRONOS_LOG_DEBUG("fetched");
		promise.set_value(tile);
	};

	TileRequestWorker::OnTileFetchFailed onTileFetchFailed = [&](std::exception const & error) {
		KRONOS_LOG_DEBUG("failed");
		// make sure to only set an exception once
		if (future.wait_for(std::chrono::nanoseconds(1)) == std::future_status::timeout) {
			try {
				throw KronosException(QString(error.what()));
			} catch (...) {
				promise.set_exception(std::current_exception());
			}
		}
	};

	std::unique_ptr<ClientTileRequestWorker> worker;

	postToMainThread([&] {
		QSet<QString> requestedLayers;
		requestedLayers << "satelliteImagery" << "heightmap";

		worker = makeUnique<ClientTileRequestWorker>(requestedLayers, onTileFetched, onTileFetchFailed, "./res/layers.json");
		KRONOS_LOG_DEBUG("initialised worker");

		worker->requestTile(0, 0, 0);
		KRONOS_LOG_DEBUG("requested tile");
	});


	ImageTile tile;
	ASSERT_NO_THROW(tile = future.get());

	// EXPECT_EQ(2, tile.getLayers());
	EXPECT_EQ(0, tile.getZoomLevel());
	EXPECT_EQ(0, tile.getTileX());
	EXPECT_EQ(0, tile.getTileY());
}

TEST_F(TestTileDownload, AbortAllRequests) {

}
