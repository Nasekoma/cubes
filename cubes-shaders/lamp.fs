#version 330 core
in vec3 ourColor;

uniform vec3 LampColor;

out vec4 color;

void main()
{
    color = vec4(LampColor, 1.0f);
}

