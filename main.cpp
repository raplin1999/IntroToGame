#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix,modelMatrix2,modelMatrix3, projectionMatrix;

float player_speed = 3.0f;
float ball_speed = 2.0f;
glm::vec3 player_position = glm::vec3(-4.5, 0, 0);

glm::vec3 player_movement = glm::vec3(0, 0, 0);

glm::vec3 player2_position = glm::vec3(4.5, 0, 0);

glm::vec3 player2_movement = glm::vec3(0, 0, 0);

glm::vec3 ball_position = glm::vec3(0, 0, 0);

glm::vec3 ball_movement = glm::vec3(-1, -0.7, 0);

GLuint playerTextureID;
GLuint DoraAmeon;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    modelMatrix2 = glm::mat4(1.0f);
    modelMatrix3 = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    playerTextureID = LoadTexture("ctg.png");
    DoraAmeon = LoadTexture("doramon.png");
    
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ProcessInput() {

    player_movement = glm::vec3(0);
    player2_movement = glm::vec3(0);
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym){

                    case SDLK_LEFT:
                        break;

                    case SDLK_RIGHT:
                        break;

                    case SDLK_SPACE:
                        break;
                }
                break;
        }
    }
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_W]) {
        player_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_S]) {
        player_movement.y = -1.0f;
    }
    if (keys[SDL_SCANCODE_UP]) {
        player2_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        player2_movement.y = -1.0f;
    }
}

bool isColloide(float x1, float x2, float w1,  float w2, float y1, float y2, float h1, float h2) {
    float xdist = fabs(x2 - x1) - ((w1 + w2) / 2.0f);
    float ydist = fabs(y2 - y1) - ((h1 + h2) / 2.0f);
    if (xdist < 0 && ydist < 0) {
        return true;
    }
    return false;
}


float lastTicks = 0.0f;
void Update() { 
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    player_position += player_movement * player_speed * deltaTime;
    player2_position += player2_movement * player_speed * deltaTime;
    ball_position += ball_movement * ball_speed * deltaTime;


    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, player_position);
    if (player_position.y>3.2 || player_position.y<-3.2) {
        player_position-= player_movement * player_speed * deltaTime;
        modelMatrix = glm::translate(modelMatrix, player_position);
    }

    modelMatrix2 = glm::mat4(1.0f);
    modelMatrix2 = glm::translate(modelMatrix2, player2_position);
    if (player2_position.y > 3.2 || player2_position.y < -3.2) {
        player2_position -= player2_movement * player_speed * deltaTime;
        modelMatrix2 = glm::translate(modelMatrix2, player2_position);
    }



    modelMatrix3 = glm::mat4(1.0f);
    if (ball_position.y < -3.2) {
        ball_movement.y = -ball_movement.y;
    }
    else if (ball_position.y > 3.2) {
        ball_movement.y = -ball_movement.y;
    }

    if (isColloide(ball_position.x, player_position.x, 0.4, 1, ball_position.y, player_position.y, 0.4, 2)) {
        ball_movement.x = -ball_movement.x;
    }

    if (ball_position.x > 5) {
        ball_movement.x = -ball_movement.x;
    }
    else if (ball_position.x < -5) {
        ball_movement.x = -ball_movement.x;
    }

    if (isColloide(ball_position.x, player2_position.x, 0.4, 1, ball_position.y, player2_position.y, 0.4, 2)) {
        ball_movement.x = -ball_movement.x;
    }

    if (ball_position.x > 4.8) {
        gameIsRunning = false;
    }
    else if (ball_position.x < -4.8) {
        gameIsRunning = false;
    }
    modelMatrix3 = glm::translate(modelMatrix3, ball_position);


    //modelMatrix3 = glm::translate(modelMatrix3, ball_position);
    //modelMatrix = glm::translate(modelMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
    //modelMatrix = glm::rotate(modelMatrix, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //modelMatrix = glm::scale(modelMatrix,  glm::vec3(1.0f, 0.0f, 0.0f));
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);


    float vertices[] = { -0.5, -1, 0.5, -1, 0.5, 1, -0.5, -1, 0.5, 1, -0.5, 1 };
    float vertices2[] = { -0.5, -1, 0.5, -1, 0.5, 1, -0.5, -1, 0.5, 1, -0.5, 1 };
    float vertices3[] = { -0.2, -0.2, 0.2, -0.2, 0.2, 0.2, -0.2, -0.2, 0.2, 0.2, -0.2, 0.2};
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    
    program.SetModelMatrix(modelMatrix);
    glBindTexture(GL_TEXTURE_2D, playerTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
    program.SetModelMatrix(modelMatrix3);
    glBindTexture(GL_TEXTURE_2D, DoraAmeon);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
    program.SetModelMatrix(modelMatrix2);
    glBindTexture(GL_TEXTURE_2D, playerTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}