"use strict";

module.exports = require('./build/Release/segmentor.node')
module.exports.version = require('./package').version;
var default_model = __dirname + '/models/';
module.exports.registerModel(default_model)
