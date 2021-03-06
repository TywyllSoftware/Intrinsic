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

#pragma once

namespace Intrinsic
{
namespace Renderer
{
namespace RenderPass
{
struct GenericMesh : Base
{
  void init(const rapidjson::Value& p_RenderPassDesc);
  void destroy();

  void render(float p_DeltaT, Components::CameraRef p_CameraRef);

private:
  Resources::PipelineRef _pipelineRef;
  Resources::PipelineLayoutRef _pipelineLayoutRef;
  Resources::DrawCallRef _drawCallRef;

  _INTR_ARRAY(_INTR_STRING) _materialPassNames;
  _INTR_ARRAY(uint8_t) _materialPassIds;
  RenderOrder::Enum _renderOrder;
};
}
}
}
