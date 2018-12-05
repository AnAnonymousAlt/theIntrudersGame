var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

app.get('/', function(req, res) {
  res.sendFile(__dirname + '/index.html');
});

io.on('connection', function(socket) {
  console.log('An user connected.');
  socket.on('update', function(msg) {
    var pos = msg;
    console.log('x: ' + pos.x + '\t\t\ty:' + pos.y);
    io.emit('update', msg);
  });
  socket.on('disconnect', function() {
    console.log('user disconnected');
  });
});

http.listen(3000, function() {
  console.log('Server running on port 3000');
})
