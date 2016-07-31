var http = require('http');
var url = require('url');
var fs = require('fs');
var index = fs.readFileSync('index.html');
var geom = fs.readFileSync('biseauted_cube.json');

http.createServer(function (req, res) {
  console.log(url.parse(req.url).pathname);
  parsed = url.parse(req.url);
  if(parsed.pathname == '/geom')
  {
    res.writeHead(200, {'Content-Type': 'text/plain'});
    res.end(geom);
  }
  else
  {
    try
    {
      stats = fs.lstatSync(parsed.pathname.substring(1));
      if(stats.isFile())
    	  res.end(fs.readFileSync(parsed.pathname.substring(1)));
    }
    catch(e)
    {res.end(index);}
  }

  console.log('Page served');
}).listen(8181);