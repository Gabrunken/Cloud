<h1>Cloud</h1>

<h3>Description</h3>

Cloud is a 2D/3D game engine made for fun, with the intent to be as simple and straight forward as possible.
NOTE: It is to this day in its very early stages and not fully functional.

<h3>Build</h3>

You'll need cmake.

> run "cmake -S . -B build"
> flags:
> - -DSHOW\_CONSOLE=ON, this will write debug info and show the console

<h3>What Cloud aims at</h3>

The engine aims to be very performant and fairly capable on 3D rendering, and wants to have a simple
API for the user, for both scripting in-game logic, to plugins, to actual editor use.
I would like plugins to be a thing, so that people can expand on the engine/editor as they want.
Cross-platform implementations are also planned and will be built from the ground up.
Also it is very important to me to have a nice and simple Doc for the whole software, covering every possible
aspect, from custom format details (import, export), to API usage, to editor navigation.

<h3>Assets</h3>

The engine uses custom file formats for ease of development and quick asset integration, but it is planned
to support at least the industry standars (fbx, obj, gltf...).

<h3>Other details</h3>

It is made in C++, with a main OpenGL rendering backend. OpenXR will probably be implemented too, but
this is a late decision which will take place when everything is already set up.

<h3>Dependencies</h3>

  - GLFW3 - cross-platform window and input handling
  - glad - OpenGL function loader
  - stb\_image - image loading
  - stb\_dxt - texture compression
  - nlohmann/json - json serialization
  - glm - linear algebra math
  - gtui - cross-platform terminal interaction
  - tracy - profiler

<h3>Issues / Pull Requests</h3>

Anyone is welcomed to find and fix issues issues within the repository, if they wish.
For now i'm the only maintainer, but maybe there will be a day where someone joins the ship.
