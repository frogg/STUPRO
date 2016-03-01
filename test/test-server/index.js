'use static';

console.log('Tile Server starting up...')

var express = require('express');
var fs = require('fs');

var satelliteImage = fs.readFileSync('./tiles/tile_0_0_0.png');
var heightmapImage = fs.readFileSync('./tiles/tile_0_0_0.bil16');

var app = express();

var standardRouter = express.Router();
standardRouter.get('/satelliteImagery', function(req, res) {
  sendSatelliteImage(res)();
});
standardRouter.get('/heightmap', function(req, res) {
  sendHeightmap(res)();
});
standardRouter.get('/invalid', function(req, res) {
  sendInvalid(res)();
});
app.use('/standard', standardRouter);

var delayRouter = express.Router();
delayRouter.get('/satelliteImagery', function(req, res) {
  setTimeout(sendSatelliteImage(res), 10000);
});
delayRouter.get('/heightmap', function(req, res) {
  setTimeout(sendHeightmap(res), 10000);
});
delayRouter.get('/invalid', function(req, res) {
  setTimeout(sendInvalid(res), 10000);
});
app.use('/delay', delayRouter);

app.listen('3000', function() {
  console.log('Tile Server started successfully');
});


function sendSatelliteImage(res) {
  return function() {
    res.set('Content-Type', 'image/png');
    res.send(satelliteImage);
  }
}

function sendHeightmap(res) {
  return function() {
    res.set('Content-Type', 'application/bil16');
    res.send(heightmapImage);
  }
}

function sendInvalid(res) {
  return function() {
    res.set('Content-Type', 'application/invalid');
    res.send('invalid content');
  }
}
