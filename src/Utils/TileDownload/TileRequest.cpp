#include <Utils/TileDownload/TileRequest.hpp>
#include <Utils/TileDownload/TileRequestWorker.hpp>

TileRequest::TileRequest(int zoom, int x, int y) : zoom(zoom), x(x), y(y) { }

void TileRequest::handle(TileRequestWorker* worker) {
    worker->handleTileRequest(this);
}

void AbortRequest::handle(TileRequestWorker* worker) {
    worker->handleAbortRequest();
}
