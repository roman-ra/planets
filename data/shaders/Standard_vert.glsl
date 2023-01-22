#version 330 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_TexCoord;

out vec3 WorldSpacePosition;
out vec3 Normal;
out vec2 TexCoord;

uniform vec3 cameraWorldPosition;
uniform mat3 modelToWorldSpace_Normal;
uniform mat4 modelToClipSpace;
uniform mat4 modelToWorldSpace;
uniform float time;

void main()
{
    WorldSpacePosition = (modelToWorldSpace * vec4(in_Position, 1.0)).xyz;
 
    Normal = normalize((transpose(inverse(modelToWorldSpace)) * vec4(in_Normal, 0.0)).xyz);

    TexCoord = in_TexCoord;
    
    gl_Position = modelToClipSpace * vec4(in_Position, 1.0);
}