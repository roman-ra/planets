#version 330 core

// Flag definitions
#define HAS_DIFFUSE_MAP   0x1
#define HAS_ROUGHNESS_MAP 0x2
#define HAS_NORMAL_MAP    0x4
#define HAS_METALNESS_MAP 0x8
#define HAS_EMISSION_MAP  0x16


in vec3 WorldSpacePosition;
in vec3 EyeDirection;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 cameraWorldPosition;


uniform int materialFlags;
uniform vec3 diffuseColor;

/* Texture maps */
uniform sampler2D diffuseMap;
uniform sampler2D roughnessMap;
uniform sampler2D normalMap;
uniform sampler2D metalnessMap;
uniform sampler2D emissionMap;
uniform sampler2D aoMap;


vec3 getDiffuseColor(vec2 texCoord, out float alpha)
{
  if (bool(materialFlags & HAS_DIFFUSE_MAP)) {
    vec4 sample = texture2D(diffuseMap, TexCoord);
    alpha = sample.a;
    return sample.rgb * diffuseColor;
  } else {
    alpha = 1.0;
    return diffuseColor;
  }
}

vec3 getNormal(vec2 texCoord)
{
  if (bool(materialFlags & HAS_NORMAL_MAP)) {
    //
    vec3 tangentSpace = (texture2D(normalMap, TexCoord).rgb - 0.5) * 2;
    return normalize(mat3(Tangent, Bitangent, Normal) * tangentSpace);
  } else {
    return Normal;
  }
}


void main()
{
  const float ambient = 0.1;
  vec3 d = WorldSpacePosition - cameraWorldPosition;

  vec3 normal = getNormal(TexCoord);
  float lightIntensity = min(pow(1.0 / length(d), 2.0) * 5.0 * dot(normalize(-d), normal) + ambient, 1.0);

  float alpha;
  vec3 surfaceColor = getDiffuseColor(TexCoord, alpha); 

  if (alpha < 0.5) {
    discard;
  }

  float rim = pow(1.0 - dot(-EyeDirection, normal), 2.0);
  
  FragColor = vec4(surfaceColor * lightIntensity, 1.0);
}
