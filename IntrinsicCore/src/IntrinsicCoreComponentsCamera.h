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
namespace Core
{
namespace Components
{
// Typedefs
typedef Dod::Ref CameraRef;
typedef _INTR_ARRAY(CameraRef) CameraRefArray;

/**
 * Stores all the relevant data for the Camera Component in a data
 * oriented fashion.
 */
struct CameraData : Dod::Components::ComponentDataBase
{
  CameraData()
      : Dod::Components::ComponentDataBase(_INTR_MAX_CAMERA_COMPONENT_COUNT)
  {
    descFov.resize(_INTR_MAX_CAMERA_COMPONENT_COUNT);
    descNearPlane.resize(_INTR_MAX_CAMERA_COMPONENT_COUNT);
    descFarPlane.resize(_INTR_MAX_CAMERA_COMPONENT_COUNT);

    frustum.resize(_INTR_MAX_CAMERA_COMPONENT_COUNT);

    forward.resize(_INTR_MAX_CAMERA_COMPONENT_COUNT);
    up.resize(_INTR_MAX_CAMERA_COMPONENT_COUNT);
  }

  // Description
  _INTR_ARRAY(float) descFov;
  _INTR_ARRAY(float) descNearPlane;
  _INTR_ARRAY(float) descFarPlane;

