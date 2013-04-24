#version 100

uniform mat4 mToV;
uniform mat4 vToP;
attribute vec3 pos;
attribute vec3 normal;
attribute vec2 uv;
attribute vec3 color;

varying vec3 outNormal;
varying vec2 outUV;
varying vec3 outColor;
varying vec3 outPos;

void main() { 
	outNormal = normal;
	outUV = uv;
	outColor = color;
	outPos = pos;
	gl_Position = vToP * mToV *vec4(pos,1);
}