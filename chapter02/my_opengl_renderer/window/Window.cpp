#include "Window.h"
#include "Logger.h"

#include <string>
#include <unordered_map>

/* Tasks:
* v 1. Play around with the window title. You can change it at any time after its creation,
* and it can store a lot of information in an easily accessible place.
* You could use it for the name of the model you loaded, the animation replay speed, and more.
*
* v 2. Set a callback for the handling of window resizing. This will be handy once we have enabled
* 3D rendering, and you will need to adjust the sizes of the other buffers too.
*
* v 3. Store information about some keys, such as W, A, S, and D or the cursor keys.
* Set the status when pressed and clear it on release.
* We will need the stored status of the keys in Chapter 5 to move around inside the virtual world.
*
* v 4. Add support for raw mouse movement on a mouse button press only.
* Imagine you would like to rotate the view around your animated model while the left button is being pressed
* or zoom in and out while the right button is being pressed
*/



std::unordered_map<int, KeyboardKeyEnum> ASCIICodeToKeyboardKeyMap{
  std::make_pair(87, KeyboardKeyEnum::KEY_W),
  std::make_pair(65, KeyboardKeyEnum::KEY_A),
  std::make_pair(83, KeyboardKeyEnum::KEY_S),
  std::make_pair(68, KeyboardKeyEnum::KEY_D),
};

std::unordered_map<KeyboardKeyEnum, int> KeyboardKeyToASCIICodeMap{
  std::make_pair(KeyboardKeyEnum::KEY_W, 87),
  std::make_pair(KeyboardKeyEnum::KEY_A, 65),
  std::make_pair(KeyboardKeyEnum::KEY_S, 83),
  std::make_pair(KeyboardKeyEnum::KEY_D, 68),
};

std::unordered_map<int, MouseKeyEnum> OpenGLMouseKeyMap{
  std::make_pair(GLFW_MOUSE_BUTTON_LEFT, MouseKeyEnum::KEY_LMB),
  std::make_pair(GLFW_MOUSE_BUTTON_RIGHT, MouseKeyEnum::KEY_RMB),
  std::make_pair(GLFW_MOUSE_BUTTON_MIDDLE, MouseKeyEnum::KEY_MMB),
};

bool Window::init(unsigned int width, unsigned int height, std::string title) {
  if (!glfwInit()) {
    Logger::log(1, "%s: glfwInit() error\n", __FUNCTION__);
    return false;
  }

  /* set a "hint" for the NEXT window created*/
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  mInitialTitle = title;
  mWindow = glfwCreateWindow(width, height, mInitialTitle.c_str(), nullptr, nullptr);

  glfwMakeContextCurrent(mWindow);

  mRenderer = std::make_unique<OGLRenderer>();
  if (!mRenderer->init(width, height))
  {
    glfwTerminate();
    return false;
  }

  if (!mWindow) {
    Logger::log(1, "%s: Could not create window\n", __FUNCTION__);
    glfwTerminate();
    return false;
  }

  mModel = std::make_unique<Model>();
  mModel->init();
  
  /* the C handlers needs a little 'stunt' here */
  /* 1) save the pointer to the instance as user pointer */
  glfwSetWindowUserPointer(mWindow, this);

  /* 2) use a lambda to get the pointer and call the member function */
  glfwSetWindowPosCallback(mWindow, [](GLFWwindow *win, int xpos, int ypos) {
    auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
    thisWindow->handleWindowMoveEvents(xpos, ypos);
    }
  );

  glfwSetWindowIconifyCallback(mWindow, [](GLFWwindow *win, int minimized) {
    auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
    thisWindow->handleWindowMinimizedEvents(minimized);
    }
  );

  glfwSetWindowMaximizeCallback(mWindow, [](GLFWwindow *win, int maximized) {
    auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
    thisWindow->handleWindowMaximizedEvents(maximized);
    }
    );

  glfwSetWindowSizeCallback(mWindow, [](GLFWwindow *win, int width, int height) {
    auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
    thisWindow->handleWindowResizedEvents(width, height);
    }
  );

  glfwSetWindowCloseCallback(mWindow, [](GLFWwindow *win) {
    auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
    thisWindow->handleWindowCloseEvents();
    }
  );

  glfwSetKeyCallback(mWindow, [](GLFWwindow *win, int key, int scancode, int action, int mods) {
    auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
    thisWindow->handleKeyEvents(key, scancode, action, mods);
    }
  );

  glfwSetMouseButtonCallback(mWindow, [](GLFWwindow *win, int button, int action, int mods) {
    auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
    thisWindow->handleMouseButtonEvents(button, action, mods);
    }
  );

  glfwSetCursorPosCallback(mWindow, [](GLFWwindow *win, double xpos, double ypos) {
    auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
    thisWindow->handleMousePositionEvents(xpos, ypos);
    }
  );

  glfwSetCursorEnterCallback(mWindow, [](GLFWwindow *win, int enter) {
    auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
    thisWindow->handleMouseEnterLeaveEvents(enter);
    }
  );

  for (auto AsciiCodeToKeyPair : ASCIICodeToKeyboardKeyMap)
  {
    int GLFWKeyStatus = glfwGetKey(mWindow, AsciiCodeToKeyPair.first);
    ButtonStatus InternalStatus = ButtonStatus::Undefined;
    switch (GLFWKeyStatus)
    {
    case GLFW_PRESS:
      InternalStatus = ButtonStatus::Pressed;
      break;
    case GLFW_REPEAT:
      InternalStatus = ButtonStatus::Pressed;
      break;
    case GLFW_RELEASE:
      InternalStatus = ButtonStatus::Released;
      break;
    default:
      InternalStatus = ButtonStatus::Undefined;  
    }
    mCurrentKeyStatusMap.insert(std::pair(AsciiCodeToKeyPair.second, InternalStatus));
  }


  Logger::log(1, "%s: Window successfully initialized\n", __FUNCTION__);
  return true;
}

