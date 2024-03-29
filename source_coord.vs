#version 330 core
layout(location=0) in vec3 pos;
layout(location=1) in vec3 color;

uniform mat4 viewpoint;
uniform mat4 rotate;
uniform mat4 projection;

out vec3 outColor;

void main(){
	gl_Position = projection * viewpoint * rotate * vec4(pos.x, -pos.y, pos.z, 1.0f) - vec4(0.4f, 0.4f, 0.1f, 0.0f);
	outColor = color;
}