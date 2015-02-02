var http = require('http');
var express = require('express');
var fs = require('fs');
var RED = require("node-red");
var app = express();



var portDef = process.env.NODE_PORT || "80";
var userDir = __dirname + "/flows/";

// Add a simple route for static content served from 'public'
app.use("/",express.static(__dirname + "/public"));

// Create a server
var server = http.createServer(app);

// Create the settings object - see default settings.js file for other options
var settings = {
    httpAdminRoot:"/red",
    httpNodeRoot: "/api",
    flowFile: __dirname + "/flows/hackiot.json",
    userDir: userDir,
    functionGlobalContext: { }    // enables global context
};

// Initialise the runtime with a server and settings
RED.init(server,settings);

// Serve the editor UI from /red
app.use(settings.httpAdminRoot,RED.httpAdmin);

// Serve the http nodes UI from /api
app.use(settings.httpNodeRoot,RED.httpNode);

server.listen(portDef);

// Start the runtime
RED.start();

