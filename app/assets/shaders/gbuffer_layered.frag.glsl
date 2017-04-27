// Copyright 2016 Benjamin Glatzel
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : enable

#include "lib_math.glsl"
#include "surface_fragment.inc.glsl"

// Ubos
PER_MATERIAL_UBO();
PER_INSTANCE_UBO();

// Bindings
BINDINGS_LAYERED();

// Input
layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inTangent;
layout (location = 2) in vec3 inBinormal;
layout (location = 3) in vec3 inColor;
layout (location = 4) in vec2 inUV0;

// Output
OUTPUT

void main()
{ 
  const mat3 TBN = mat3(inTangent, inBinormal, inNormal);

  const vec2 uv0 = UV0_TRANSFORMED_ANIMATED;
  const vec2 uv0Raw = UV0;

  const vec4 albedo0 = texture(albedoTex0, uv0);
  const vec4 normal0 = texture(normalTex0, uv0);
  const vec4 roughness0 = texture(roughnessTex0, uv0);

  const vec4 albedo1 = texture(albedoTex1, uv0);
  const vec4 normal1 = texture(normalTex1, uv0);
  const vec4 roughness1 = texture(roughnessTex1, uv0);
  
  const vec4 albedo2 = texture(albedoTex2, uv0);
  const vec4 normal2 = texture(normalTex2, uv0);
  const vec4 roughness2 = texture(roughnessTex2, uv0);

  vec4 blendMask = texture(blendMaskTex, uv0Raw);

  vec3 albedo = min(albedo0.rgb * blendMask.r + albedo1.rgb * blendMask.g + albedo2.rgb * blendMask.b, vec3(1.0));
  vec3 normal = min(normal0.rgb * blendMask.r + normal1.rgb * blendMask.g + normal2.rgb * blendMask.b, vec3(1.0));
  vec3 roughness = min(roughness0.rgb * blendMask.r + roughness1.rgb * blendMask.g + roughness2.rgb * blendMask.b, vec3(1.0));

  outAlbedo = vec4(albedo.rgb * uboPerInstance.data0.x, 1.0); // Albedo
  outNormal.rg = encodeNormal(normalize(TBN * (normal.xyz * 2.0 - 1.0)));
  outNormal.b = roughness.g + uboPerMaterial.pbrBias.g; // Specular
  outNormal.a = max(roughness.b + uboPerMaterial.pbrBias.b, 0.01); // Roughness
  outParameter0.rgba = vec4(roughness.r + uboPerMaterial.pbrBias.r, uboPerMaterial.data0.x, 0.0, 0.0); // Metal Mask / Material Buffer Index;
}
