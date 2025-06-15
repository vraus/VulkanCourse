// Metadata - Version of GLSL 4.5
#version 450

// Interpolated color from Vertex Shader (location must match)
layout(location = 0) in vec3 fragColor;

// Final output color. Must also have a location.
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}