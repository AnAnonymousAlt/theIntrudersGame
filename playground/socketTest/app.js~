var app = require('express')();
var server = require('http').Server(app);
var io = require('socket.io')(server);
var url = require('url');

server.listen(3000, function() {
  console.log('server running on port 3000');  
}

app.get('/', function(req, res) {
  res.sendFile(__dirname + '/game.html');
  io.on('connection', function(socket) {
  });  
});
