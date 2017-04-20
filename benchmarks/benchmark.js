const apiBenchmark = require('api-benchmark');
const fs = require("fs");
const glob = require("glob")

glob("*.html", {}, function (err, files) {
    const service = {
        gumbo: "http://127.0.0.1:8888"
    };
    const routes = {};
    const fc = {};

    files.forEach((file) => {
	fc[file] = fs.readFileSync(file);
        routes[file] = {
            method: 'post',
            route: '/',
            data: fc[file]
        }
    })

    apiBenchmark.measure(service, routes, function(err, results){
        if (err) {
            throw err;
        }
	
        files.forEach((file) => {
	    var len = fc[file].length / 1024;
            var mean = results.gumbo[file].stats.mean * 1000;
            var p99 = results.gumbo[file].stats.p99 * 1000;
            console.log(`${file} with ${len}KB took mean ${mean}ms p99 ${p99}ms`);
        });
    });
});