void Window::handleWindowMoveEvents(int xpos, int ypos) {
  Logger::log(1, "%s: Window has been moved to %i/%i\n", __FUNCTION__, xpos, ypos);
}

void Window::handleWindowMinimizedEvents(int minimized) {
  if (minimized) {
    Logger::log(1, "%s: Window has been minimized\n", __FUNCTION__);
  } else {
    Logger::log(1, "%s: Window has been restored\n", __FUNCTION__);
  }
}

void Window::handleWindowMaximizedEvents(int maximized) {
  if (maximized) {
    Logger::log(1, "%s: Window has been maximized\n", __FUNCTION__);
  } else {
    Logger::log(1, "%s: Window has been restored\n", __FUNCTION__);
  }
}

void Window::handleWindowResizedEvents(int width, int height){
  mRenderer->setSize(width, height);
  Logger::log(1, "%s: Window has been resized to %i/%i\n", __FUNCTION__, width, height);
}

void Window::handleWindowCloseEvents() {
  Logger::log(1, "%s: Window got close event... bye!\n", __FUNCTION__);
}

void Window::handleKeyEvents(int key, int scancode, int action, int mods) {
  std::string actionName;
  
  ButtonStatus status = ButtonStatus::Undefined;
  
  switch (action) {
    case GLFW_PRESS:
      status = ButtonStatus::Pressed;
      actionName = "pressed";
      break;
    case GLFW_RELEASE:
      status = ButtonStatus::Released;
      actionName = "released";
      break;
    case GLFW_REPEAT:
      actionName = "repeated";
      break;
    default:
      actionName = "invalid";
      break;
  }

  const char *keyName = glfwGetKeyName(key, 0);
  
  if (status == ButtonStatus::Undefined)
  {
    Logger::log(2, "%s: button press is ignored (key %i, scancode %i) %s\n", __FUNCTION__, keyName, key, scancode, actionName.c_str());
    return;
  }

  KeyboardKeyEnum AffectedKey = KeyboardKeyEnum::KEY_UNDEFINED;
  auto It = ASCIICodeToKeyboardKeyMap.find(key);
  if (It != ASCIICodeToKeyboardKeyMap.end())
  {
    AffectedKey = It->second;
  }
  
  const bool buttonHandled = AffectedKey != KeyboardKeyEnum::KEY_UNDEFINED && status != ButtonStatus::Undefined;

  if (!buttonHandled)
  {
    Logger::log(1, "%s: button press is ignored. key %s \n", __FUNCTION__, keyName);
    return; 
  }
  
  handleKeyChangedState(AffectedKey, status);

  if (status == ButtonStatus::Pressed)
  {
    handleKeyPressed(AffectedKey);
  }

  if (status == ButtonStatus::Released)
  {
    handleKeyReleased(AffectedKey);
  }
  
  Logger::log(2, "%s: key %s (key %i, scancode %i) %s. It was handled\n", __FUNCTION__, keyName, key, scancode,
    actionName.c_str());
}

