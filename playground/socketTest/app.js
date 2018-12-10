var express = require("express");
var app = express();
var server = require("http").Server(app);
var io = require("socket.io")(server);
var url = require("url");

server.listen(3000, function() {
  console.log("server running on port 3000");  
});

app.get("/", function(req, res) {
  res.sendFile(__dirname + "/game.html");
});

app.get("/canvas", function(req, res) {
  res.sendFile(__dirname + "/gamecanvas.html");
});
app.get("/pic/\*", function(req,res) {
  res.sendFile(__dirname + req.path);
});


var update_message = {
  player1 : -1,
  player2 : -1,
  player1x : -1,
  player1y : -1,
  player2x : -1,
  player2y : -1
};
var player_counter = 0;
var update_counter = 0;

io.on("connection", function(socket) {
  console.log("get connection");
  socket.on("gamestart", function(msg) {
    console.log("gamestart");
    socket.emit("approve", player_counter);
    player_counter++;
  });
  socket.on("update", function(msg) {
    console.log("update");
  });
  socket.on("disconnect", function(msg) {
    console.log("diconnection");
  })
});
