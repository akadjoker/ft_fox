
#include "Core.hpp"
#include "ChunkMesh.hpp"
#include "Chunk.hpp"
#include "World.hpp"

#include "PlayerPhysics.hpp"

extern "C" const char *__lsan_default_suppressions()
{
    return "leak:libSDL2\n"
           "leak:SDL_DBus\n";
}

int main(int argc, char *argv[])
{
    Device device;

    bool ABORT = false;

    if (!device.Init("FT_VOX BY Luis Santos AKA DJOKER", 1024, 720))
    {
        return 1;
    }

    RenderBatch batch;
    batch.Init(1, 1024);
    Shader shader;
    Shader shaderVoxels;
    Shader cloudShader;

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
        // Vertex Shader
        const char *vShader = GLSL(
            layout(location = 0) in vec3 aPos;
            layout(location = 1) in vec2 aTexCoord;
            layout(location = 2) in vec3 aNormal;

            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;

            out vec3 Normal;
            out vec2 TexCoord;
            out vec3 FragPos;
            out vec3 ViewPos;
            out float height;
            out float visibility;

            void main() {
                FragPos = vec3(model * vec4(aPos, 1.0));
                ViewPos = vec3(view * vec4(FragPos, 1.0));
                Normal = mat3(transpose(inverse(model))) * aNormal;
                height = aPos.y; // Para gradiente de altura

                // Fog calculation
                float distance = length(ViewPos);
                visibility = clamp(exp(-pow((distance / 500.0), 1.5)), 0.0, 1.0);

                gl_Position = projection * view * vec4(FragPos, 1.0);
                TexCoord = aTexCoord;
            });

        const char *fShader = GLSL(
            in vec2 TexCoord;
            in vec3 Normal;
            in vec3 FragPos;
            in vec3 ViewPos;
            in float height;
            in float visibility;

            out vec4 FragColor;

            uniform sampler2D texture0;
            uniform vec3 lightDir = normalize(vec3(-0.5, -1.0, -0.3));
            uniform vec3 lightColor = vec3(1.0, 1.0, 0.9);
            uniform float time;

            const float ambientStrength = 0.4;
            const float diffuseStrength = 0.7;
            const float specularStrength = 0.3;
            const float shininess = 32.0;

            vec3 calculateSkyColor() {
                // Usa time para criar um ciclo dia/noite suave
                float dayNightCycle = (sin(time * 0.1) + 1.0) * 0.5;
                vec3 dayColor = vec3(0.3, 0.5, 0.7);
                vec3 nightColor = vec3(0.05, 0.1, 0.2);
                return mix(nightColor, dayColor, dayNightCycle);
            }

            void main() {
                vec3 norm = normalize(Normal);
                vec3 viewDir = normalize(-ViewPos);

                // Luz dinâmica baseada no tempo
                vec3 dynamicLightDir = normalize(vec3(
                    lightDir.x * cos(time * 0.5),
                    lightDir.y,
                    lightDir.z * sin(time * 0.5)));

                // Textura base
                vec4 texColor = texture(texture0, TexCoord);

                // Ambiente com ciclo dia/noite
                float dayNightCycle = (sin(time * 0.1) + 1.0) * 0.5;
                float dynamicAmbient = mix(0.2, ambientStrength, dayNightCycle);
                vec3 ambient = dynamicAmbient * lightColor;

                // Difuso com luz dinâmica
                float diff = max(dot(norm, -dynamicLightDir), 0.0);
                vec3 diffuse = diff * lightColor * diffuseStrength;

                // Especular
                vec3 reflectDir = reflect(dynamicLightDir, norm);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
                vec3 specular = specularStrength * spec * lightColor;

                // Gradiente de altura com ondulação suave
                float heightWave = sin(height * 0.5 + time) * 0.1;
                float heightFactor = clamp((height + heightWave) / 100.0, 0.0, 1.0);
                vec3 heightColor = mix(vec3(0.8, 0.8, 0.8), vec3(1.0, 1.0, 1.0), heightFactor);

                // Efeito de borda
                float edge = 1.0 - max(dot(viewDir, norm), 0.0);
                vec3 edgeColor = vec3(0.2, 0.2, 0.2) * pow(edge, 3.0);

                // Combinação final
                vec3 lighting = ambient + diffuse + specular + edgeColor;
                vec3 finalColor = texColor.rgb * lighting * heightColor;

                // Atmosfera e fog
                vec3 skyColor = calculateSkyColor();
                FragColor = mix(vec4(skyColor, 1.0), vec4(finalColor, texColor.a), visibility);

                gl_FragDepth = gl_FragCoord.z + 0.0001;
            });

        if (!shaderVoxels.Create(vShader, fShader))
        {
            ABORT = true;
        }
        shaderVoxels.LoadDefaults();
    }

    {
        // Vertex Shader
        const char *cloudVShader = GLSL(
            layout(location = 0) in vec3 aPos;
            layout(location = 1) in vec3 aInstancePos;
     

            uniform mat4 view;
            uniform mat4 projection;
            uniform float time;
           

            void main() 
            {
                  vec3 windDirection = vec3(1.0, 0.0, 0.5); // Direção do vento (X, Y, Z)
                vec3 offset = windDirection * time * 0.6; // Velocidade controlada por time
                vec3 pos = aPos + aInstancePos + offset;  // Deslocamento aplicado às instâncias
            

                 // vec3 pos = aPos + aInstancePos;
                 gl_Position = projection * view * vec4(pos, 1.0);
            });

        // Fragment Shader
        const char *cloudFShader = GLSL(
           out vec4 FragColor;

                void main()
                {
                    FragColor = vec4(1.0, 1.0, 1.0, 1.0); // Cor branca para as nuvens
                
                });

        if (!cloudShader.Create(cloudVShader, cloudFShader))
        {
            ABORT = true;
        }
        cloudShader.LoadDefaults();
    }

    font.LoadDefaultFont();
    font.SetBatch(&batch);
    font.SetSize(12);

    Camera camera(800.0f / 600.0f);
    camera.setPosition(0.0f, 80.0f, -20.0f);

    Driver::Instance().SetClearColor(0.2f, 0.3f, 0.3f, 1.0);

    float cameraSpeed = 5.5f;
    float mouseSensitivity = 0.1f;

    Texture2D texture;

    if (!texture.Load("assets/Blocks.png"))
    {
        unsigned char data[4] = {255, 255, 255, 255};
        texture.LoadFromMemory(data, 4, 1, 1);
    }

    texture.SetAnisotropicFiltering(8.0f);
    texture.SetWrapS(WrapMode::Repeat);
    texture.SetWrapT(WrapMode::Repeat);
    texture.SetMinFilter(FilterMode::Nearest);
    texture.SetMagFilter(FilterMode::Nearest);

    World world(12345); // seed para o noise
    // world.generateInitialChunks();  // gera 9 chunks (3x3)

    Frustum frustum;

    bool isDebug = false;

    CollisionSystem collision(&world);

    CloudSystem clouds(12345);
    float totalTime = 0.0f;

    clouds.update(camera.getPosition());


    while (device.Running())
    {
        if (ABORT)
            break;

        float delta = device.GetFrameTime();

        Vec3 force(0.0f, 0.0f, 0.0f);
        float moveForce = cameraSpeed * 100.0f; // Força de aceleração

        if (Input::IsKeyPressed(SDLK_f))
        {
            camera.swicthFly();
        }

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

        if (Input::IsKeyDown(SDLK_SPACE) && camera.isOnGround())
        {
            force = force + Vec3(0.0f, 5000.0f, 0.0f); // Pular
        }

        camera.addForce(force, delta);
        camera.applyPhysics(delta);

        Vec3 playerPos = camera.getPosition();

        // if (collision.checkAndResolveCollision(camera))
        // {
        // }

        if (Input::IsMouseButtonDown(SDL_BUTTON_LEFT))
        {
            int x = Input::GetMouseDeltaX();
            int y = Input::GetMouseDeltaY();
            camera.rotate(x, -y, mouseSensitivity);
        }

        if (Input::IsKeyPressed(SDLK_b))
        {
            isDebug = !isDebug;
        }

        Driver::Instance().Clear();

        glViewport(0, 0, device.GetWidth(), device.GetHeight());

        Mat4 projection = camera.getProjectionMatrix();
        Mat4 view = camera.getViewMatrix();
        camera.setAspectRatio(device.GetWidth() / device.GetHeight());

        Mat4 viewProjectionMatrix = projection * view;
        frustum.update(viewProjectionMatrix);

        Mat4 identity = Mat4::Identity();

        // Render 3d world
        totalTime += delta;
//        Driver::Instance().SetDepthWrite(false);
        Driver::Instance().SetBlend(true);
        Driver::Instance().SetBlendMode(BlendMode::BLEND);
        Driver::Instance().SetDepthTest(true);


        shaderVoxels.Use();
        shaderVoxels.SetMatrix4("model", identity.m);
        shaderVoxels.SetMatrix4("view", view.m);
        shaderVoxels.SetMatrix4("projection", projection.m);
        shaderVoxels.SetFloat("time", (float)SDL_GetTicks() / 1000.0f);

        texture.Use(0);

        Driver::Instance().SetDepthWrite(true);
        Driver::Instance().SetCullFace(true);
        world.update(playerPos);
        world.render(playerPos, shaderVoxels, frustum);


        cloudShader.Use();
        cloudShader.SetMatrix4("view", view.m);
        cloudShader.SetMatrix4("projection", projection.m);
        cloudShader.SetFloat("time", (float)SDL_GetTicks() / 1000.0f);
        clouds.update(playerPos);
        clouds.render();



        Driver::Instance().SetCullFace(false);
        Driver::Instance().SetBlend(false);

        shader.Use();
        shader.SetMatrix4("model", identity.m);
        shader.SetMatrix4("view", view.m);
        shader.SetMatrix4("projection", projection.m);

        shader.SetMatrix4("model", identity.m);
        batch.SetColor(255, 255, 255, 255);
        if (isDebug)
            world.debug(batch, frustum);
        batch.Grid(10, 10);
        batch.Render();

        // Render 2d STUFF

        projection = Mat4::Ortho(0.0f, device.GetWidth(), device.GetHeight(), 0.0f, -1.0f, 1.0f);
        Driver::Instance().SetBlend(true);
        Driver::Instance().SetBlendMode(BlendMode::BLEND);
        Driver::Instance().SetDepthTest(false);

        shader.Use();
        shader.SetMatrix4("model", identity.m);
        shader.SetMatrix4("view", identity.m);
        shader.SetMatrix4("projection", projection.m);
        batch.SetColor(255, 255, 255, 255);

        font.SetSize(20);
        batch.DrawRectangle(5, 5, 200, 100, Color::BLACK, true);
        batch.SetColor(255, 255, 255, 255);
        font.Print(10, 20, "FPS %d", device.GetFPS());
        font.Print(10, 40, "Voxels %d", world.getTotalVoxels());
        font.Print(10, 60, "Chunks %d", world.getTotalChunks());
        font.Print(10, 80, "Triangles %d", world.getTotalTris());

        batch.Render();

        device.Swap();
    }

    world.close();
    clouds.destroy();
    texture.Release();
    shaderVoxels.Release();
    cloudShader.Release();
    font.Release();
    batch.Release();
    shader.Release();
    while (world.IsRunning())
    {
        SDL_Delay(1);
    }
    LogInfo("Shutting down.");
    device.Close();
    return 0;
}