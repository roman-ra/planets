#version 330 core

#define M_PI 3.1415926535897932384626433832795

// Flag definitions
#define HAS_DIFFUSE_MAP   (1 << 0)
#define HAS_ROUGHNESS_MAP (1 << 1)
#define HAS_NORMAL_MAP    (1 << 2)
#define HAS_METALNESS_MAP (1 << 3)
#define HAS_EMISSION_MAP  (1 << 4)
#define HAS_AO_MAP        (1 << 5)

// Lighting-related definitions
#define MAX_LIGHTS 128
#define LIGHT_POINTLIGHT  0
#define LIGHT_SPOTLIGHT   1
#define LIGHT_DIRECTIONAL 2


in vec3 WorldSpacePosition;
in vec3 EyeDirection;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec2 TexCoord;

out vec4 FragColor;
out vec4 Normal_Depth;

uniform vec3 cameraWorldPosition;
uniform vec3 cameraDirection;
uniform float time;

uniform int materialFlags;
uniform vec3 diffuseColor;
uniform float roughness;
uniform float metalness;
uniform vec3 emissionColor;

/* Texture maps */
uniform sampler2D diffuseMap;
uniform sampler2D roughnessMap;
uniform sampler2D normalMap;
uniform sampler2D metalnessMap;
uniform sampler2D emissionMap;
uniform sampler2D aoMap;

/* Ligths */
const int numLights = 4;
struct Light {
  int lightType;
  vec3 positionWorld; // Not relevant for directional lights
  vec4 direction_Angle; // Direction not relevant for point lights, angle for point lights
  vec4 color_Intensity; // .xyz color, .w intensity (scale) 
} lights[MAX_LIGHTS];


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
    vec3 tangentSpace = texture2D(normalMap, TexCoord).rgb * 2.0 - 1.0;
    return normalize(mat3(normalize(Tangent), normalize(Bitangent), normalize(Normal)) * tangentSpace);
  } else {
    return Normal;
  }
}

float getRoughness(vec2 texCoord)
{
  if (bool(materialFlags & HAS_ROUGHNESS_MAP)) {
    return texture2D(roughnessMap, TexCoord).r;
  } else {
    return roughness;
  }
}

float getMetalness(vec2 texCoord)
{
  if (bool(materialFlags & HAS_METALNESS_MAP)) {
    return texture2D(metalnessMap, TexCoord).r;
  } else {
    return metalness;
  }
}

vec3 getEmission(vec2 texCoord)
{
  if (bool(materialFlags & HAS_EMISSION_MAP)) {
    return texture2D(emissionMap, TexCoord).rgb;
  } else {
    return emissionColor;
  }
}

float getAo(vec2 texCoord)
{
  if (bool(materialFlags & HAS_AO_MAP)) {
    return texture2D(aoMap, TexCoord).r;
  } else {
    return 1.0; // No AO without AO map
  }
}


vec3 shade(Light light, 
          vec3 diffuse, 
          float roughness, 
          float metalness, 
          vec3 P, 
          vec3 N, 
          vec3 V)
{
  vec3 L = light.positionWorld - P; // direction to the light, not relevant for directional lights
  float L_len = length(L); // distance to the light, not relevant for directional lights
  L = L / L_len; // normalize it before using
  float NdotL = max(dot(N, L), 0);

  if (light.lightType == LIGHT_POINTLIGHT) {
    vec3 H = normalize(L + V);
    return diffuse 
            * NdotL
            * (1 / (L_len * L_len))
            * light.color_Intensity.rgb
            * light.color_Intensity.a;

  } else if (light.lightType == LIGHT_SPOTLIGHT) {
    vec3 H = normalize(L + V);
    
    return vec3(0.0);

  } else if (light.lightType == LIGHT_DIRECTIONAL) {
    vec3 H = normalize(-light.direction_Angle.xyz + V);

    return vec3(0.0);

  } else {
    return vec3(0.0);
  }
}



// Filmic Tonemapping Operators http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 filmic(vec3 x) {
  vec3 X = max(vec3(0.0), x - 0.004);
  vec3 result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
  return pow(result, vec3(2.2));
}

float filmic(float x) {
  float X = max(0.0, x - 0.004);
  float result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
  return pow(result, 2.2);
}



void main()
{
  const vec3 ambient = vec3(0.1);

  // This will be passed in as uniform data in the future
  lights[0].lightType = LIGHT_POINTLIGHT;
  lights[0].color_Intensity = vec4(vec3(1, 1, 1), 10.0);
  lights[0].positionWorld = vec3(0, 5, 0);

  lights[1].lightType = LIGHT_POINTLIGHT;
  lights[1].color_Intensity = vec4(vec3(1, 0.7, 0.5), 10.0);
  lights[1].positionWorld = vec3(sin(time) * 5, 6, 5);

  lights[2].lightType = LIGHT_POINTLIGHT;
  lights[2].color_Intensity = vec4(vec3(1, 0.7, 0.5), 10.0);
  lights[2].positionWorld = vec3(sin(time + M_PI/2) * 5, 6, -4.5);

  lights[3].lightType = LIGHT_POINTLIGHT;
  lights[3].color_Intensity = vec4(vec3(1, 1, 1), 5.0);
  lights[3].positionWorld = cameraWorldPosition;

  float alpha = 1.0;
  vec3 diffuse = getDiffuseColor(TexCoord, alpha);

  if (alpha < 0.5) {
    discard;
  }

  vec3 N = getNormal(TexCoord);
  vec3 V = normalize(-EyeDirection);
  vec3 P = WorldSpacePosition;
  

  float roughness = getRoughness(TexCoord);
  float metalness = getMetalness(TexCoord);

  //vec3 total = diffuse * (ambient * getAo(TexCoord)); // Add ambient term
  vec3 total = vec3(0.0);

  for (int  i = 0; i < numLights; i++)
  {
    total += shade(lights[i], diffuse, roughness, metalness, P, N, V);
  }

  total += getEmission(TexCoord);

  // TODO: tonemapping should be done on the entire color buffer and not here
  FragColor = vec4(filmic(total), 1.0);
  Normal_Depth = vec4(N, distance(WorldSpacePosition, cameraWorldPosition));
}
