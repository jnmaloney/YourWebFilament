#include "ResourceManager.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <stdio.h>


int ResourceManager::pending = 0;


void ResourceManager::f_onload(void* arg, void* buffer, int size)
{
  --pending;
  Resource* r = (Resource*)arg;
  printf("Resource type %i loaded. Processing...\n", r->resource_type);
  r->loading_status = 50;
  r->handle = (*r->f)(buffer, size);
  r->loading_status = 100;
}


void ResourceManager::f_onerr(void* arg)
{
  --pending;
  Resource* r = (Resource*)arg;

  printf("Resource %s could not be loaded.\n", r->source_path.c_str());
}


ResourceManager::ResourceManager()
{

}


ResourceManager::~ResourceManager()
{

}


void ResourceManager::addResourceType(RESOURCE_TYPE type_id, callback_func* callback)
{
  printf("adding resource type %i\n", type_id);
  printf("callback address %i\n", (int)callback);
  //s_callbacks[type_id] = callback;
  s_callbacks.insert(std::pair<RESOURCE_TYPE, callback_func*>(type_id, callback));
}


void ResourceManager::addResource(const char* resource_name, const char* file_path, RESOURCE_TYPE resource_type)
{
  // Check if exitsts
  if (m_resources.count(resource_name) > 0) 
  {
    printf("A resource with that name already exists: %s\n", resource_name);
    return;
  }

  // Create an empty, unloaded resource
  ResourceManager::Resource new_resource;
  new_resource.resource_type = resource_type;
  new_resource.source_path = file_path;
  new_resource.f = s_callbacks[resource_type]; // !
  m_resources[resource_name] = new_resource;
}


void ResourceManager::start_async_load()
{
  printf("Begin async resource loading.\n");

  for (const auto& i: m_resources)
  {
    ++pending;

    //const Resource& r = i.second;
    const Resource* r = &m_resources[i.first];

    // Set up the async load for each resource
    emscripten_async_wget_data(
      r->source_path.c_str(),
      (void**)r,
      &f_onload,
      &f_onerr
    );
  }

  printf("End async resource loading.\n");
}


bool ResourceManager::getResource(const char* resource_name, unsigned int& resource_handle)
{
  // //printf("getting \"%s\" (%i)\n", resource_name, m_resources[resource_name].loading_status);
  // auto i = m_resources.find(resource_name);
  // if (i == m_resources.end()) return false;

  // resource_handle = i->second.handle;

  // return true;
  return false;
}


// bool ResourceManager::getResource(const char* resource_name, void*& resource_handle)
// {
//   resource_handle = (void*)m_resources[resource_name].handle;

//   return false;
// }


bool ResourceManager::getResource(const char* resource_name, void** resource_handle)
{
  // //printf("getting \"%s\" (%i)\n", resource_name, m_resources[resource_name].loading_status);
  // auto i = m_resources.find(resource_name);
  // if (i == m_resources.end()) return false;

  // if (i->second.loading_status == 100)
  //   *resource_handle = (void*)i->second.handle;

  // return true;
  return false;
}


void* ResourceManager::getResource(const char* resource_name)
{
  auto i = m_resources.find(resource_name);
  if (i == m_resources.end()) return nullptr;

  if (i->second.loading_status == 100)
    return (void*)i->second.handle;

  return (void*)1;
}
