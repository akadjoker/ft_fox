
#include "Core.hpp"
#include "World.hpp"
#include "Chunk.hpp"

extern "C" const char *__lsan_default_suppressions()
{
    return "leak:libSDL2\n"
           "leak:SDL_DBus\n";
}

struct Skybox
{
    unsigned int skyboxVAO, skyboxVBO;

    void Init()
    {

        float skyboxVertices[] = {

            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f};

        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    void Render(unsigned int skybox)
    {
        glBindVertexArray(skyboxVAO);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
};

GLuint LoadCubemap(const char *top_name, const char *side_name, const char *bot_name)
{

    GLenum format = GL_RGB;
    GLenum glFormat = GL_RGB;

    //   format = GL_RGBA8;
    //     glFormat = GL_RGBA;

    GLuint texture_id;

    Pixmap top;
    top.Load(top_name);
    Pixmap side;
    side.Load(side_name);
    Pixmap bot;
    bot.Load(bot_name);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0f);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, side.width, side.height, 0, glFormat,
                 GL_UNSIGNED_BYTE, side.pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, side.width, side.height, 0, glFormat,
                 GL_UNSIGNED_BYTE, side.pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, top.width, top.height, 0, glFormat,
                 GL_UNSIGNED_BYTE, top.pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, bot.width, bot.height, 0, glFormat,
                 GL_UNSIGNED_BYTE, bot.pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, side.width, side.height, 0, glFormat,
                 GL_UNSIGNED_BYTE, side.pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, side.width, side.height, 0, glFormat,
                 GL_UNSIGNED_BYTE, side.pixels);
   // glGenerateMipmap(GL_TEXTURE_2D);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture_id;
}

GLuint LoadSkybox(const char *top_name, const char *left_name, const char *right_name, const char *back_name, const char *front_name, const char *bot_name)
{
    int width;
    int height;

    GLuint texture_id;

    Pixmap top;
    top.Load(top_name);
    Pixmap right;
    right.Load(right_name);
    Pixmap left;
    left.Load(left_name);
    Pixmap front;
    front.Load(front_name);
    Pixmap back;
    back.Load(back_name);
    Pixmap bot;
    bot.Load(bot_name);

    width = top.width;
    height = top.height;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, right.pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, left.pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, top.pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, bot.pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, front.pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, back.pixels);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture_id;
}

