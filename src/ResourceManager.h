#pragma once


// #include "Singleton.h"
#include <map>
#include <string>
#include <functional>


class Mesh;
void inspect(Mesh* mesh);


// Async resource file loader
class ResourceManager 
{
public:
  typedef unsigned int RESOURCE_TYPE;
  typedef std::function<void* (void*, unsigned int)>  callback_func;
  
  ResourceManager();
  ~ResourceManager();

  void addResourceType(RESOURCE_TYPE type_id, callback_func* callback);
  void addResource(const char* resource_name, const char* file_path, RESOURCE_TYPE resource_type);

  void start_async_load();

  bool getResource(const char* resource_name, unsigned int& resource_handle);
  //bool getResource(const char* resource_name, void*& resource_handle);
  bool getResource(const char* resource_name, void** resource_handle);
  void* getResource(const char* resource_name);
  
  //void unloadResource(..);
  std::map<RESOURCE_TYPE, callback_func*> s_callbacks;
  //std::map<RESOURCE_TYPE, void*> s_callbacks;

  static int pending;

protected:

  struct Resource
  {
    std::string source_path;
    //std::string name;
    RESOURCE_TYPE resource_type;
    int loading_status = 0;
    void* handle = nullptr;
    //ResourceManager* rm;
    callback_func* f = nullptr;
  };

  std::map<std::string, Resource> m_resources;

  static void f_onload(void* arg, void* buffer, int size);
  static void f_onerr(void* arg);

};