  // Resources
  _INTR_ARRAY(Resources::FrustumRef) frustum;
  _INTR_ARRAY(glm::vec3) forward;
  _INTR_ARRAY(glm::vec3) up;
};

/**
 * The manager for all Camera Components.
 */
struct CameraManager
    : Dod::Components::ComponentManagerBase<CameraData,
                                            _INTR_MAX_CAMERA_COMPONENT_COUNT>
{
  /**
   * Initializes the Camera Manager.
   */
  static void init();

  // <-

  /**
   * Requests a new reference for a Camera Component.
   */
  _INTR_INLINE static CameraRef createCamera(Entity::EntityRef p_ParentEntity)
  {
    CameraRef ref = Dod::Components::ComponentManagerBase<
        CameraData,
        _INTR_MAX_CAMERA_COMPONENT_COUNT>::_createComponent(p_ParentEntity);

    _frustum(ref) = Resources::FrustumManager::createFrustum(_N(CameraFrustum));

    return ref;
  }

  // <-

  /**
   * Resets the given Camera Component to the default values.
   */
  _INTR_INLINE static void resetToDefault(MeshRef p_Ref)
  {
    _descFov(p_Ref) = glm::radians(75.0f);
    _descNearPlane(p_Ref) = 1.0f;
    _descFarPlane(p_Ref) = 10000.0f;
  }

  // <-

  /**
   * Destroys the given Camera Component by putting the reference
   * back in to the pool.
   */
  _INTR_INLINE static void destroyCamera(CameraRef p_Camera)
  {
    Resources::FrustumManager::destroyFrustum(_frustum(p_Camera));
    _frustum(p_Camera) = Dod::Ref();

    Dod::Components::ComponentManagerBase<
        CameraData,
        _INTR_MAX_CAMERA_COMPONENT_COUNT>::_destroyComponent(p_Camera);
  }

  // <-

  /**
   * Compiles all exposed properties to a JSON descriptor.
   */
  _INTR_INLINE static void compileDescriptor(CameraRef p_Ref,
                                             bool p_GenerateDesc,
                                             rapidjson::Value& p_Properties,
                                             rapidjson::Document& p_Document)
  {
    p_Properties.AddMember(
        "fov",
        _INTR_CREATE_PROP(p_Document, p_GenerateDesc, _N(Camera), _N(float),
                          glm::degrees(_descFov(p_Ref)), false, false),
        p_Document.GetAllocator());
    p_Properties.AddMember(
        "nearPlane",
        _INTR_CREATE_PROP(p_Document, p_GenerateDesc, _N(Camera), _N(float),
                          _descNearPlane(p_Ref), false, false),
        p_Document.GetAllocator());
    p_Properties.AddMember(
        "farPlane",
        _INTR_CREATE_PROP(p_Document, p_GenerateDesc, _N(Camera), _N(float),
                          _descFarPlane(p_Ref), false, false),
        p_Document.GetAllocator());
  }

  // <-

  /**
   * Initializes all properties from a JSON descriptor.
   */
  _INTR_INLINE static void initFromDescriptor(CameraRef p_Ref,
                                              bool p_GenerateDesc,
                                              rapidjson::Value& p_Properties)
  {
    if (p_Properties.HasMember("fov"))
      _descFov(p_Ref) =
          glm::radians(JsonHelper::readPropertyFloat(p_Properties["fov"]));
    if (p_Properties.HasMember("nearPlane"))
      _descNearPlane(p_Ref) =
          JsonHelper::readPropertyFloat(p_Properties["nearPlane"]);
    if (p_Properties.HasMember("farPlane"))
      _descFarPlane(p_Ref) =
          JsonHelper::readPropertyFloat(p_Properties["farPlane"]);
  }

  // <-

  /**
   * Updates all frustums and matrices of the given Camera Components.
   */
  static void updateFrustumsAndMatrices(const CameraRefArray& p_Cameras);

  // <-

  /**
   * Computes a custom projection matrix the given Camera Component and
   * parameters.
   */
  static glm::mat4 computeCustomProjMatrix(CameraRef p_Ref, float p_Near,
                                           float p_Far);

  // Description

  /**
   * The distance to the near plane (in world units).
   */
  _INTR_INLINE static float& _descNearPlane(CameraRef p_Ref)
  {
    return _data.descNearPlane[p_Ref._id];
  }

  /**
   * The distance to the far plane (in world units).
   */
  _INTR_INLINE static float& _descFarPlane(CameraRef p_Ref)
  {
    return _data.descFarPlane[p_Ref._id];
  }

  /**
   * The field of view (in radians).
   */
  _INTR_INLINE static float& _descFov(CameraRef p_Ref)
  {
    return _data.descFov[p_Ref._id];
  }

  // Resources

  /**
   * The frustum used internally by the component.
   */
  _INTR_INLINE static Resources::FrustumRef& _frustum(CameraRef p_Ref)
  {
    return _data.frustum[p_Ref._id];
  }

  /**
   * The view matrix.
   */
  _INTR_INLINE static glm::mat4x4& _viewMatrix(CameraRef p_Ref)
  {
    return Resources::FrustumManager::_descViewMatrix(_frustum(p_Ref));
  }

  /**
   * The view matrix of the previous frame.
   */
  _INTR_INLINE static glm::mat4x4& _prevViewMatrix(CameraRef p_Ref)
  {
    return Resources::FrustumManager::_descPrevViewMatrix(_frustum(p_Ref));
  }

  /**
   * The inverse of the view matrix.
   */
  _INTR_INLINE static glm::mat4x4& _inverseViewMatrix(CameraRef p_Ref)
  {
    return Resources::FrustumManager::_invViewMatrix(_frustum(p_Ref));
  }

  /**
   * The projection matrix.
   */
  _INTR_INLINE static glm::mat4x4& _projectionMatrix(CameraRef p_Ref)
  {
    return Resources::FrustumManager::_descProjectionMatrix(_frustum(p_Ref));
  }

  /**
   * The inverse of the projection matrix.
   */
  _INTR_INLINE static glm::mat4x4& _inverseProjectionMatrix(CameraRef p_Ref)
  {
    return Resources::FrustumManager::_invProjectionMatrix(_frustum(p_Ref));
  }

  /**
   * Concatenation of the view and projection matrix.
   */
  _INTR_INLINE static glm::mat4x4& _viewProjectionMatrix(CameraRef p_Ref)
  {
    return Resources::FrustumManager::_viewProjectionMatrix(_frustum(p_Ref));
  }

  /**
   * Inverse of the concatenation of the view and projection matrix.
   */
  _INTR_INLINE static glm::mat4x4& _inverseViewProjectionMatrix(CameraRef p_Ref)
  {
    return Resources::FrustumManager::_invViewProjectionMatrix(_frustum(p_Ref));
  }

  /**
   * The forward vector of the camera.
   */
  _INTR_INLINE static glm::vec3& _forward(CameraRef p_Ref)
  {
    return _data.forward[p_Ref._id];
  }

  /**
   * The up vector of the camera.
   */
  _INTR_INLINE static glm::vec3& _up(CameraRef p_Ref)
  {
    return _data.up[p_Ref._id];
  }
};
}
}
}
