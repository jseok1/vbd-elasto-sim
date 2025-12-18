#pragma once

#include <memory>
#include <vector>

#include "camera/Camera.hpp"
#include "light/Light.hpp"
#include "model/Model.hpp"
#include "pipeline/RenderPipeline.hpp"

/**
 * App
 *   ComputePipeline[]  } calls OpenGL
 *   RenderPipeline[]  } calls OpenGL
 *
 *   Renderer (rendering passes, shaders, uniforms)
 *     FrameBuffer[]
 *     Texture[]  } calls OpenGL (makes sense out here, since textures aren't always for actors)
 *     Batcher:
 *       Mesh[]       (for batching, best to group here)
 *     Material[] -> reference texture and shader
 *           RenderPipeline (reference)
 *           Texture (reference)
 *
 *   Scene
 *     Camera (should be a component?)
 *       Transform
 *     Light[] (should be a component?)
 *       Transform
 *
 *     Actor[]   (drawn by Scene: Actor.RenderPipeline.use(); Actor.draw();)
 *               (instances as member variable)
 *       Transform
 *       uniforms
 *       Material (reference) don't make this a property of Mesh bc you can use the same material
 *       for different meshes.
 *       Mesh[]  } calls OpenGL    (drawn by Actor: Mesh.Material.Texture.use(); Mesh.draw();)
 *         Vertex[]
 *
 *  where should uniforms go? what other info does a Material carry besides shader choice?
 *
 *           vertex: xyz, normal, rgb, uv0-7
 *           triangle:
 *
 *           Unity uses separate buffers for each attribute
 *
 * Options:
 * 1. Sort Actor[] by _PATH_ RenderPipeline and Mesh[] by _PATH_ Texture to reduce context changes.
 * 2. Use batching and put all uniforms into a UBO to avoid having to reassign uniforms between
 *    Actor instances.
 * 3. Use indirect rendering (store params to draw calls in GPU memory).
 *      Draw call buffer
 *
 * Eventually, add frustum/occlusion culling. Deferred shading. Bindless textures (preferred over
 * atlases to avoid complexity of generating atlas).
 *
 * Batch STATIC meshes together. Actually as long as the shape doesn't change, you can still batch.
 * It has to be static in Unity because Unity combines everything into a single mesh.
 *
 * As for different vertex data and shaders, think about these in terms of unique MATERIALS.
 *
 *
 *
 * indirect rendering: 64 MiB -> 4 KiB
 * put mesh data into buffers wherever there's space, then when drawing, only draw the ones that
 * are visible
 * put meshes that are likely to be drawn together in the same memory region
 *
 *
 * GPU culling seems to be a big strength of indirect rendering
 * indirect rendering is limited to the same shader, like batching
 *
 * bindless textures?
 *
 * is the batcher indirect rendering?
 *
 *
 *
 * Batching.
 * For each SHADER, put all vertices into a buffer.
 * Put uniforms in a UBO.
 * Textures?
 */

class Scene {
 public:
  std::unique_ptr<Camera> camera;
  std::vector<Light> lights;
  std::vector<Model> models;
  std::vector<RenderPipeline> renderPipelines;

  Scene() : camera{nullptr}, lights{}, models{}, renderPipelines{} {}

 private:
};
