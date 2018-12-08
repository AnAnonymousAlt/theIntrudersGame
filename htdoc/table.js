  var at = "AccountTable";
  var ct = "CharactorTable";
  var rt = "RoomTable";
  var AWS = require("aws-sdk");
  AWS.config.update({
    region: "us-east-2",
    endpoint: "https://dynamodb.us-east-2.amazonaws.com"
  });
  var docClient = new AWS.DynamoDB.DocumentClient();

module.exports = {
  adda: function(email, password, nickname) {
    var params = {
      TableName: at,
      Item: {
	"password": password,
	"email": email,
	"nickname": nickname,
	"info": {
	  "rating": 0
	}
      }
    }

    console.log("Adding user");
    docClient.put(params, function(err,data) {
      if (err) {
	  console.error("Unable to add item. Error JSON:", JSON.stringify(err, null, 2));
      } else {
          console.log("Added item:", JSON.stringify(data, null, 2));
      }
    });
  },
  
  reada: function(email, pwd) {
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
	  return "";
      } else {
	  // console.log("GetItem succeeded:", JSON.stringify(data, null, 2));
	   var t = JSON.stringify(data, null, 2);
           //console.log(t);
	   var result = JSON.parse(t);

	  var txt = "" + result["Item"]["password"] +"";
	  console.log(txt);
	  return txt;
	  
	  //return t;

      }
    });
  }
}
