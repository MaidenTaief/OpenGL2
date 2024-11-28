// rainFrag.glsl

#version 330 core

// Incoming data from vertex shader
in float particleTime;

// Output fragment color
out vec4 FragColor;

// Constants for rain effect
const float PI = 3.14159265;

// Function to generate random value based on position
float rand(float seed)
{
    return fract(sin(seed) * 43758.5453);
}

void main()
{
    // Number of raindrops
    float numDrops = 100.0; // Reduced for performance

    // Initialize color
    vec3 color = vec3(0.0);

    // Loop through raindrops
    for (float i = 0.0; i < numDrops; i++) {
        // Generate a random position for each raindrop
        float x = rand(i) * 2.0 - 1.0;
        float y = fract(rand(i + 100.0) + particleTime * 0.5);

        // Calculate distance from current pixel to raindrop
        float dist = length(gl_FragCoord.xy / 800.0 * 2.0 - vec2(x, y)); // Assuming window width 800

        // Accumulate color based on distance (raindrops appear as streaks)
        color += vec3(0.0, 0.0, 1.0) * smoothstep(0.02, 0.0, dist);
    }

    // Final color output with alpha blending
    FragColor = vec4(color, 0.5); // Semi-transparent rain
}
