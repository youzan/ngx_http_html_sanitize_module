var request = require("superagent");
var fs = require("fs");
var html = fs.readFileSync('./google.html')

console.time("test");
request
  .post("http://127.0.0.1:8888?document=1&context=0&html=1&element=0&attribute=0")
  .send(html)
  .end(function(err, res){
	if (err) {
		console.log(err);
		return;
	}
	console.log(html.length);
	console.timeEnd("test");
   });
