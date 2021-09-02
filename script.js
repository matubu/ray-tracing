class Vector3d {
//	x: Number;
//	y: Number;
//	z: Number;

	constructor(x/*: Number*/, y/*: Number*/, z/*: Number*/)
	{
		this.x = x;
		this.y = y;
		this.z = z;
	}
}

class Ray {
//	pos: Vector3d;
//	dir: Vector3d;

	constructor(pos/*: Vector3d*/, dir/*: Vector3d*/)
	{
		this.pos = pos;
		this.dir = dir;
	}
}

class Color {
//	red: Number;
//	green: Number;
//	blue: Number;

	constructor(red/*: Number*/, green/*: Number*/, blue/*: Number*/)
	{
		this.red = red;
		this.green = green;
		this.blue = blue;
	}
}

class Material {
//	color: Color;

	constructor(color: Color)
	{
		this.color = color;
	}
}

class Tris {
	points: [Vector3d, Vector3d, Vector3d]
	//normal: Pos;
	material: Material;

	constructor(points: /*[Vector3d, Vector3d, Vector3d]*/, material/*: Material*/)
	{
		this.points = points;
		//this.normal = Tris.calculateNormal(points);
		this.material = material;
	}
	/*static calculateNormal(points: [Pos, Pos, Pos]): Pos
	{
		return (new Pos(0,0,0));
	}*/
	collide(ray: Ray): Ray
	{
		return null;
	}
}

class Sphere {
	pos: Vector3d;
	rad: Number;
	material: Material;

	constructor(pos: Vector3d, rad: Number, material: Material)
	{
		this.pos = pos;
		this.rad = rad;
		this.material = material;
	}
	collide(ray: Ray): Ray
	{
		return null;
	}
}

class Light extends Sphere {
	color: Color;
	intensity: Number;

	constructor(pos: Vector3d, rad: Number, color: Color, intensity: Number)
	{
		super(pos, rad, null);

		this.color = color;
		this.intensity = intensity;
	}
}

class Object3D {
}

class Scene {
	objects: Object3D[];
	lights: Light[];

	constructor()
	{
		this.objects = [];
		this.lights = [];
	}
	addObject(object: Object3D)
	{
		this.objects.push(object);
	}
	addLight(light: Light)
	{
		this.lights.push(light);
	}
}

class Renderer {
	scene: Scene;
	ctx: CanvasRenderingContext2D;

	constructor(scene: Scene, ctx: CanvasRenderingContext2D)
	{
		this.scene = scene;
		this.ctx = ctx;
	}
	render()
	{

	}
}

const canvas: HTMLCanvasElement = document.querySelector('#canvas'),
	ctx = canvas.getContext('2d'),
	scene = new Scene(),
	renderer = new Renderer(scene, ctx);

scene.addObject();
renderer.render();