void Window::handleMouseButtonEvents(int button, int action, int mods) {
  std::string actionName;
  ButtonStatus status = ButtonStatus::Undefined;
  switch (action) {
    case GLFW_PRESS:
      status = ButtonStatus::Pressed;
      actionName = "pressed";
      break;
    case GLFW_RELEASE:
      status = ButtonStatus::Released;
      actionName = "released";
      break;
    case GLFW_REPEAT:
      actionName = "repeated";
      break;
    default:
      actionName = "invalid";
      break;
  }

  bool buttonHandled = false;
  MouseKeyEnum mouseKey = MouseKeyEnum::KEY_UNDEFINED;
  std::string mouseButtonName;
  switch(button) {
    case GLFW_MOUSE_BUTTON_LEFT:
      buttonHandled = true;
      mouseKey = MouseKeyEnum::KEY_LMB;
      mouseButtonName = "left";
      break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      buttonHandled = true;
      mouseKey = MouseKeyEnum::KEY_MMB;
      mouseButtonName = "middle";
      break;
    case GLFW_MOUSE_BUTTON_RIGHT:
      buttonHandled = true;
      mouseKey = MouseKeyEnum::KEY_RMB;
      mouseButtonName = "right";
      break;
    default:
      mouseKey = MouseKeyEnum::KEY_UNDEFINED;
      mouseButtonName = "other";
      break;
  }

  bool validClick = mouseKey != MouseKeyEnum::KEY_UNDEFINED && status != ButtonStatus::Undefined;

  if (!validClick)
  {
    Logger::log(1, "%s: mouse click was not handled\n", __FUNCTION__);
    return;
  }

  handleMouseKeyChangedState(mouseKey, status);
  if (status == ButtonStatus::Pressed)
  {
    handleMouseKeyPressed(mouseKey);
  }
  if (status == ButtonStatus::Released)
  {
    handleMouseKeyReleased(mouseKey);
  }
  Logger::log(2, "%s: %s mouse button (%i) %s. Handled %d\n", __FUNCTION__, mouseButtonName.c_str(), button,
    actionName.c_str(), buttonHandled);
}

void Window::handleMousePositionEvents(double xpos, double ypos) {
  std::string new_title = ". MouseAt = (";
  new_title += std::to_string((int)xpos);
  new_title += ", ";
  new_title += std::to_string((int)ypos);
  new_title += ")";
  
  glfwSetWindowTitle(this->mWindow, (mInitialTitle + new_title).c_str());
  
  Logger::log(2, "%s: Mouse is at position %lf/%lf\n", __FUNCTION__, xpos, ypos);
}

void Window::handleMouseEnterLeaveEvents(int enter) {
  if (enter) {
    Logger::log(1, "%s: Mouse entered window\n", __FUNCTION__);
  } else {
    glfwSetWindowTitle(this->mWindow, mInitialTitle.c_str());
    Logger::log(1, "%s: Mouse left window\n", __FUNCTION__);
  }
}

void Window::mainLoop() {
  glfwSwapInterval(1);
  mRenderer->uploadData(mModel->getVertexData());
  
  while (!glfwWindowShouldClose(mWindow)) {
    mRenderer->draw();
    
    glfwSwapBuffers(mWindow);
    glfwPollEvents();
  }
}

void Window::cleanup() {
  Logger::log(1, "%s: Terminating Window\n", __FUNCTION__);
  glfwDestroyWindow(mWindow);
  glfwTerminate();
}

void Window::handleKeyPressed(KeyboardKeyEnum Key)
{
  Logger::log(1, "%s: KeyPressed - %d \n", __FUNCTION__, Key);
}

void Window::handleKeyReleased(KeyboardKeyEnum Key)
{
  Logger::log(1, "%s: KeyReleased - %d \n", __FUNCTION__, Key);
}

void Window::handleKeyChangedState(KeyboardKeyEnum Key, ButtonStatus NewStatus)
{
  auto It = mCurrentKeyStatusMap.find(Key);
  if (It != mCurrentKeyStatusMap.end())
  {
    It->second = NewStatus;
  }
  Logger::log(2, "%s: KeyChangedState - %d->%d \n", __FUNCTION__, Key, NewStatus);
}

void Window::handleMouseKeyPressed(MouseKeyEnum Key)
{
  if (Key == MouseKeyEnum::KEY_LMB)
  {
    hideTheMouseCursor();
  }
  Logger::log(1, "%s: MouseKeyPressed - %d \n", __FUNCTION__, Key);
}

void Window::handleMouseKeyReleased(MouseKeyEnum Key)
{
  if (Key == MouseKeyEnum::KEY_LMB)
  {
    unHideTheMouseCursor();
  }
  Logger::log(1, "%s: MouseKeyReleased - %d \n", __FUNCTION__, Key);
}

void Window::handleMouseKeyChangedState(MouseKeyEnum Key, ButtonStatus NewStatus)
{
  auto It = mCurrentMouseKeyStatusMap.find(Key);
  if (It != mCurrentMouseKeyStatusMap.end())
  {
    It->second = NewStatus;
  }
  Logger::log(2, "%s: MouseKeyChangedState - %d->%d \n", __FUNCTION__, Key, NewStatus);
}

void Window::hideTheMouseCursor()
{
  if (mouseHidden)
  {
    return;
  }
  glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  if (glfwRawMouseMotionSupported()) {
    glfwSetInputMode(mWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  }
  mouseHidden = true;
}

void Window::unHideTheMouseCursor()
{
  if (!mouseHidden)
  {
    return;
  }
  glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  mouseHidden = false;
}
