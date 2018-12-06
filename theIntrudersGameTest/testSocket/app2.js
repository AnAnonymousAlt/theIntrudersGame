var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

app.get('/', function(req, res) {
  res.sendFile(__dirname + '/index.html');
});

var counter = -1;
var array = [];
var uc = [0]; // update counter
io.on('connection', function(socket) {
  console.log('An user connected.');
  
  socket.on('request', function(msg) {
    console.log('An user sent a request');
    counter++;
    socket.emit('accept', counter);
    console.log('new userid: ' + counter);
  });  
  
  socket.on('update', function(msg) {
    //console.log('x: ' + msg.x + '\t\t\ty: ' + msg.y + '\t\t\tid: ' + msg.id);
    console.log('msg: ' + msg.x + msg.y + msg.id);
    array.push(msg);
    uc[msg.id] = 1;
    if (uc == counter) {
      console.log('array: ' + array[0].x + array[0].y + array[0].id);
      if (counter >= 1) console.log('\t' +array[1].x + array[1].y + array[1].id);
      io.emit('update', array);
      uc = 0;
    }
  });
  
  socket.on('disconnect', function() {
    console.log('user disconnected');
  });

});

http.listen(3000, function() {
  console.log('Server running on port 3000');
})
