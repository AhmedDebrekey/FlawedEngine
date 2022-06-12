#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
  
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

out vec3 ourColor; // output a color to the fragment shader

void main()
{
    gl_Position = Projection * View * Model * vec4(aPos, 1.0);
}     