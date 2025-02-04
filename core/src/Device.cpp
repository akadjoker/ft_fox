#include <cmath>
#include <SDL2/SDL.h>
#include "Device.hpp"
#include "Input.hpp"
#include "Driver.hpp"

#if defined(PLATFORM_DESKTOP) && defined(_WIN32) && (defined(_MSC_VER) || defined(__TINYC__))

    #include "wdirent.h"    // Required for: DIR, opendir(), closedir()
#else
    #include <dirent.h>             // Required for: DIR, opendir(), closedir() 
#endif

#if defined(_WIN32)
    #include <direct.h>             // Required for: _getch(), _chdir()
    #define GETCWD _getcwd          // NOTE: MSDN recommends not to use getcwd(), chdir()
    #define CHDIR _chdir
    #include <io.h>                 // Required for: _access() [Used in FileExists()]
#else
    #include <unistd.h>             // Required for: getch(), chdir() (POSIX), access()
    #define GETCWD getcwd
    #define CHDIR chdir
#endif



#ifdef PLATFORM_WIN

#define CONSOLE_COLOR_RESET ""
#define CONSOLE_COLOR_GREEN ""
#define CONSOLE_COLOR_RED ""
#define CONSOLE_COLOR_PURPLE ""

#else

#define CONSOLE_COLOR_RESET "\033[0m"
#define CONSOLE_COLOR_GREEN "\033[1;32m"
#define CONSOLE_COLOR_RED "\033[1;31m"
#define CONSOLE_COLOR_PURPLE "\033[1;35m"

#endif
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

double GetTime()
{
    return static_cast<double>(SDL_GetTicks()) / 1000;
}



Device::Device() : window(nullptr),
                   glContext(nullptr),
                   width(800),
                   height(600),
                   title("OpenGL Device"),
                   isRunning(true)
{
    m_current = 0;
    m_previous = 0;
    m_update = 0;
    m_draw = 0;
    m_frame = 0;
    // SetTargetFPS(60);

    m_current = GetTime();
    m_draw = m_current - m_previous;
    m_previous = m_current;
    m_frame = m_update + m_draw;
}

Device::~Device()
{
    
}



void Device::Wait(float ms)
{
    Uint32 time = static_cast<Uint32>(ms * 1000.0f);
    SDL_Delay(time);
}

int Device::GetFPS(void)
{
#define FPS_CAPTURE_FRAMES_COUNT 30   // 30 captures
#define FPS_AVERAGE_TIME_SECONDS 0.5f // 500 millisecondes
#define FPS_STEP (FPS_AVERAGE_TIME_SECONDS / FPS_CAPTURE_FRAMES_COUNT)

    static int index = 0;
    static float history[FPS_CAPTURE_FRAMES_COUNT] = {0};
    static float average = 0, last = 0;
    float fpsFrame = GetFrameTime();

    if (fpsFrame == 0)
        return 0;

    if ((GetTime() - last) > FPS_STEP)
    {
        last = (float)GetTime();
        index = (index + 1) % FPS_CAPTURE_FRAMES_COUNT;
        average -= history[index];
        history[index] = fpsFrame / FPS_CAPTURE_FRAMES_COUNT;
        average += history[index];
    }

    return (int)roundf(1.0f / average);
}

void Device::SetTargetFPS(int fps)
{
    if (fps < 1)
        m_target = 0.0;
    else
        m_target = 1.0 / (double)fps;
}

float Device::GetFrameTime(void)
{
    return (float)m_frame;
}

double Device::GetTime(void)
{
    unsigned int ms = SDL_GetTicks(); // Elapsed time in milliseconds since SDL_Init()
    double time = (double)ms / 1000;
    return time;
}

u32 Device::GetTicks(void)
{
    return SDL_GetTicks();
}
bool Device::initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("SDL initialization failed: %s", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
   // SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);


    return true;
}

