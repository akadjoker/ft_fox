
#include "Input.hpp"


std::unordered_map<SDL_Keycode, bool> Input::currentKeys;
std::unordered_map<SDL_Keycode, bool> Input::previousKeys;
std::unordered_map<uint8_t, bool> Input::currentMouseButtons;
std::unordered_map<uint8_t, bool> Input::previousMouseButtons;
int Input::mouseX = 0;
int Input::mouseY = 0;
int Input::mouseDeltaX = 0;
int Input::mouseDeltaY = 0;
bool Input::quit = false;
int Input::quit_key = SDLK_ESCAPE;

void Input::Init()
{
    currentKeys.clear();
    previousKeys.clear();
    currentMouseButtons.clear();
    previousMouseButtons.clear();
    mouseX = 0;
    mouseY = 0;
    mouseDeltaX = 0;
    mouseDeltaY = 0;
    quit = false;
}

void Input::Update()
{
    // Copy current state to previous state
    previousKeys = currentKeys;
    previousMouseButtons = currentMouseButtons;

    // Reset delta movement
    mouseDeltaX = 0;
    mouseDeltaY = 0;
}

void Input::ProcessEvent(const SDL_Event &event)
{
    switch (event.type)
    {
    case SDL_KEYDOWN:
        currentKeys[event.key.keysym.sym] = true;
        break;

    case SDL_KEYUP:
        currentKeys[event.key.keysym.sym] = false;
        //SDL_Log( "key %d"   ,event.key.key);
        if (Input::quit_key==event.key.keysym.sym)
            quit=true;
        break;

    case SDL_MOUSEBUTTONDOWN:
        mouseX = event.motion.x;
        mouseY = event.motion.y;
           currentMouseButtons[event.button.button] = true;
        break;

    case SDL_MOUSEBUTTONUP:
        currentMouseButtons[event.button.button] = false;
         break;

    case SDL_MOUSEMOTION:
        mouseX = event.motion.x;
        mouseY = event.motion.y;
        mouseDeltaX = event.motion.xrel;
        mouseDeltaY = event.motion.yrel;
         break;

    case SDL_QUIT:
        quit = true;
        break;
    }
}

bool Input::IsKeyPressed(SDL_Keycode key)
{
    return currentKeys[key] && !previousKeys[key];
}

bool Input::IsKeyDown(SDL_Keycode key)
{
    return currentKeys[key];
}

bool Input::IsKeyReleased(SDL_Keycode key)
{
    return !currentKeys[key] && previousKeys[key];
}

bool Input::IsKeyUp(SDL_Keycode key)
{
    return !currentKeys[key];
}

bool Input::IsMouseButtonPressed(uint8_t button)
{
    return currentMouseButtons[button] && !previousMouseButtons[button];
}

bool Input::IsMouseButtonDown(uint8_t button)
{
    return currentMouseButtons[button];
}

bool Input::IsMouseButtonReleased(uint8_t button)
{
    return !currentMouseButtons[button] && previousMouseButtons[button];
}

bool Input::IsMouseButtonUp(uint8_t button)
{
    return !currentMouseButtons[button];
}

int Input::GetMouseX()
{
    return mouseX;
}

int Input::GetMouseY()
{
    return mouseY;
}

void Input::GetMousePosition(int &x, int &y)
{
    x = mouseX;
    y = mouseY;
}

int Input::GetMouseDeltaX()
{
    return mouseDeltaX;
}

int Input::GetMouseDeltaY()
{
    return mouseDeltaY;
}

void Input::GetMouseDelta(int &deltaX, int &deltaY)
{
    deltaX = mouseDeltaX;
    deltaY = mouseDeltaY;
}

bool Input::ShouldQuit()
{
    return quit;
}

void Input::SetQuit(bool shouldQuit)
{
    quit = shouldQuit;
}

void Input::SetQuitKey(int key)
{
    quit_key = key;
}