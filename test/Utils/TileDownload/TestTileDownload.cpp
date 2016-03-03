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
	this->testServer.waitForFinished();
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
	return true;
}

TEST_F(TestTileDownload, Client_RequestTile) {
	std::promise<ImageTile> promise;
	auto future = promise.get_future();

	TileRequestWorker::OnTileFetched onTileFetched = [&](ImageTile tile) {
		promise.set_value(tile);
	};

	TileRequestWorker::OnTileFetchFailed onTileFetchFailed = [&](std::exception const & error) {
		// make sure to only set an exception once
		if (future.wait_for(std::chrono::nanoseconds(0)) == std::future_status::timeout) {
			try {
				throw KronosException(QString(error.what()));
			} catch (...) {
				promise.set_exception(std::current_exception());
			}
		}
	};

	ClientTileRequestWorker* worker;

	postToMainThread([&] {
		QSet<QString> requestedLayers;
		requestedLayers << "satelliteImagery" << "heightmap";

		worker = new ClientTileRequestWorker(requestedLayers, onTileFetched, onTileFetchFailed, "./res/layers.json");
		worker->requestTile(0, 0, 0);
	});


	ImageTile tile;
	ASSERT_NO_THROW(tile = future.get());

	EXPECT_EQ(0, tile.getZoomLevel());
	EXPECT_EQ(0, tile.getTileX());
	EXPECT_EQ(0, tile.getTileY());

	EXPECT_TRUE(tile.getLayers().contains("satelliteImagery"));
	EXPECT_TRUE(tile.getLayers().contains("heightmap"));

	// make sure to delete the worker on Qt's main thread
	worker->deleteLater();
}

TEST_F(TestTileDownload, Client_AbortAllRequests) {
	std::promise<ImageTile> promise;
	auto future = promise.get_future();

	TileRequestWorker::OnTileFetched onTileFetched = [&](ImageTile tile) {
		promise.set_value(tile);
	};

	TileRequestWorker::OnTileFetchFailed onTileFetchFailed = [&](std::exception const & error) {
		// make sure to only set an exception once
		if (future.wait_for(std::chrono::nanoseconds(0)) == std::future_status::timeout) {
			promise.set_exception(make_exception_ptr(std::runtime_error(error.what())));
		}
	};

	ClientTileRequestWorker* worker;

	postToMainThread([&] {
		QSet<QString> requestedLayers;
		requestedLayers << "satelliteImageryDelay" << "heightmapDelay";

		worker = new ClientTileRequestWorker(requestedLayers, onTileFetched, onTileFetchFailed, "./res/delayed-layers.json");
		worker->requestTile(0, 0, 0);
	});

	// not very nice, but the tile download is guaranteed to take around 10seconds, so waiting for
	// .5 seconds ensures that the tile request was processed and a download was started, but it
	// didn't finish. (Not too nice, but there are no other means of 'synchronization' that would
	// work here without modifying the whole downloader part again)
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	worker->requestAbort();

	ImageTile tile;
	try {
		tile = future.get();
		FAIL() << "future.get() should throw an exception";
	} catch (std::exception const& e) {
		EXPECT_TRUE(QString(e.what()).startsWith("The download was aborted."))
				<< "future.get() should throw a DownloadAbortedException";
	}

	// make sure to delete the worker on Qt's main thread
	worker->deleteLater();
}
