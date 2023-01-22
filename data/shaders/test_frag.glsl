#version 330 core

in vec3 WorldSpacePosition;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D diffuseTexture;


float checkerboard(vec2 tex_coords, float scale) {
  float s = tex_coords[0];
  float t = tex_coords[1];

  float sum = floor(s * scale) + floor(t * scale);
  bool isEven = mod(sum,2.0)==0.0;
  float percent = (isEven) ? 1.0 : 0.0;

  return percent;
}

void main()
{
    float ambient = 0.2;
    const vec3 lightDir = normalize(vec3(0.2, 1.5, 0.3));
    float lightIntensity = min(max(dot(Normal, lightDir), 0.0) + ambient, 1.0);
    //vec3 surfaceColor = texture2D(diffuseTexture, TexCoord).xyz; //vec3(0.5, 0.5, 1.0) * min(checkerboard(TexCoord, 30.0) + 0.85, 1.0);
    vec3 surfaceColor = vec3(0.5, 0.5, 1.0) * min(checkerboard(TexCoord, 30.0) + 0.85, 1.0);
    FragColor = vec4(surfaceColor * lightIntensity, 1.0);
}