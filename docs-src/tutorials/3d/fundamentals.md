When learning most programming languages the first thing people do is make
the computer print "Hello World!". For 3D, one of the most common first
things to do is to make a 3D cube. So let's start with "Hello Cube!"

Before we get started let's try to give you an idea of the structure of a
3D GUI app. This app requires you to create a bunch of objects and connect
them together. Here's a diagram that represents a small 3D app.

![3D App Structure](class:mx-auto:d-block:img/3d-structure.png)

Things to notice about the diagram above:

* There is an **Application**. This is the main object of GUI App. You add
widgets to an app and the **View** widget is used to render a 3D scene via
a specified **Camera**.
* There is a Scenegraph which is a tree like structure, consisting of
various objects like a **Background**, **Skybox**, **Meshes**, **Lights**
and **Fog**. A **Scene** object defines the
root of the Scenegraph in which objects define a hierarchical parent/child
tree like structure and represent where objects appear and how they are
oriented. Children are positioned and oriented relative to their parent.
For example the wheels on a car might be children of the car so that moving
and orienting the car's object automatically moves the wheels.
* **Mesh** and **Wireframe** objects represent drawing a specific
**Geometry** with a specific **Material**. Both **Material** objects and
**Geometry** objects can be used by multiple **Mesh** objects.
For example to draw two blue cubes in different locations we could
need two **Mesh** objects to represent the position and orientation of each
cube. We would only need one **Geometry** to hold the vertex data for a
cube and we would only need one **Material** to specify the color blue.
Both **Mesh** objects could reference the same **Geometry** object and the
same **Material** object.
* **Geometry** objects represent the vertex data of some piece of geometry
like a sphere, cube, plane, dog, cat, human, tree, building, etc... ObjectTalk provides many kinds of built in **Geometry** primitives. You can
also create custom geometry as well as load geometry models from files.
* **Material** objects represent the surface properties used to draw
geometry including things like the color to use and how shiny it is. A
**Material** can also reference a texture which can be used, for example,
to wrap an image onto the surface of a geometry.
* **Light** objects represent different kinds of lights.

Given all of that, we're going to make the smallest possible "Hello Cube"
App that looks like this:

![3D App Structure](class:mx-auto:d-block:img/3d-hello-cube.png)

Below is a skeleton ObjectTalk GUI App that forms the basis for every
application. You simply subclass the **Application** class, populate the
**setup** member function and run the App.

	var gui = import("gui");

	class App : gui.App {
		function setup(this) {
		}
	}

	var app = App();
	os.runGUI();

 This is a complete and valid App even though it doesn't do anything.
 You can launch the App and it will create a black screen or black
 window depending on your operating system.

 So let's start to populate the **setup** member function and first create
 a **Scene**.

 	var ambient = gui.AmbientLight();

	var geometry = gui.BoxGeometry();
	var material = gui.ColoredMaterial("gold");

	var cube = gui.Mesh(geometry, material);

	var scene = gui.Scene()
		.add(ambient)
		.add(cube);

Here we turn on the light (a simple ambient light source in this case)
to ensure we can actusally see our creation. Then we create a
**BoxGeometry** geometry followed by a **Material**
instance. Please note that we are going for the gold here in our first App
(this is not a low budget tutorial, is it). The **Mesh** uses the created
geometry and material. Our simple **Scene** can now be created as it only
has one member.

Now let's create a **Camera**:

	var camera = gui.Camera()
		.setPerspective(75, 0.1, 5)
		.setPosition(0, 0, 2);

FOV is short for Field Of View. In this case 75 degrees. Note that most
angles in ObjectTalk's GUI module are in radians but the camera takes
degrees for FOV. Read up onOpenGL and other libraries and you see where
this comes from. The clipping parameters express a near and far point.
near and far represent the space in front of the camera that will be
rendered. Anything before that range or after that range will be clipped
(not drawn).

