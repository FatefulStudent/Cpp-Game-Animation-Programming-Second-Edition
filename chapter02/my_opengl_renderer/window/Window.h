#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../opengl_renderer/OGLRenderer.h"
#include "../opengl_renderer/Model.h"

enum class KeyboardKeyEnum : uint8_t
{
  KEY_UNDEFINED,
  KEY_W,
  KEY_A,
  KEY_S,
  KEY_D,
};

enum class MouseKeyEnum : uint8_t
{
  KEY_UNDEFINED,
  KEY_LMB,
  KEY_RMB,
  KEY_MMB,
};

enum class ButtonStatus : uint8_t
{
  Undefined,
  Pressed,
  Released
};

class Window {
public:
  bool init(unsigned int width, unsigned int height, std::string title);
  void mainLoop();
  void cleanup();

private:
  GLFWwindow *mWindow = nullptr;
  std::string mInitialTitle;

  std::unordered_map<KeyboardKeyEnum, ButtonStatus> mCurrentKeyStatusMap;
  std::unordered_map<MouseKeyEnum, ButtonStatus> mCurrentMouseKeyStatusMap;
  
  bool mouseHidden = false;

  std::unique_ptr<OGLRenderer> mRenderer;
  std::unique_ptr<Model> mModel;
  
  void handleWindowMoveEvents(int xpos, int ypos);
  void handleWindowMinimizedEvents(int minimized);
  void handleWindowMaximizedEvents(int maximized);
  void handleWindowResizedEvents(int width, int height);
  void handleWindowCloseEvents();

  void handleKeyEvents(int key, int scancode, int action, int mods);
  void handleMouseButtonEvents(int button, int action, int mods);
  void handleMousePositionEvents(double xpos, double ypos);
  void handleMouseEnterLeaveEvents(int enter);

  void handleKeyPressed(KeyboardKeyEnum Key);
  void handleKeyReleased(KeyboardKeyEnum Key);
  void handleKeyChangedState(KeyboardKeyEnum Key, ButtonStatus NewStatus);

  void handleMouseKeyPressed(MouseKeyEnum Key);
  void handleMouseKeyReleased(MouseKeyEnum Key);
  void handleMouseKeyChangedState(MouseKeyEnum Key, ButtonStatus NewStatus);

  void hideTheMouseCursor();
  void unHideTheMouseCursor();
};