void glDebugOutput(u32 source,
                   u32 type,
                   u32 id,
                   u32 severity,
                   s32 length,
                   const char *message,
                   const void *userParam)
{
    (void)length;
    (void)userParam;
    (void)severity;
    GLuint ignore_ids[1] = {131185}; // "will use video memory..."

    for (int i = 0; i < (int)ARRAY_SIZE_IN_ELEMENTS(ignore_ids); i++)
    {
        if (id == ignore_ids[i])
        {
            return;
        }
    }

    //  SDL_LogWarn(1,"!!! Debug callback !!!\n");
    SDL_LogDebug(2, "Debug message: id %d, %s", id, message);

    //   SDL_LogWarn(1,"Message source: ");
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        SDL_LogWarn(1, "API");
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        SDL_LogWarn(1, "Device System");
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        SDL_LogWarn(1, "Shader Compiler");
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        SDL_LogWarn(1, "Third Party");
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        SDL_LogWarn(1, "Application");
        break;
    case GL_DEBUG_SOURCE_OTHER:
        SDL_LogWarn(1, "Other");
        break;
    }

    //  printf("Error type: ");
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        SDL_LogError(2, "Error");
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        SDL_LogError(2, "Deprecated Behaviour");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        SDL_LogError(2, "Undefined Behaviour");
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        SDL_LogError(2, "Portability");
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        SDL_LogError(2, "Performance");
        break;
    case GL_DEBUG_TYPE_MARKER:
        SDL_LogError(2, "Marker");
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        SDL_LogError(2, "Push Group");
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        SDL_LogError(2, "Pop Group");
        break;
    case GL_DEBUG_TYPE_OTHER:
        SDL_LogError(2, "Other");
        break;
    }
}
bool Device::initGL(bool vzync)
{
    glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
        SDL_Log("OpenGL context creation failed: %s", SDL_GetError());
        return false;
    }

    if (vzync)
    {
        SetTargetFPS(60);
        SDL_GL_SetSwapInterval(1); // Enable VSync
    }
    else
    {
        SetTargetFPS(INT_MAX);
        SDL_GL_SetSwapInterval(0);
    }

   // glEnable(GL_DEBUG_OUTPUT);
   // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    Driver::Instance().Init();

 //   glDebugMessageCallback(glDebugOutput, nullptr);
                           //     glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
                   

    LogInfo("[DEVICE] Vendor  :  %s", glGetString(GL_VENDOR));
    LogInfo("[DEVICE] Renderer:  %s", glGetString(GL_RENDERER));
    LogInfo("[DEVICE] Version :  %s", glGetString(GL_VERSION));

    LogInfo("[DEVICE] GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

        glDepthFunc(GL_LESS); 
        glDepthMask(GL_TRUE); 


        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);  


    return true;
}

int Device::GetWidth() const
{

    return width;
}
int Device::GetHeight() const
{
    return height;
}