Those settings define a "frustum". A frustum is the name of a 3D shape
that is like a pyramid with the tip sliced off. In other words, think of
the word "frustum" as another 3D shape like sphere, cube, prism, frustum.

![3D App Structure](class:mx-auto:d-block:img/3d-frustum.png)

The camera defaults to looking down the -Z axis with +Y up. We'll put our
cube at the origin so we need to move the camera back a little from the
origin in order to see anything. This is what **setPosition** does. So
here is what we are aiming for:

![3D App Structure](class:mx-auto:d-block:img/3d-scene-down.png)

In the diagram above, we can see our camera is at z = 2. It's looking down
the -Z axis. Our frustum starts 0.1 units from the front of the camera and
goes to 5 units in front of the camera.

Finally we create the **View**, connect the **Camera**, connect the
**Scene** and make it visible on the **Screen**:

	var view = gui.View()
		.setCamera(camera)
		.setScene(scene);

	screen.add(view);

Our complete App now looks like this:

	var gui = import("gui");

	class App : gui.App {
		function setup(this) {
			var ambient = gui.AmbientLight();

			var geometry = gui.BoxGeometry();
			var material = gui.ColoredMaterial("gold");
			var cube = gui.Mesh(geometry, material);

			var scene = gui.Scene()
				.add(ambient)
				.add(cube);

			var camera = gui.Camera()
				.setPerspective(75, 0.1, 5)
				.setPosition(0, 0, 2);

			var view = gui.View()
				.setCamera(camera)
				.setScene(scene);

			this.add(view);
		}
	}

	var app = App();
	os.runGUI();

Let's run this App and be amazed how underwhelming the result is.
It looks like we drew just a square. Yes, It's kind of hard to tell
that is a 3D cube since we're viewing it directly down the -Z axis and
the cube itself is axis aligned so we're only seeing a single face.

Let's animate it spinning and hopefully that will make it clear it's
being drawn in 3D. To animate it we'll add an animation to the App:

	this.addAnimation(gui.Animation()
		.from(0.0)
		.to(pi * 2.0)
		.during(15000)
		.continuous()
		.onStep(function(angle) {
			cube.rotateAroundVector(angle, 1, 1, 0);
		}));

This animation animates a real from 0.0 to 2PI during 15000 microseconds
(or 15 seconds) and repeats this endlessly. The number is than used to
rotate the cube around the [0, 0, 0] vector which makes it spin around
the X and Y axis.

We now start to see that we have a cube but it is start hard to make out
as all sides are colored the same way. The first thing we can do is
change the **Mesh** 3D object to a **Wireframe**. This makes it clear
that we truly have a cube.

The better solution is to add light to make it more realistic. So far,
our **Scene** only included a dim **Ambient** light so we could see our
creations. You can also add directional **Lights** that make it easier to
make the scene more realistic. So let's change the App to:

	var gui = import("gui");

	class App : gui.App {
		function setup(this) {
			var geometry = gui.BoxGeometry();
			var material = gui.ColoredMaterial("gold");
			var cube = gui.Mesh(geometry, material);

			this.addAnimation(gui.Animation()
				.from(0.0)
				.to(pi * 2.0)
				.during(15000)
				.continuous()
				.onStep(function(angle) {
					cube.rotateAroundVector(angle, 1, 1, 0);
				}));

			var ambient = gui.AmbientLight();
			var light = gui.DirectionalLight(-1, -2, -4);

			var scene = gui.Scene()
				.add(cube)
				.add(ambient)
				.add(light);

			var camera = gui.Camera()
				.setPerspective(75, 0.1, 5)
				.setPosition(0, 0, 2);

			var view = gui.View()
				.setCamera(camera)
				.setScene(scene);

			this.add(view);
		}
	}

	var app = App();
	os.runGUI();

It should now be pretty clearly 3D. Directional lights have a direction as
the name suggests In our case we're setting the light's direction to
1, 2, 4 so it's coming from the right, above, and behind our camera. A
slightly more complex version with three rotating cubes can be found
in the examples folder.
