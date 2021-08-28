#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;

out vec4 vCol;
out vec2 TexCoord0;
out vec3 Normal;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main() {
	gl_Position = projection * view * model * vec4(pos, 1.0f);
	vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);

	TexCoord0 = tex;
	Normal = mat3(transpose(inverse(model))) * norm;	// Normal needs to be moved relative to the rotation of the object
																										// mat3(model) leaves only the rotation of the model
																										// Inversion and transposition accounts for any transformation and scaling that could change the normal relative to the vertex
}