bool Device::Init(const char *windowTitle, int windowWidth, int windowHeight, bool vzync,bool fullScreen)
{
    title = windowTitle;
    width = windowWidth;
    height = windowHeight;

    if (!initSDL())
    {
        return false;
    }

    if (fullScreen)
    {
        window = SDL_CreateWindow(
            title.c_str(),
            0, 0,
            width, height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else 
    {

    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    }

    if (!window)
    {
        SDL_Log("Device creation failed: %s", SDL_GetError());
        return false;
    }

    if (!initGL(vzync))
    {
        return false;
    }

    Input::Init();
  //  GUI::Instance();
    return true;
}

void Device::Close()
{
    LogInfo("Release Device.");
    Driver::Instance().Release();
    LogInfo("Release Gui.");
  //  GUI::Instance()->DestroyInstance();
    if (glContext)
    {
        LogInfo("Release Opengl Context.");
        SDL_GL_DeleteContext(glContext);
        glContext = nullptr;
    }

    if (window)
    {
        LogInfo("Release Window.");
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    LogInfo("Quit :) By! By!");
    SDL_Quit();
}

void Device::Swap()
{
    SDL_GL_SwapWindow(window);

    m_current = GetTime();
    m_draw = m_current - m_previous;
    m_previous = m_current;
    m_frame = m_update + m_draw;

    // Wait for some milliseconds...
    if (m_frame < m_target)
    {
        Wait((float)(m_target - m_frame));

        m_current = GetTime();
        double waitTime = m_current - m_previous;
        m_previous = m_current;

        m_frame += waitTime; // Total frame time: update + draw + wait
    }

    Input::Update();
}

void Device::SetTitle(const char *newTitle)
{
    title = newTitle;
    SDL_SetWindowTitle(window, title.c_str());
}

void Device::SetSize(int newWidth, int newHeight)
{
    width = newWidth;
    height = newHeight;
    SDL_SetWindowSize(window, width, height);
}

void Device::Update()
{
    m_current = GetTime(); // Number of elapsed seconds since InitTimer()
    m_update = m_current - m_previous;
    m_previous = m_current;
    Driver::Instance().ResetStats();

     while (SDL_PollEvent(&event))
    {
        
        switch (event.type)
        {
            case SDL_QUIT:
            {
                 isRunning = false;
                return;
            }
            case SDL_WINDOWEVENT:
            {
                switch (event.window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:
                    {
                        width = event.window.data1;
                        height = event.window.data2;
                        Driver::Instance().Resize(width,height);
                        break;
                    }
                }
                break;
            }
        }
        Input::ProcessEvent(event);

      

    }
    
}
bool Device::Running()
{
    Update();
    return (isRunning && !Input::ShouldQuit());
}

static void LogMessage(int level, const char *msg, va_list args)
{
    time_t rawTime;
    struct tm *timeInfo;
    char timeBuffer[80];

    time(&rawTime);
    timeInfo = localtime(&rawTime);

    strftime(timeBuffer, sizeof(timeBuffer), "[%H:%M:%S]", timeInfo);

    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), msg, args);

    switch (level)
    {
    case 0:
        SDL_LogInfo(0, "%s%s: %s%s", CONSOLE_COLOR_GREEN, timeBuffer, buffer, CONSOLE_COLOR_RESET);
        break;
    case 1:
        SDL_LogError(0, "%s%s: %s%s", CONSOLE_COLOR_RED, timeBuffer, buffer, CONSOLE_COLOR_RESET);
        break;
    case 2:
        SDL_LogWarn(0, "%s%s: %s%s", CONSOLE_COLOR_PURPLE, timeBuffer, buffer, CONSOLE_COLOR_RESET);
        break;
    }
}

void LogError(const char *msg, ...)
{

    va_list args;
    va_start(args, msg);
    LogMessage(1, msg, args);
    va_end(args);
}

void LogWarning(const char *msg, ...)
{

    va_list args;
    va_start(args, msg);
    LogMessage(2, msg, args);
    va_end(args);
}

void LogInfo(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    LogMessage(0, msg, args);
    va_end(args);
}

char *LoadTextFile(const char *fileName)
{
    char* text = NULL;

  
        SDL_RWops* textFile = SDL_RWFromFile(fileName, "rt");
        if (textFile != NULL)
        {
            unsigned  int size = (int)SDL_RWsize(textFile);
            if (size > 0)
            {
                text = (char*)malloc((size + 1) * sizeof(char));
                unsigned int count = (unsigned int)SDL_RWread(textFile, text, sizeof(char), size);
                if (count < size) text = (char*)realloc(text, count + 1);
                text[count] = '\0';

                SDL_LogInfo(0, "FILEIO: [%s] Text file loaded successfully", fileName);
            }
            else SDL_LogError(0, "FILEIO: [%s] Failed to read text file", fileName);

            SDL_RWclose(textFile);
        }
        else SDL_LogError(0, "FILEIO: [%s] Failed to open text file", fileName);
    

    return text;

}

unsigned char *LoadDataFile(const char *fileName, unsigned int *bytesRead)
{
     unsigned char* data = NULL;
    *bytesRead = 0;

 
        SDL_RWops* file = SDL_RWFromFile(fileName, "rb");


        if (file != NULL)
        {
            unsigned int size = (int)SDL_RWsize(file);

            if (size > 0)
            {
                data = (unsigned char*)malloc(size * sizeof(unsigned char));

                unsigned int count = (unsigned int)SDL_RWread(file, data, sizeof(unsigned char), size);
                *bytesRead = count;

                 SDL_LogInfo(0, "FILEIO: [%s] File loaded successfully", fileName);

            }
            else SDL_LogError(0, "FILEIO: [%s] Failed to read file", fileName);
            SDL_RWclose(file);
        }
        else SDL_LogError(0, "FILEIO: [%s] Failed to open file", fileName);

    return data;
}

bool FileExists(const char *fileName)
{
   bool result = false;

#if defined(_WIN32)
    if (_access(fileName, 0) != -1) result = true;
#else
    if (access(fileName, F_OK) != -1) result = true;
#endif
    return result;
}

bool IsFileExtension(const char *fileName, const char *ext)
{
    bool result = false;

    const char *fileExt = strrchr(fileName, '.');

    if (fileExt != NULL)
    {
        if (strcmp(fileExt, ext) == 0) result = true;
    }

    return result;
}

bool DirectoryExists(const char *dirPath)
{
    bool result = false;

    DIR *dir = opendir(dirPath);
    if (dir != NULL)
    {
        result = true;
        closedir(dir);
    }

    return result;
}

const char *GetFileExtension(const char *fileName)
{
    const char *fileExt = strrchr(fileName, '.');

    if (fileExt != NULL) return fileExt;

    return NULL;
}

const char *GetFileName(const char *filePath)
{
    const char *fileName = strrchr(filePath, '/');

    if (fileName != NULL) return fileName + 1;

    return filePath;
}

const char *GetFileNameWithoutExt(const char *filePath)
{
    static char baseName[256];
    strcpy(baseName, GetFileName(filePath));

    char *dot = strrchr(baseName, '.');
    if (dot) *dot = '\0';

    return baseName;
}

const char *GetDirectoryPath(const char *filePath)
{
    static char dirPath[256];
    strcpy(dirPath, filePath);

    char *lastSlash = strrchr(dirPath, '/');

    if (lastSlash != NULL) lastSlash[1] = '\0';
    else dirPath[0] = '\0';

    return dirPath;
}
