# CSE167 Final Project Spring 2016

Progress of this application over the last couple weeks of the course can be found in the following blog:

http://cse167sp16.tumblr.com/

In short, this project features an island in the middle of the ocean. In a creative aspect, the island is uninhabited, has many hills, trees, and features a cartoon theme when toggled.

The technical features implemented in this are:

1. Water with both reflection and refraction of the skybox, and waves generated in the shader. 
- The water is created using Bezier surfaces by defining points and using a linear interpolation method to stitch them together. 
- The waves are calculated using the euclidean distance of the each vertex to the center of the mesh and using a sin/cos function on that distance with frequency and elapsed time to create waves. 
- The only problem with doing it this way is calculating normals for reflection which aren't as accurate as other methods. But given the time constraint, the waves were still impressive.


2. Procedurally generated terrain that takes in a height map: either real or generated from other applications and adds 3 different textures to it. 
- The textures are blended using a blend map to create smooth transitions between them. 
- The terrain features a grid system to add N x N terrains and stitch them together to create larger N x N terrains. 
- In order to smooth out the terrain, the diamond square algorithm is performed at most 2^n steps and a range for randomness. 
- To get the height at a certain position, we use barycentric coordinates.


3. Toon shading by calculating certain cut offs in the diffuse lighting and setting it to specific values when in certain ranges. This effectively sets the diffuse light to be extreme in certain ranges, making the entire scene look more dramatic.

4. Particle System to randomly generate instances of an object or geometry without any slowdown. This is done by generating many instances and maintaining more than 1,000 particles at once.

5. Collision detection using arbitrary objects and creating bounding boxes around them.
- When certain sections are inside another object, we determine if the objects collide through a specific threshold.





