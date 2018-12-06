var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var tt = require('./table');

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

var counter = -1;
var array = [];
var uc = []; // update counter
var ucc = -1;

tt.adda("li@p.c", "asdf", "haha");


io.sockets.on('connection', function(socket) {
  console.log('An user connected.');

  socket.on('request', function(msg) {
    console.log('An user sent a request');
    counter++;
    socket.emit('accept', counter);
    console.log('new userid: ' + counter);
    uc.push(0);
  });

  socket.on('update', function(msg) {
    //console.log('x: ' + msg.x + '\t\t\ty: ' + msg.y + '\t\t\tid: ' + msg.id);
    console.log('msg: ' + msg.x + ' ' + msg.y + ' '  + msg.id);
    console.log('uc: ' + uc + 'uc[0]: ' + uc[0]);
    console.log('ucc: ' + ucc);
    console.log('counter: ' + counter);
    if (uc[msg.id] == 0) {
      array.push(msg);
      uc[msg.id] = 1;
      ucc = ucc + 1;
    }
    console.log('');
    console.log('msg: ' + msg.x + ' ' + msg.y + ' '  + msg.id);

    console.log('uc: ' + uc + 'uc[0]: ' + uc[0]);
    console.log('ucc: ' + ucc);
    console.log('counter: ' + counter);

    console.log('-------------');
    if (ucc == counter && ucc != -1) {
      console.log('array: ' + array[0].x + ' ' + array[0].y + ' ' + array[0].id);
      if (counter >= 1) console.log('\t' +array[1].x + array[1].y + array[1].id);
      io.emit('update', array);
      var i = 0;
      while (i <= counter) {
        uc[i] = 0;
        i++;
      }
      array = [];
      ucc = -1;
    }
  });
  socket.on('win', function() {
    socket.broadcast.emit('lose');
  });
  socket.on('disconnect', function() {
    counter--;
    console.log('user disconnected');
  });
});
http.listen(80, function() {
    console.log('Server running on port 80(http)');
});
