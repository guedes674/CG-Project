#version 120

varying vec2 v_tex_coord;

void main() {
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    v_tex_coord = gl_MultiTexCoord0.st;
}