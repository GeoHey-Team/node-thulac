"use strict";

var segmentor = require('./build/Release/segmentor.node')

exports.version = require('./package').version;

exports.segmentor = segmentor;
//exports.loadModel = segmentor.loadModel;
//exports.predict = segmentor.predict;

