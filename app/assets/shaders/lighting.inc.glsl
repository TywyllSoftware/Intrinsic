// Copyright 2017 Benjamin Glatzel
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

void calcTransl(
  in LightingData d, 
  in MaterialParameters matParams, 
  in float att, 
  in vec4 lightColorAndIntensity, 
  inout vec4 outColor)
{
  const float localTranslThickness = matParams.translucencyThickness;
  const vec3 translLightVector = d.L + d.N * translDistortion;
  const float translDot = exp2(clamp(dot(d.V, -translLightVector), 0.0, 1.0) 
    * translPower - translPower) * translScale;
  const vec3 transl = (translDot + translAmbient) * localTranslThickness;
  const vec3 translColor = att * lightColorAndIntensity.w 
    * lightColorAndIntensity.rgb * transl * d.diffuseColor;

  outColor.rgb += translColor;
}

void calcPointLightLighting(
  in Light light, 
  in LightingData d, 
  in MaterialParameters matParams, 
  inout vec4 outColor)
{
  const vec3 lightDistVec = light.posAndRadius.xyz - d.posVS;
  const float dist = length(lightDistVec);
  const float att = calcInverseSqrFalloff(light.posAndRadius.w, dist);

  d.L = lightDistVec / dist;
  d.energy = vec3(light.colorAndIntensity.a);
  calculateLightingData(d);

  const vec3 lightColor = light.colorAndIntensity.rgb 
    * kelvinToRGB(light.temp.r, kelvinLutTex);

  outColor.rgb += calcLighting(d) * att * lightColor;
  calcTransl(d, matParams, att, 
    vec4(lightColor, light.colorAndIntensity.w), outColor);
}
