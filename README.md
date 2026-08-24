# CS-330-Project-3D-Scene
# Anna Karelina
## Final Project 

## Justify Development Choices for the 3D Scene

For my 3D scene, I chose to create a still-life scene with a cake, coffee cup, and vase with a tulip. In my original plan, I also wanted to include a teapot, but I decided to focus on the other objects. I chose this type of scene because baking is one of my hobbies. The original picture also had objects that could be made using simple 3D shapes.

I used basic shapes to make the objects easier to build. The cake uses a tapered cylinder for the main part and another tapered cylinder for the frosting. I used small spheres for the sliced almonds on top. The coffee cup uses a cylinder for the cup and a half-torus for the handle. The vase uses a cone for the main part and a cylinder for the neck. The tulip uses a sphere for the flower and a cylinder for the stem.

I also used textures, colors, and materials to make the objects look more like the original picture. I added several lights with different colors so the objects could be seen clearly. I used ambient, diffuse, and specular lighting to add light and highlights to the objects.

The functions such as `SetTransformations()`, `SetShaderColor()`, `SetShaderTexture()`, and `SetShaderMaterial()` were used to organize my code. These functions can be used for different objects, so I do not have to write the same code each time. I also used the shape functions from `ShapeMeshes` to create the objects. This made it easier to build the scene and change the size, position, color, and texture of each object.

## Explain How a User Can Navigate Your 3D Scene

The user can move around my 3D scene using the keyboard and mouse. The **W, A, S, and D** keys move the camera forward, left, backward, and right. The **Q and E** keys move the camera up and down. The mouse lets the user look around and change the direction of the camera. The mouse wheel can also be used to change the camera zoom. The **P** key switches to perspective view, and the **O** key switches to orthographic view. This makes it possible to see the objects from different angles.

## Explain the Custom Functions in Your Program

The `SetTransformations()` function sets the size, rotation, and position of each object. I can use this function for different objects by changing the values.

The `SetShaderColor()` function sets the color of an object. The `SetShaderTexture()` function applies a texture to an object, and `SetTextureUVScale()` controls the scale of the texture. The `SetShaderMaterial()` function sets the material properties used for ambient, diffuse, and specular lighting.

I also used the functions from `ShapeMeshes` to load and draw the basic 3D shapes. I can use these functions more than once when I need the same shape. This keeps the code organized and makes it easier to make changes to the scene.

# Reflection on My Project

## 1. How Do I Approach Designing Software?

This project helped me learn how to break a larger idea into smaller parts. Instead of trying to create the whole scene at once, I first looked at the objects in the picture and decided which basic shapes could be used to create them. For example, I used cylinders and spheres for many of the objects.

I also learned that planning the position, size, and rotation of objects is important. I worked on each object separately and then adjusted them to make the whole scene look right. I can use this same approach in future projects by breaking a large problem into smaller parts before writing all of the code.

## 2. How Do I Approach Developing Programs?

While working on the project, I used functions to avoid repeating the same code. I also changed values such as the position, scale, rotation, colors, textures, and lighting while working on the scene.

Iteration was a big part of the project. I often ran the program, looked at the scene, found something that did not look right, and then changed the code. For example, I adjusted the position and size of objects several times until they looked closer to the original picture. I also made changes to the lighting because some objects did not look right at first.

During the milestones, I became more comfortable changing the code and testing small changes instead of trying to fix everything at once. This helped me finish the scene and understand how the different parts of the program work together.

## 3. How Can Computer Science Help Me Reach My Goals?

This project gave me more experience with 3D graphics, transformations, lighting, textures, and cameras. I learned how code can be used to create something that can be seen and controlled on the screen. These skills can be useful in my future education because they give me more experience with programming and problem-solving.

The project also helped me practice working with C++ and debugging code. These skills can be useful in my future professional work in software development. Even if I do not work directly with computer graphics, understanding how graphics programs work can help me work with different types of software and understand how different parts of a program connect.
