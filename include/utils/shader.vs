#version 400 core
layout (location = 0) in vec3 aPos;
out vec4 vertColor;
void main(){
	gl_Position = vec4(aPos, 1.0);
	vertColor = vec4(1.0, 1.0, 0.0, 1.0);
}
