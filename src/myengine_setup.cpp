#include "myengine_setup.h"
#include "global_helpers.h"
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
//#include <glad/glad.h>
#include <GLFW/glfw3.h>


uint32_t win_w = 800;
uint32_t win_h = 600;
GLFWwindow* g_window = NULL;
const char* g_windowTitle = "My Render Engine";


unsigned int get_w() { return win_w; }
unsigned int get_h() { return win_h; }


void init_myengine()
{
  if(!glfwInit()) 
  {
    printf("Error: cannot setup glfw.\n");
    return;
  }

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#if !USE_GL
  printf("Not using a GL context.\n");
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif  
       
  g_window = glfwCreateWindow(win_w, win_h, g_windowTitle, NULL, NULL);
  if(!g_window) 
  {
    printf("failed to create window\n");
    glfwTerminate();
    return;
  }
}


void stop_myengine()
{
  glfwTerminate();
}


void loop_myengine()
{
}


void myengine_mouse_event(int x, int y)
{
  static float s_a = 0;
  static float s_b = 0;
  s_a +=  0.002f * (float)x;
  s_b += -0.002f * (float)y;
  if (s_b > 1.0f) s_b = 1.0f;
  if (s_b < -1.0f) s_b = -1.0f;
  set_camera_angle(s_a, s_b);
}