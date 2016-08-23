var http = require('http');
var walk = require('walk');
var url = require('url');
var fs = require('fs');
var files = []
var index = fs.readFileSync('index.html');

try
{
  var geom = fs.readFileSync('../shining/3:8:4:18-11-4.json');
}
catch(e)
{
    console.log("Resource not found");
}

i = 0;
var walker = walk.walk('../shining', {followLinks: false});
walker.on('file', function(root, stat, next) {files.push(root+'/'+stat.name); next();})

http.createServer(function (req, res) {
  console.log(url.parse(req.url).pathname);
  parsed = url.parse(req.url);
  if(parsed.pathname == '/geom')
  {
    res.writeHead(200, {'Content-Type': 'text/plain'});
    res.end(geom);
  }
  else if(parsed.pathname == '/prev')
  {
    i--;
    if(i<0)
        i += files.length;
    if(i<0)
        i = 0;
    if(i<files.length)
        geom = fs.readFileSync(files[i]);
    console.log('Prev served '+files[i]);
    res.end(geom);
  }
  else if(parsed.pathname == '/next')
  {
    i++;
    if(i>=files.length)
        i -= files.length;
    if(i>=files.length)
        i = 0;
    if(i<files.length)
        geom = fs.readFileSync(files[i]);
    console.log('Next served '+files[i]);
    res.end(geom);
  }
  else
  {
    try
    {
      stats = fs.lstatSync('../shining/'+parsed.pathname.substring(1)+'.json');
      if(stats.isFile())
      {
        geom = fs.readFileSync('../shining/'+parsed.pathname.substring(1)+'.json');
        console.log('Shining served');
      }
    }
    catch(e)
    {
        try
        {
          geom = fs.readFileSync('../archimede_json/'+parsed.pathname.substring(1)+'.json');
          console.log('Archimede served');
        }
        catch(e)
        {
          console.log('Normal index served');
        }
    }

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
