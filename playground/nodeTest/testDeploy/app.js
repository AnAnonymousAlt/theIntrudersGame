var http = require('http');
var dt = require('./myfirstmodule');
var fs = require('fs');
var AWS = require('aws-sdk');
AWS.config.region = process.env.REGION
http.createServer(function (req, res) {
  fs.readFile('demofile1.html', function(err, data) {
    res.writeHead(200, {'Content-Type': 'text/html'});
    res.write(data); 
    res.end();
  });
}).listen(8080);
