#version 460 core
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D Tex;

void main() {
  FragColor = vec4(0.5, 0.5, 0.0, 1.0);
}
