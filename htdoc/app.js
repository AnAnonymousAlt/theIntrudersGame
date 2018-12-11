var express = require("express");
var app = express();
var server = require("http").Server(app);
var io = require("socket.io")(server);
var url = require("url");

server.listen(80, function() {
  console.log("server running on port 80");
});

app.get("/", function(req, res) {
  res.sendFile(__dirname + "/login.html");
});

app.get("/game.html", function(req, res) {
  res.sendFile(__dirname + "/game.html");
});

app.get('/welcome.html', function(req, res) {
  res.sendFile(__dirname + '/welcome.html');
});

app.get('/rooms.html', function(req, res) {
  res.sendFile(__dirname + '/rooms.html');
});

app.get("/pic/\*", function(req,res) {
  res.sendFile(__dirname + req.path);
});



var login_socket = io.of("/login");
login_socket.on("connection", function (socket) {
  var at = "AccountTable";
  var ct = "CharactorTable";
  var rt = "RoomTable";
  var AWS = require("aws-sdk");
  AWS.config.update({
    region: "us-east-2",
    endpoint: "https://dynamodb.us-east-2.amazonaws.com"
  });
  var docClient = new AWS.DynamoDB.DocumentClient();

  function reada(email, pwd) {
    console.log("Querying");
    var params = {
      TableName : at,
      Key : {
        "email" : email
      }
    };
    docClient.get(params, function(err, data) {
      if (err) {
          console.error("Unable to read item. Error JSON:", JSON.stringify(err, null, 2));
          socket.emit('abad');
      } else {
        try {
          // console.log("GetItem succeeded:", JSON.stringify(data, null, 2));
          var t = JSON.stringify(data, null, 2);
          console.log(t);
          var result = JSON.parse(t);
          var txt = result["Item"]["password"] +"";
          var pwd_t = pwd +"";
          if (txt == pwd_t) {
            console.log("nice");
            socket.emit('aok');}
          else socket.emit('abad');
          } catch (error) {
            socket.emit('abad');
          }
      }
    });
  }

  function adda(email, password) {
    var params = {
      TableName: at,
      Item: {
        "password": password,
        "email": email,
       // "nickname": nickname,
        "info": {
          "rating": 0
        }
      }
    }

    console.log("Adding user");
    docClient.put(params, function(err,data) {
      if (err) {
          console.error("Unable to add item. Error JSON:", JSON.stringify(err, null, 2));
          socket.emit('abad');
      } else {
          console.log("Added item:", JSON.stringify(data, null, 2));
          socket.emit('aok');

      }
    });
  }

  console.log(socket.handshake.address);
  const u = url.parse(socket.handshake.url);
  console.log("pathname: " + u.pathname);

  socket.on('login', function(msg) {
    reada(msg.username, msg.password);
  });

  socket.on('register', function(msg) {
    adda(msg.username, msg.password);
    });
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

var game_socket = io.of("/game");
game_socket.on("connection", function(socket) {
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
  socket.on("update_client", function(msg) {
    //console.log("update");
    if (update_counter < player_counter) {
      update_array.push(msg);
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
        game_socket.emit("update_server", update_message);
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
