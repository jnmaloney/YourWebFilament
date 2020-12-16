#define USE_GL 0

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>

#include "myengine_setup.h"
#include "filament_setup.h"

#include "ResourceManager.h"


// Some javascript is used to set the current GL context
void register_fila_context()
{
  EM_ASM_INT({
    const handle = GL.registerContext(myengine.glContext, myengine.glOptions);
    GL.makeContextCurrent(handle);
  });
}


ResourceManager g_rm;
bool g_mesh_loaded = false;
void on_load_final(ResourceManager& rm);


/* -------------------------------------------- */

void loop()
{
  loop_myengine();
  loop_filament();

  // Final step to async loading  
  if (g_mesh_loaded == false && g_rm.pending == 0)
  {
    g_mesh_loaded = true;
    on_load_final(g_rm);
  }
}


/* -------------------------------------------- */
EM_BOOL em_mouse_callback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
  static bool down = false;
  static int x = 0;
  static int y = 0;

  if (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN)
  {
    down = true;
    x = mouseEvent->clientX;
    y = mouseEvent->clientY;
  }

  if (eventType == EMSCRIPTEN_EVENT_MOUSEUP)
  {
    down = false;
  }

  if (eventType == EMSCRIPTEN_EVENT_MOUSEMOVE)
  {
    if (down)
    {
      int dx = x - mouseEvent->clientX;
      int dy = y - mouseEvent->clientY;
      x = mouseEvent->clientX;
      y = mouseEvent->clientY;
      myengine_mouse_event(dx, dy);
    }
  }

  return 0;
}


int main(int argc, char* argv[]) {

  enum 
  {
    FILAMESH = 0,
    FILAMAT,
    TEXTURE_KTX,
  };
  std::function x = &on_mesh_load;
  std::function y = &on_material_load;
  std::function z = &on_ktx_load;
  g_rm.addResourceType(FILAMESH, &x);
  g_rm.addResourceType(FILAMAT, &y);
  g_rm.addResourceType(TEXTURE_KTX, &z);

  g_rm.addResource("sky",               "default_env/default_env_skybox.ktx",   TEXTURE_KTX);
  g_rm.addResource("ibl",               "default_env/default_env_ibl.ktx",      TEXTURE_KTX);
  g_rm.addResource("Heart ao",          "model/ao_etc.ktx",                     TEXTURE_KTX);
  g_rm.addResource("Default material",  "materials/plastic.filamat",          FILAMAT);
  g_rm.addResource("Heart mesh",        "model/heart.filamesh",                 FILAMESH);

  init_myengine();
  register_fila_context();
  init_filament();

  emscripten_set_mousedown_callback("canvas", nullptr, true, em_mouse_callback);
  emscripten_set_mouseup_callback("canvas", nullptr, true, em_mouse_callback);
  emscripten_set_mousemove_callback("canvas", nullptr, true, em_mouse_callback);

  g_rm.start_async_load();  
   
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(loop, 0, 1);
#endif

  stop_filament();
  stop_myengine();

  return EXIT_SUCCESS;
}



/* -------------------------------------------- */

