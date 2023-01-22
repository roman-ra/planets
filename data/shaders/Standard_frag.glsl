#version 330 core

in vec3 WorldSpacePosition;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 cameraWorldPosition;
uniform sampler2D diffuseTexture;

void main()
{
    const float ambient = 0.1;

    float lightIntensity = min(pow(1.0 / distance(WorldSpacePosition, cameraWorldPosition), 2.0) * 5.0 + ambient, 1.0);

    vec4 surfaceColor = texture2D(diffuseTexture, TexCoord); 

    if (surfaceColor.a < 0.5) {
      discard;
    }
    FragColor = vec4(surfaceColor.rgb * lightIntensity, 1.0);
}
