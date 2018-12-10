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

app.get("/testkey", function(req, res) {
  res.sendFile(__dirname + "/testkey.html");
});

var player_counter = 0;
var update_counter = 0;
var update_checker = [0, 0];
var update_array = [];
var newgame = 1;
var boss = {
  x : 0,
  y : 0,
  width : 0,
  height : 0,
  color : "red",
  direction : 0,
  load : function() {
    boss.x = 100;
    boss.y = 50;
    boss.width = 30;
    boss.height = 60;
    direction = 0;
  },
  update : function() {
    if (boss.direction == 0) {
      boss.x += 3;
    }
    else {
      boss.x -= 3;
    }
    if (boss.x > 400) {
      boss.direction = 1;
    }
    if (boss.x < 100) {
      boss.direction = 0;
    }
  }
};


io.on("connection", function(socket) {
  if (newgame == 1) {
    boss.load();
    newgame = 0;
  }
  if (player_counter == 2) {
    socket.disconnect();
  }
  console.log("get connection");
  socket.on("gamestart", function(msg) {
    console.log("gamestart");
    socket.emit("approve", player_counter);
    player_counter++;
  });
  socket.on("update", function(msg) {
    //console.log("update");
    if (update_counter < player_counter) {
      update_array[msg.id] = msg;
      update_counter++;
      if ((msg.x <= boss.x + boss.width && msg.x + 50 >= boss.x) 
           && (msg.y <= boss.y + boss.height && msg.y + 50 >= boss.y)) {
        socket.broadcast.emit("lose", "green");
        socket.emit("win");
        update_array = [];
        update_counter = 0;
        player_counter = 0;
        newgame = 1;
      }
      else if (update_counter == player_counter) {
        boss.update();
        var update_message = {
          number_player : player_counter,
          boss_x : boss.x,
          boss_y : boss.y,
          boss_width : boss.width,
          boss_height : boss.height,
          boss_color : boss.color,
          player_array : update_array
        }
        io.emit("update", update_message);
        update_array = [];
        update_counter = 0;
      }
    }
  });
  socket.on("disconnect", function(msg) {
    console.log("diconnection");
    player_counter--;

    if (player_counter == 0) {
    
    }
  })
});
