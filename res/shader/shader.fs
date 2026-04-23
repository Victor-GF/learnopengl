#version 330 core
out vec4 FragColor;
in vec3 vertexColor;
in vec2 TexCoord;
uniform sampler2D uniformTexture1;
uniform sampler2D uniformTexture2;
uniform float mixInterpolate;
void main() {
    FragColor = mix(texture(uniformTexture1, TexCoord), texture(uniformTexture2, TexCoord), mixInterpolate);
}