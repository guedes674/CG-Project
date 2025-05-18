// Outline detection fragment shader
#version 120
#extension GL_EXT_gpu_shader4 : enable

varying vec2 texCoord;

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;
uniform float outlineWidth = 1.0;
uniform vec3 outlineColor = vec3(1.0, 1.0, 1.0); // White

void main() {
    // Get screen texture size for proper pixel sampling
    vec2 texSize = 1.0 / vec2(textureSize2D(depthTexture, 0));
    
    // Sample depth at current position
    float depth = texture2D(depthTexture, texCoord).r;
    
    // Sample neighboring pixels
    float neighborDepths[4];
    neighborDepths[0] = texture2D(depthTexture, texCoord + vec2(texSize.x * outlineWidth, 0.0)).r;
    neighborDepths[1] = texture2D(depthTexture, texCoord - vec2(texSize.x * outlineWidth, 0.0)).r;
    neighborDepths[2] = texture2D(depthTexture, texCoord + vec2(0.0, texSize.y * outlineWidth)).r;
    neighborDepths[3] = texture2D(depthTexture, texCoord - vec2(0.0, texSize.y * outlineWidth)).r;
    
    // Calculate maximum depth difference
    float maxDepthDiff = 0.0;
    for (int i = 0; i < 4; i++) {
        float diff = abs(depth - neighborDepths[i]);
        maxDepthDiff = max(maxDepthDiff, diff);
    }
    
    // Apply threshold to determine edges
    float edgeThreshold = 0.0001;
    float isEdge = (maxDepthDiff > edgeThreshold) ? 1.0 : 0.0;
    
    // Mix original color with outline color
    vec4 originalColor = texture2D(colorTexture, texCoord);
    gl_FragColor = mix(originalColor, vec4(outlineColor, 1.0), isEdge);
}