#version 330

in vec4 vCol;
in vec2 TexCoord0;

out vec4 colour;

uniform sampler2D theTexture;

void main(){
	colour = texture(theTexture, TexCoord0);
}