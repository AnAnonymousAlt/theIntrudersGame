var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

app.get('/', function(req, res) {
  res.sendFile(__dirname + '/login.html');
});
app.get('/index.html', function(req, res) {
  res.sendFile(__dirname + '/index.html');
});
app.get('/welcome.html', function(req, res) {
  res.sendFile(__dirname + '/welcome.html');
});
app.get('/rooms.html', function(req, res) {
  res.sendFile(__dirname + '/rooms.html');
});
app.get('/background.jpeg', function(req, res) {
  res.sendFile(__dirname + '/background.jpeg');
});
app.get('/player1.jpeg', function(req, res) {
  res.sendFile(__dirname + '/player1.jpeg');
});
app.get('/player2.jpeg', function(req, res) {
  res.sendFile(__dirname + '/player2.jpeg');
});
app.get('/boss.jpeg', function(req, res) {
  res.sendFile(__dirname + '/boss.jpeg');
});

http.listen(3000, function() {
  console.log('Server running on port 3000');
})
