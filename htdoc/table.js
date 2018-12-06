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
    /*
    var at = "AccountTable";
    var ct = "CharactorTable";
    var rt = "RoomTable";
    var AWS = require("aws-sdk");
    AWS.config.update({
      region: "us-east-2",
      endpoint: "https://dynamodb.us-east-2.amazonaws.com"
    });
    var docClient = new AWS.DynamoDB.DocumentClient(); 
    */
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
  }

}
