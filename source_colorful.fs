#version 330 core

in vec3 outColor;

out vec4 fragcolor;

uniform vec4 ourColor;

void main(){
	fragcolor = ourColor;
}