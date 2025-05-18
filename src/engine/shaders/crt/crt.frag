#version 120

varying vec2 v_tex_coord;
uniform sampler2D screen_texture;
uniform float time;

// CRT effect parameters
const float scanline_intensity = 0.2;
const float scanline_count = 250.0;
const float vignette_strength = 0.3;
const float distortion = 0.1;
const float rgb_offset = 0.003;
const float flicker_intensity = 0.05;
const float noise = 0.05;

// Helper functions
float random(vec2 pos) {
    return fract(sin(dot(pos.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 curve_remap_UV(vec2 uv) {
    // Curve effect - bulge in the middle
    uv = uv * 2.0 - 1.0;
    vec2 offset = abs(uv.yx) / vec2(5.0, 5.0);
    uv = uv + uv * offset * offset;
    uv = uv * 0.5 + 0.5;
    return uv;
}

void main() {
    // Apply screen distortion
    vec2 uv = curve_remap_UV(v_tex_coord);
    
    // Check if we're off the screen after distortion
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }
    
    // Apply scanlines
    float scanline = sin(uv.y * scanline_count * 3.14159) * 0.5 + 0.5;
    scanline = pow(scanline, 1.5);
    scanline = (1.0 - scanline_intensity) + scanline_intensity * scanline;
    
    // Add color bleeding (RGB offset)
    vec3 col;
    col.r = texture2D(screen_texture, vec2(uv.x + rgb_offset, uv.y)).r;
    col.g = texture2D(screen_texture, uv).g;
    col.b = texture2D(screen_texture, vec2(uv.x - rgb_offset, uv.y)).b;
    
    // Apply vignette effect
    float vignette = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);
    vignette = clamp(pow(16.0 * vignette, vignette_strength), 0.0, 1.0);
    col *= vignette;
    
    // Apply scanlines
    col *= scanline;
    
    // Add some grain/noise
    float noise_val = random(uv + mod(time, 100.0));
    col += noise * (noise_val - 0.5);
    
    // Flicker effect
    float flicker = random(vec2(mod(time, 100.0), 0.0));
    col *= 1.0 - flicker_intensity + flicker_intensity * flicker;
    
    // Final color
    gl_FragColor = vec4(col, 1.0);
}