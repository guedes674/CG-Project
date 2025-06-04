// Outline detection fragment shader
#version 120
#extension GL_EXT_gpu_shader4 : enable

varying vec2 tex_coord;

uniform sampler2D color_texture;
uniform sampler2D depth_texture;
uniform float outline_width = 1.0;
uniform vec3 outline_color = vec3(1.0, 1.0, 1.0); // White

void main() {
    // Get screen texture size for proper pixel sampling
    vec2 tex_size = 1.0 / vec2(textureSize2D(depth_texture, 0));
    
    // Sample depth at current position
    float depth = texture2D(depth_texture, tex_coord).r;
    
    // Sample neighboring pixels
    float neighborDepths[4];
    neighborDepths[0] = texture2D(depth_texture, tex_coord + vec2(tex_size.x * outline_width, 0.0)).r;
    neighborDepths[1] = texture2D(depth_texture, tex_coord - vec2(tex_size.x * outline_width, 0.0)).r;
    neighborDepths[2] = texture2D(depth_texture, tex_coord + vec2(0.0, tex_size.y * outline_width)).r;
    neighborDepths[3] = texture2D(depth_texture, tex_coord - vec2(0.0, tex_size.y * outline_width)).r;
    
    // Calculate maximum depth difference
    float max_depth_diff = 0.0;
    for (int i = 0; i < 4; i++) {
        float diff = abs(depth - neighborDepths[i]);
        max_depth_diff = max(max_depth_diff, diff);
    }
    
    // Apply threshold to determine edges
    float edge_threshold = 0.0001;
    float is_edge = (max_depth_diff > edge_threshold) ? 1.0 : 0.0;
    
    // Mix original color with outline color
    vec4 originalColor = texture2D(color_texture, tex_coord);
    gl_FragColor = mix(originalColor, vec4(outline_color, 1.0), is_edge);
}