int main(int argc, char *argv[])
{
    Device device;

    bool ABORT = false;

    if (!device.Init("FT_VOX BY Luis Santos AKA DJOKER & Felix Le Bihan ", 1024, 720, true, true))
    {
        return 1;
    }

    RenderBatch batch;
    batch.Init(1, 1024);
    Shader shader;
    Shader shaderVoxels;
    Shader skyboxShader;

    Font font;

    {
        const char *vShader = GLSL(
            layout(location = 0) in vec3 position;
            layout(location = 1) in vec2 texCoord;
            layout(location = 2) in vec4 color;

            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;

            out vec2 TexCoord;
            out vec4 vertexColor;
            void main() {
                gl_Position = projection * view * model * vec4(position, 1.0);
                TexCoord = texCoord;

                vertexColor = color;
            });

        const char *fShader = GLSL(
            in vec2 TexCoord;
            out vec4 color;
            in vec4 vertexColor;
            uniform sampler2D texture0;
            void main() {
                color = texture(texture0, TexCoord) * vertexColor;
            });

        shader.Create(vShader, fShader);
        shader.LoadDefaults();
    }

    {
        const char *vShader = GLSL(
            layout(location = 0) in vec3 vertexPosition_modelspace;
            layout(location = 1) in vec3 positions;
            layout(location = 2) in uint which;

            out vec3 textureDir;
            out vec3 worldPos;
            flat out uint texturess;

            uniform mat4 MVP;
            uniform mat4 ModelMatrix;

            void main() {
                vec4 worldPosition = ModelMatrix * vec4(vertexPosition_modelspace + positions, 1);
                worldPos = worldPosition.xyz;
                gl_Position = MVP * worldPosition;
                textureDir = vertexPosition_modelspace;
                texturess = which;
            });

        const char *fShader = GLSL(
            in vec3 textureDir;
            in vec3 worldPos;

            out vec3 color;

            flat in uint texturess;

            uniform samplerCube grass;
            uniform samplerCube stone;
            uniform samplerCube sand;
            uniform samplerCube water;

            // Iluminação
            uniform vec3 lightDir;
            uniform vec3 lightColor;
            uniform vec3 ambientColor;

            void main() {
                vec3 texColor;

                if (texturess == 1)
                    texColor = texture(grass, textureDir).rgb;
                else if (texturess == 2)
                    texColor = texture(stone, textureDir).rgb;
                else if (texturess == 3)
                    texColor = texture(sand, textureDir).rgb;
                else if (texturess == 4)
                    texColor = texture(water, textureDir).rgb;
                else
                    texColor = vec3(1.0);

                vec3 normal = normalize(textureDir);
                float diff = max(dot(normal, normalize(lightDir)), 0.0);
                if (diff < 0.0)
                {
                    discard;
                }

                vec3 lighting = ambientColor + (lightColor * diff);

                color = texColor * lighting;
            });

        if (!shaderVoxels.Create(vShader, fShader))
        {
            ABORT = true;
        }
        shaderVoxels.LoadDefaults();

        Vec3 lightDirection = Vec3(0.0f, 0.9f, 0.02f);
        Vec3 lightColor = Vec3(1.0f, 1.0f, 1.0f);   // Luz branca
        Vec3 ambientColor = Vec3(0.5f, 0.5f, 0.5f); // Pequena luz ambiente

        shaderVoxels.SetFloat("lightDir", lightDirection.x, lightDirection.y, lightDirection.z);
        shaderVoxels.SetFloat("lightColor", lightColor.x, lightColor.y, lightColor.z);
        shaderVoxels.SetFloat("ambientColor", ambientColor.x, ambientColor.y, ambientColor.z);
    }

    {
        // Vertex Shader
        const char *vShader = GLSL(
            layout(location = 0) in vec3 aPos;

            out vec3 TexCoords;

            uniform mat4 view;
            uniform mat4 projection;

            void main() {
                TexCoords = aPos;

                mat3 v = mat3(view);

                vec4 pos = projection * mat4(v) * vec4(aPos, 1.0); //removemos a position :D
                gl_Position = pos.xyww;
            });

        const char *fShader = GLSL(

            out vec4 FragColor;

            in vec3 TexCoords;

            uniform samplerCube skybox;

            void main() {
                FragColor = texture(skybox, TexCoords);
            });

        if (!skyboxShader.Create(vShader, fShader))
        {
            ABORT = true;
        }
        skyboxShader.LoadDefaults();
    }

    font.LoadDefaultFont();
    font.SetBatch(&batch);
    font.SetSize(12);

   


    Camera camera(1024.0f / 720.0f);
    camera.setAspectRatio(1024.0f / 720.0f);



    camera.setFarPlane(VIEW_DISTANCE);
    camera.setNearPlane(0.1f);
    camera.setPosition(0.0f, 130.0f, -20.0f);

    Driver::Instance().SetClearColor(0.2f, 0.3f, 0.3f, 1.0);

    float cameraSpeed = 5.5f;
    float mouseSensitivity = 0.1f;

    GLuint grass = LoadCubemap("assets/grass_top.png", "assets/grass_side.png", "assets/dirt.png");
    GLuint stone = LoadCubemap("assets/stone.png", "assets/stone.png", "assets/stone.png");
    GLuint sand = LoadSkybox("assets/sand.png", "assets/sand.png", "assets/sand.png", "assets/sand.png", "assets/sand.png", "assets/sand.png");
    GLuint water = LoadSkybox("assets/water.png", "assets/water.png", "assets/water.png", "assets/water.png", "assets/water.png", "assets/water.png");

    GLuint skyboxTex = LoadSkybox("assets/sky/top.jpg", "assets/sky/left.jpg", "assets/sky/right.jpg", "assets/sky/back.jpg", "assets/sky/front.jpg", "assets/sky/bottom.jpg");
    Skybox skybox;
    skybox.Init();

    //  Texture2D texture;

    // if (!texture.Load("assets/Blocks.png"))
    // {
    //     unsigned char data[4] = {255, 255, 255, 255};
    //     texture.LoadFromMemory(data, 4, 1, 1);
    // }

    // texture.SetAnisotropicFiltering(8.0f);
    // texture.SetWrapS(WrapMode::Repeat);
    // texture.SetWrapT(WrapMode::Repeat);
    // texture.SetMinFilter(FilterMode::Nearest);
    // texture.SetMagFilter(FilterMode::Nearest);

    bool isDebug = false;
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);
    float totalTime = 0.0f;

    GLuint vaoID;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(0);
    World world(&camera, false, 0);

    float move = 5;

    while (device.Running())
    {
        if (ABORT)
            break;

        float delta = device.GetFrameTime();

        Vec3 force(0.0f, 0.0f, 0.0f);

        if (Input::IsKeyDown(SDLK_q))
        {
            move = 5 * 20;
        }
        else
        {
            move = 5;
        }
        float moveForce = cameraSpeed * move; // Força de aceleração

        if (Input::IsKeyDown(SDLK_w))
        {
            force = force + camera.getFront() * moveForce;
        }
        else if (Input::IsKeyDown(SDLK_s))
        {
            force = force - camera.getFront() * moveForce;
        }

        if (Input::IsKeyDown(SDLK_a))
        {
            force = force - camera.getRight() * moveForce;
        }
        else if (Input::IsKeyDown(SDLK_d))
        {
            force = force + camera.getRight() * moveForce;
        }

        // if (Input::IsKeyDown(SDLK_SPACE) && camera.isOnGround())
        // {
        //     force = force + Vec3(0.0f, 5000.0f, 0.0f); // Pular
        // }

        camera.addForce(force, delta);
        camera.applyPhysics(delta);

        //  Vec3 playerPos = camera.getPosition();

        // if (collision.checkAndResolveCollision(camera))
        // {
        // }

        if (Input::IsMouseButtonDown(SDL_BUTTON_LEFT))
        {
            int x = Input::GetMouseDeltaX();
            int y = Input::GetMouseDeltaY();
            camera.rotate(x, -y, mouseSensitivity);
        }

        if (Input::IsKeyPressed(SDLK_e))
        {
            world.switchMode();
        }

        Driver::Instance().Clear();

        glViewport(0, 0, device.GetWidth(), device.GetHeight());

        Mat4 projection = camera.getProjectionMatrix();
        Mat4 view = camera.getViewMatrix();
        camera.setAspectRatio(device.GetWidth() / device.GetHeight());

        //  Mat4 viewProjectionMatrix = projection * view;

        Mat4 mvp = projection * view;

        camera.getFrustum()->update(mvp);

        // Render 3d world
        totalTime += delta;

   
  

        world.MapHandler();

        shaderVoxels.Use();
        Mat4 identity = Mat4::Identity();


        shaderVoxels.SetMatrix4("MVP", mvp.m);
        shaderVoxels.SetMatrix4("ModelMatrix", identity.m);

  

        Driver::Instance().SetDepthTest(true);
        Driver::Instance().SetDepthWrite(true);
        Driver::Instance().SetCullFace(true);
        Driver::Instance().SetBlend(false);

        shaderVoxels.SetInt("grass", 0);
        shaderVoxels.SetInt("stone", 1);
        shaderVoxels.SetInt("sand", 2);
        shaderVoxels.SetInt("water", 3);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, grass);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, stone);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, sand);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, water);

        glBindVertexArray(vaoID);
        world.LoadChunks();
        // bool found = false;

        // if (Input::IsMouseButtonDown(SDL_BUTTON_LEFT))
        // {
            
                
        //     RayPickResult result = world.getChunkAndVoxelFromScreen(&batch, device.GetWidth(), device.GetHeight(), Input::GetMouseX(), Input::GetMouseY());
        //     if (result.found)
        //     {
        //         printf("Voxel: %d, %d, %d\n", result.voxelX, result.voxelY, result.voxelZ);   
        //         found = true;
        
        //     }
        // }
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glBindVertexArray(0);

        Driver::Instance().SetCullFace(false);
        glDepthFunc(GL_LEQUAL);
        skyboxShader.Use();
        skyboxShader.SetMatrix4("view", view.m);
        skyboxShader.SetMatrix4("projection", projection.m);
        skyboxShader.SetInt("skybox", 0);
        skybox.Render(skyboxTex);
        glDepthFunc(GL_LESS);

      //  Driver::Instance().SetBlend(false);

    //    shader.Use();


    //     Driver::Instance().SetCullFace(false);
    //     Driver::Instance().SetBlend(false);

    //     shader.Use();
    //     shader.SetMatrix4("model", identity.m);
    //     shader.SetMatrix4("view", view.m);
    //     shader.SetMatrix4("projection", projection.m);

     
 
  
    //     batch.Render();
        // Render 2d STUFF

        projection = Mat4::Ortho(0.0f, device.GetWidth(), device.GetHeight(), 0.0f, -1.0f, 1.0f);
        Driver::Instance().SetBlend(true);
        Driver::Instance().SetBlendMode(BlendMode::BLEND);
        Driver::Instance().SetDepthTest(false);

        shader.Use();

        shader.SetMatrix4("model", identity.m);
        shader.SetMatrix4("view", identity.m);
        shader.SetMatrix4("projection", projection.m);
        batch.SetColor(55, 255, 55, 255);

        font.SetSize(20);
        batch.DrawRectangle(5, 5, 200, 120, Color::GRAY, true);
        batch.SetColor(55, 255, 55, 255);
        font.Print(10, 20, "FPS %d", device.GetFPS());
        font.Print(10, 40, "Voxels %d", world.getTotalVoxels());
        font.Print(10, 60, "Cuber %d", world.getTotalCubes());
        font.Print(10, 80, "Vertices %d", world.getTotalVertices());
        if (world.isCityMode())
        {
            font.Print(10, 100, "City Mode");
        }
        else
        {
            font.Print(10, 100, "Voxel Mode");
        }
        // if (found)
        // {
        //     font.Print(10, 100, "X: %d", result.voxelX);
        //     font.Print(10, 120, "Y: %d", result.voxelY);
        //     font.Print(10, 140, "Z: %d", result.voxelZ);
        // }


        batch.Render();

        device.Swap();
    }
    LogInfo("Shutting down.");

    shaderVoxels.Release();
    world.clean();

    font.Release();
    batch.Release();
    shader.Release();

    device.Close();
    return 0;
}