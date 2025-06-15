// Metadata - Version of GLSL 4.5
#version 450            

// Output color for Vertew (location is required)
layout(location = 0) out vec3 fragColor;

//  Triangle vertex positions (Will put in to vertew buffer later!)
vec3 positions[3] = vec3[](
    vec3(0.0, -0.4, 0.0),
    vec3(0.4, 0.4, 0.0),
    vec3(-0.4, 0.4, 0.0)
);

// All the colors for each point in sRGB
vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),    // RED
    vec3(0.0, 1.0, 0.0),    // GREEN
    vec3(0.0, 0.0, 1.0)     // BLUE
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 1.0);
    fragColor = colors[gl_VertexIndex];
}