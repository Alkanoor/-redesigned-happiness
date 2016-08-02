var scene, camera, renderer;
var geometry, material, mesh;

init();

function parse_to_geom(vertices, faces)
{
    var geometry = new THREE.Geometry();
    for(i=0; i<vertices.length; i+=3)
	geometry.vertices.push(new THREE.Vector3(200*vertices[i],200*vertices[i+1],200*vertices[i+2]));
    
    for(i=0; i<faces.length; i++)
        for(j=2; j<faces[i].length; j++)
	    geometry.faces.push(new THREE.Face3(faces[i][0],faces[i][j-1],faces[i][j]));
    
    return geometry;
}

function init()
{
    scene = new THREE.Scene();
	
    camera = new THREE.OrthographicCamera(window.innerWidth/-2, window.innerWidth/2, window.innerHeight/2, window.innerHeight/-2, 0.1, 10000);
    //camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 1, 10000);
    camera.position.z = 1000;
    
    $.get("http://localhost:8181/geom", function(result) {
	  console.log(result);
	  tmp = JSON.parse(result);
	  console.log(tmp);
    	  geometry = parse_to_geom(tmp.vertices, tmp.faces);
   	  console.log(geometry);
	  finish_init();
	  animate();
	});
}

function finish_init()
{
    console.log(geometry);
    geometry.computeBoundingSphere();

    material = new THREE.MeshBasicMaterial({color: 0xff0000, wireframe: true});

    mesh = new THREE.Mesh(geometry, material);
    scene.add(mesh);

    renderer = new THREE.WebGLRenderer();
    renderer.setSize(window.innerWidth, window.innerHeight);

    document.body.appendChild(renderer.domElement);
    
    pause = 0;
    window.addEventListener('keydown', function(event) {
  	switch(event.keyCode) {
            case 32:
	        pause = 1-pause;
	    break;}
        }, false);
}

function animate()
{
    requestAnimationFrame(animate);
    
    if(!pause)
    {
        mesh.rotation.x += 0.01;
        mesh.rotation.y += 0.02;
    }

    renderer.render(scene, camera);
}
