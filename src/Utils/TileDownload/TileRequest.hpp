#ifndef KRONOS_UTILS_TILE_DOWNLOAD_TILE_REQUEST_HPP
#define KRONOS_UTILS_TILE_DOWNLOAD_TILE_REQUEST_HPP

class TileRequestWorker;

class WorkerRequest {
public:
	WorkerRequest() = default;
	virtual ~WorkerRequest() = default;

	virtual void handle(TileRequestWorker* worker) = 0;
};

class TileRequest : public WorkerRequest {
public:
	int zoom;
	int x;
	int y;

	TileRequest(int zoom, int x, int y);
	virtual ~TileRequest() = default;

	virtual void handle(TileRequestWorker* worker);
};

class AbortRequest : public WorkerRequest {
public:
	AbortRequest() = default;
	virtual ~AbortRequest() = default;

	virtual void handle(TileRequestWorker* worker);
};

#endif // KRONOS_UTILS_TILE_DOWNLOAD_TILE_REQUEST_HPP
