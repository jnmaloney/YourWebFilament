#include "filament_setup.h"
#include "global_helpers.h"
#include <stdio.h>
#include <filament/FilamentAPI.h>
#include <filament/Engine.h>
#include <filament/Renderer.h>
#include <filament/View.h>
#include <utils/EntityManager.h>
#include <filament/RenderableManager.h>
#include <filament/Scene.h>
#include <filament/Material.h>
#include <filament/VertexBuffer.h>
#include <filament/IndexBuffer.h>
#include <filament/Skybox.h>
#include <filament/Viewport.h>
#include <filament/SwapChain.h>
#include <filament/Camera.h>
#include <filament/TransformManager.h>
#include <filameshio/MeshReader.h>
#include <math/mat3.h>
#include <math/mat4.h>
#include <utils/Path.h>
#include <filament/LightManager.h>
#include <filament/TextureSampler.h>
#include <filament/IndirectLight.h>
#include <image/KtxBundle.h>
#include <image/KtxUtility.h>
#include "ResourceManager.h"


// Filement core objects
filament::Engine* fila_engine;
filament::SwapChain* fila_swap_chain;
filament::View* fila_view;
filament::Renderer* fila_renderer;
utils::Entity fila_cam_entity;
filament::Camera* fila_cam;
filament::Scene* fila_scene;


void set_camera_angle(float a, float b)
{
  float r = 8.f;
  float x = r * sinf(a) * cosf(b);
  float y = r * sinf(b);
  float z = r * cosf(a) * cosf(b);
  fila_cam->lookAt({x, y, z}, {0, 0, 0}, {0, 1, 0});
}


void init_filament()
{
  // Create engine (Set current GL context beforehand)
  fila_engine = filament::Engine::create();
  if (fila_engine == nullptr) 
  {
    printf("Failled to create the filament engine.\n");
    return;
  }

  // Swap chain from context?
  fila_swap_chain = fila_engine->createSwapChain(nullptr);

  if (fila_swap_chain == nullptr) 
  {
    printf("Failed to create the SwapChain.\n");
    return;
  }

  // Other objects...

  fila_renderer = fila_engine->createRenderer();
  fila_scene = fila_engine->createScene();
  fila_view = fila_engine->createView();
  fila_cam_entity = utils::EntityManager::get().create();
  fila_cam = fila_engine->createCamera(fila_cam_entity);

  fila_cam->setExposure(16.0f, 1 / 125.0f, 100.0f);
  fila_cam->setExposure(100.0f);
  fila_cam->setProjection(45.0f, float(get_w())/get_h(), 0.1f, 100.0f);
  set_camera_angle(0.f, 0.f);
  
  printf("Creating viewport %ix%i\n", get_w(), get_h());

  fila_view->setViewport({0, 0, get_w(), get_h()});
  fila_view->setScene(fila_scene);
  fila_view->setCamera(fila_cam); 
  fila_view->setName("main-view");

  fila_renderer->setClearOptions({
    .clearColor = { 0.0f, 0.13f, 0.0f, 1.0f },
    .clear = true
  });
}


void stop_filament()
{
}


void loop_filament()
{
  if (fila_renderer->beginFrame(fila_swap_chain)) 
  {
    fila_renderer->render(fila_view);
    fila_renderer->endFrame();
  }

  fila_engine->execute();
}


class FilaMeshBuffer
{
public:
  FilaMeshBuffer(void* data, unsigned int data_size);
  void* data;
  unsigned int data_size;
};
FilaMeshBuffer::FilaMeshBuffer(void* a_data, unsigned int a_data_size)
{
  data_size = a_data_size;
  data = malloc(a_data_size);
  memcpy(data, a_data, a_data_size);
}



void* on_mesh_load(void* data, unsigned int data_size)
{
  FilaMeshBuffer* mesh = new FilaMeshBuffer(data, data_size);

  return (void*)mesh;
}


void* on_material_load(void* buffer, unsigned int buffer_size)
{
  filament::Material* material = filament::Material::Builder()
     .package(buffer, buffer_size)
     .build(*fila_engine);

  return (void*)material;
}


void* on_ktx_load(void* buffer, unsigned int buffer_size)
{
  image::KtxBundle* bundle = 
    new image::KtxBundle(
      (const uint8_t*)buffer, 
      buffer_size
    );

  filament::Texture* texture = image::ktx::createTexture(fila_engine, bundle, !bundle->isCubemap());
  return (void*)texture;
}


void on_load_final(ResourceManager& rm)
{
  // Material instance
  filament::Material* material = (filament::Material*)rm.getResource("Default material"); 
  filament::MaterialInstance* my_material = material->createInstance();

  my_material->setParameter("baseColor", filament::RgbType::sRGB, {0.8, 0.0, 0.0});
  my_material->setParameter("roughness", 0.2f);
  my_material->setParameter("clearCoat", 1.0f);
  my_material->setParameter("clearCoatRoughness", 0.3f);

  // Textures
  filament::TextureSampler sampler(
                  filament::TextureSampler::MinFilter::LINEAR_MIPMAP_LINEAR,
                  filament::TextureSampler::MagFilter::LINEAR
                );

  filament::Texture* texture_ao = (filament::Texture*)rm.getResource("Heart ao"); 

  my_material->setParameter("ao", texture_ao, sampler);

  // Mesh
  FilaMeshBuffer* buffer_resource = (FilaMeshBuffer*)rm.getResource("Heart mesh"); 

  filamesh::MeshReader::MaterialRegistry material_registry;
  material_registry.registerMaterialInstance(utils::CString("DefaultMaterial"), my_material);

  filamesh::MeshReader::Mesh mesh;
  mesh = filamesh::MeshReader::loadMeshFromBuffer(
      fila_engine,
      buffer_resource->data,
      [](void* buffer, size_t size, void* user) {},
      nullptr,
      material_registry
    );

  fila_scene->addEntity(mesh.renderable);

  // Sky / IBL
  filament::Texture* skytex = (filament::Texture*)rm.getResource("sky"); 
  filament::Texture* ibltex = (filament::Texture*)rm.getResource("ibl"); 

  filament::IndirectLight * ibl = filament::IndirectLight::Builder()
        .reflections(ibltex)
        .build(*fila_engine);

  fila_scene->setIndirectLight(ibl);
  ibl->setIntensity(21);
  
  filament::Skybox* sky = filament::Skybox::Builder().environment(skytex).build(*fila_engine);
  fila_scene->setSkybox(sky);
}

