#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <iostream>

bool flag=false;
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// declaration functions

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void renderLightCube(unsigned int VAO, Shader shader,Camera camera, glm::vec3 pointLightPosition);
unsigned int initEBOBuffers();
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void renderQuad();
// camera
Camera camera(glm::vec3(2.0f, 0.0f, 6.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

bool hdr = true;
bool hdrKeyPressed = false;
bool bloom = true;
bool bloomKeyPressed = false;
float exposure = 1.0f;


// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // hocemo da crtamo u ovom prozoru
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // kad se velicina promeni(viewPort)
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // ucitavamo sve glad funkcije
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //skybox iz background
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("resources/shaders/lightShaderM.vs", "resources/shaders/lightShaderM.fs");

    Shader lightCubeShader("resources/shaders/lightCube.vs", "resources/shaders/lightCube.fs");

    Shader ourShader("resources/shaders/modelShader.vs", "resources/shaders/modelShader.fs");

    Shader skyboxShader("resources/shaders/skyboxShader.vs", "resources/shaders/skyboxShader.fs");

    Shader snowShader("resources/shaders/snowflakeShader.vs", "resources/shaders/snowflakeShader.fs");


    Shader skiShader(FileSystem::getPath("resources/shaders/chair.vs").c_str(),FileSystem::getPath("resources/shaders/chair.fs").c_str() );

    Shader bloomShader("resources/shaders/bloom.vs","resources/shaders/bloom.fs");

    Shader hdrShader("resources/shaders/hdr.vs","resources/shaders/hdr.fs");

    // models loading
    Model ourModel("resources/objects/santa/Santa.obj");
    Model ourModel1("resources/objects/ball/ball.obj");
    Model ourModel2("resources/objects/skis/10534_Snow_skis_v1_L3.obj");



    // set up vertex data (and buffer(s)) and configure vertex attributes  --gifts
    // ------------------------------------------------------------------
    float vertices[] = {
             // positions          // normals           // texture coords
            0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f, // bottom-left
            -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,-1.0f,  1.0f,  0.0f, // bottom-right
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f,  1.0f,  1.0f,// top-right
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,-1.0f,  1.0f,  1.0f, // top-right
            0.5f,  0.5f, -0.5f,  0.0f, 1.0f,-1.0f,  0.0f,  1.0f, // top-left
            0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f, // bottom-left


            // front face (CCW winding)
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f,// bottom-left
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,1.0f,  1.0f,  1.0f, // bottom-right
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f,  0.0f, // top-right
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,1.0f,  1.0f,  1.0f, // top-right
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f, // top-left
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,1.0f,  0.0f,  1.0f, // bottom-left


            // left face (CCW)
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  1.0f,  0.0f, // bottom-left
            -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,0.0f,  0.0f,  1.0f, // bottom-right
            -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0f,  1.0f, // top-right
            -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f,  1.0f,// top-right
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,// top-left
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,0.0f,  0.0f,  0.0f, // bottom-left


            // right face (CCW)
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  1.0f,  0.0f, // bottom-left
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,0.0f,  1.0f,  1.0f, // bottom-right
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,0.0f,  0.0f,  1.0f, // top-right
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,0.0f,  0.0f,  1.0f, // top-right
            0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f,  0.0f, // top-left
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,0.0f,  1.0f,  0.0f,
            // bottom-left

            // bottom face (CCW)
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,0.0f,  0.0f,  1.0f, // bottom-left
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,0.0f,  1.0f,  0.0f, // bottom-right
            0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0f,  1.0f,// top-right
            0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0f,  0.0f, // top-right
            -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f,  1.0f, // top-left
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f,  0.0f, // bottom-left

            // top face (CCW)
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,0.0f,  0.0f,  1.0f, // bottom-left
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,0.0f,  1.0f,  1.0f, // bottom-right
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,0.0f,  1.0f,  0.0f, // top-right
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,0.0f,  1.0f,  0.0f, // top-right
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,0.0f,  0.0f,  0.0f, // top-left
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f,  1.0f // bottom-left

    };

    // positions of gifts
    glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
            glm::vec3( -7.0f,  0.2f,  5.0f),
            glm::vec3( 2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };


    // snowflake rectangle
    float transparentVertices[] = {
            // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
            1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };

    vector<glm::vec3> snowflakePosition
            {
                    glm::vec3(-1.0f, 2.0f, -0.48f),
                    glm::vec3( 2.5f, 1.5f, 0.51f),
                    glm::vec3( 0.6f, 2.0f, 0.7f),
                    glm::vec3(1.3f, 3.0f, -2.3f),
                    glm::vec3 (4.5f, 1.5f, -0.6f),
                    glm::vec3(-2.0f, 3.0f, -0.48f),
                    glm::vec3( 2.5f, 3.0f, 0.51f),
                    glm::vec3( 0.6f, 1.0f, 0.7f),
                    glm::vec3(1.3f, 4.0f, -2.3f),
                    glm::vec3 (4.5f, 4.0f, -0.6f)
            };


    // cube's VAO (and VBO)

    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    //snowflake VAO
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    //skybox faces
    vector<std::string> faces
            {
                    "resources/textures/skybox/right.jpg",
                    "resources/textures/skybox/left.jpg",
                    "resources/textures/skybox/top.jpg",
                    "resources/textures/skybox/bottom.jpg",
                    "resources/textures/skybox/front.jpg",
                    "resources/textures/skybox/back.jpg"
            };
    unsigned int cubemapTexture = loadCubemap(faces);



   // definisanje svega sto treba za rad sa bloom i HDR
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ping-pong-framebuffer for blurring
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        // also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }




    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    unsigned int diffuseMap = loadTexture("resources/textures/gift.jpg");
    unsigned int specularMap = loadTexture("resources/textures/glitter.jpeg");
    unsigned int transparentTexture = loadTexture("resources/textures/snow.png");

    // lightningShader configuration
    // --------------------
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);

    ourModel2.SetShaderTextureNamePrefix("material.");

    ourModel.SetShaderTextureNamePrefix("material.");
    ourShader.use();
    ourShader.setInt("material.texture_diffuse1", 0);

    skiShader.use();
    skiShader.setInt("material.texture_diffuse1", 0);


    lightingShader.use();
    unsigned int VAO2 = initEBOBuffers();

    // snowShader configuration
    // --------------------
    snowShader.use();
    snowShader.setInt("texture1", 0);

    lightingShader.setBool("blin", false);


    bloomShader.use();
    bloomShader.setInt("image", 0);

    hdrShader.use();
    hdrShader.setInt("hdrBuffer", 0);
    hdrShader.setInt("bloomBlur", 1);

    float lin = 0.14f;
    float kvad = 0.07f;


    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.use();
        if(!flag) {
            lightingShader.setBool("blin", true);
        }
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);

        // directional light
        lightingShader.setVec3("dirLight.direction", -20.2f, -20.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

        // point light 1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032);

        // point light 2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.09);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032);
        // point light 3
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032);
        // point light 4
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", 0.09);
        lightingShader.setFloat("pointLights[3].quadratic", 0.032);

         // spotLight
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09);
        lightingShader.setFloat("spotLight.quadratic", 0.032);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);



        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // render gifts
        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0; i < 10; i++)
        {

            // calculate the model matrix for each object and pass it to shader before drawing

            // CHANGE CULLING
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);



            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            float scaleAmount=(sin(glfwGetTime())+1)/2+0.2;
            model = glm::scale(model,glm::vec3(scaleAmount,scaleAmount,scaleAmount));
            lightingShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glDisable(GL_CULL_FACE);



        // pointlights TODO model matrix
        lightCubeShader.use();
        lightCubeShader.setMat4("projection",projection);
        lightCubeShader.setMat4("view",view);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);

            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            lightCubeShader.setMat4("model", model);
            glm::mat4 model1 = glm::mat4(1.0f);
            model1 = glm::translate(model1, pointLightPositions[i]);

            model1= glm::scale(model1, glm::vec3(0.01f,0.01f,0.01f)); // Make it a smaller cube
            lightCubeShader.setMat4("model1", model1);
            ourModel1.Draw(lightCubeShader);
        }


        // santa claus
        ourShader.use();
        
        //setup uniforms
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);
        ourShader.setVec3("material.specular", glm::vec3(0.25f));

        // directional light
        ourShader.setVec3("dirLight.direction", 2.2f, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        ourShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[0].constant", 1.0f);
        ourShader.setFloat("pointLights[0].linear", 0.09);
        ourShader.setFloat("pointLights[0].quadratic", 0.032);
        // point light 2
        ourShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        ourShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[1].constant", 1.0f);
        ourShader.setFloat("pointLights[1].linear", 0.09);
        ourShader.setFloat("pointLights[1].quadratic", 0.032);
        // point light 3
        ourShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        ourShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[2].constant", 1.0f);
        ourShader.setFloat("pointLights[2].linear", 0.09);
        ourShader.setFloat("pointLights[2].quadratic", 0.032);
        // point light 4
        ourShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        ourShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[3].constant", 1.0f);
        ourShader.setFloat("pointLights[3].linear", 0.09);
        ourShader.setFloat("pointLights[3].quadratic", 0.032);
        // spotLight
        ourShader.setVec3("spotLight.position", camera.Position);
        ourShader.setVec3("spotLight.direction", camera.Front);
        ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("spotLight.constant", 1.0f);
        ourShader.setFloat("spotLight.linear", 0.09);
        ourShader.setFloat("spotLight.quadratic", 0.032);
        ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));



        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        model=glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(7.5f, -4.70f, -10.0f)); // translate it down so it's at the center of the scene
        model=glm::rotate(model,glm::radians(-90.0f),glm::vec3(1.0f,0.0f,0.0f));
        model=glm::rotate(model,glm::radians(-60.0f),glm::vec3(0.0f,0.0f,1.0f));
        model=glm::rotate(model,glm::radians(-20.0f),glm::vec3(0.0f,1.0f,0.0f));
        model = glm::scale(model, glm::vec3(0.06f));	// it's a bit too big for our scene, so scale it down

        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);


        // chair 
        skiShader.use();
        
        //setup uniforms
        skiShader.setVec3("viewPos", camera.Position);
        skiShader.setFloat("material.shininess", 32.0f);
        skiShader.setVec3("material.specular", glm::vec3(0.2f));

        // directional light
        skiShader.setVec3("dirLight.direction", 2.2f, -1.0f, -0.3f);
        skiShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        skiShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        skiShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        skiShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        skiShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        skiShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        skiShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        skiShader.setFloat("pointLights[0].constant", 1.0f);
        skiShader.setFloat("pointLights[0].linear", 0.09);
        skiShader.setFloat("pointLights[0].quadratic", 0.032);
        // point light 2
        skiShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        skiShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        skiShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        skiShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        skiShader.setFloat("pointLights[1].constant", 1.0f);
        skiShader.setFloat("pointLights[1].linear", 0.09);
        skiShader.setFloat("pointLights[1].quadratic", 0.032);
        // point light 3
        skiShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        skiShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        skiShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        skiShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        skiShader.setFloat("pointLights[2].constant", 1.0f);
        skiShader.setFloat("pointLights[2].linear", 0.09);
        skiShader.setFloat("pointLights[2].quadratic", 0.032);
        // point light 4
        skiShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        skiShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        skiShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        skiShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        skiShader.setFloat("pointLights[3].constant", 1.0f);
        skiShader.setFloat("pointLights[3].linear", 0.09);
        skiShader.setFloat("pointLights[3].quadratic", 0.032);
        // spotLight
        skiShader.setVec3("spotLight.position", camera.Position);
        skiShader.setVec3("spotLight.direction", camera.Front);
        skiShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        skiShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        skiShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        skiShader.setFloat("spotLight.constant", 1.0f);
        skiShader.setFloat("spotLight.linear", 0.09);
        skiShader.setFloat("spotLight.quadratic", 0.032);
        skiShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        skiShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        skiShader.setMat4("projection", projection);
        skiShader.setMat4("view", view);

        auto modelski= glm::mat4(2.0f);
        modelski= glm::translate(modelski, glm::vec3(3.7f, -2.5f, -5.0f));
        modelski=glm::rotate(modelski, glm::radians(65.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelski=glm::rotate(modelski, glm::radians(100.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelski=glm::rotate(modelski, glm::radians(-44.5f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelski = glm::scale(modelski, glm::vec3(0.04f));

        skiShader.setMat4("model", modelski);
        ourModel2.Draw(skiShader);

        // using blanding for snowflakes- discard techinque
        snowShader.use();
        snowShader.setMat4("projection", projection);
        snowShader.setMat4("view", view);



        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);
        for (unsigned int i = 0; i < snowflakePosition.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, snowflakePosition[i]);
            float angle=10*sin(glfwGetTime())*3;
            model=glm::rotate(model,glm::radians(angle),glm::vec3(1.0f,0.3f,0.5f));
            model=glm::scale(model,glm::vec3(0.5f,0.5f,0.5f));
            snowShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }




        //drow  skybox
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        std::cout << (flag ? "Blinn-Phong" : "Phong") << std::endl;


        //*********************************************
        //load pingpong
        bool horizontal = true, first_iteration = true;
        unsigned int amount = 10;
        bloomShader.use();
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            bloomShader.setInt("horizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);

            renderQuad();

            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // **********************************************
        // load hdr
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        hdrShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        hdrShader.setBool("hdr", hdr);
        hdrShader.setBool("bloom", bloom);
        hdrShader.setFloat("exposure", exposure);
        renderQuad();


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window); //2 bafera, trenutni salje na crtanje nakon cega ga prazni
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteBuffers(1,&VAO2);
    glDeleteBuffers(1,&skyboxVAO);
    glDeleteBuffers(1,&transparentVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1,&skyboxVBO);
    glDeleteBuffers(1,&transparentVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

//------------------------------------------------------------------------------------------------------------------
// definition of functions

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);



    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && !hdrKeyPressed)
    {
        hdr = !hdr;
        hdrKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE)
    {
        hdrKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && !bloomKeyPressed)
    {
        bloom = !bloom;
        bloomKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE)
    {
        bloomKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (exposure > 0.0f)
            exposure -= 0.005f;
        else
            exposure = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        exposure += 0.005f;
    }

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height); // menja prostor u kom renderujemo
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int initEBOBuffers(){
    float vertices[] = {
            -0.5f, -0.5f, -0.5f, // levo dole nazad
            0.5f, -0.5f, -0.5f, //desno dole nazad
            -0.5f, 0.5f, -0.5f, // levo gore nazad
            0.5f, 0.5f, -0.5f, // desno gore nazad
            -0.5f, -0.5f, 0.5f, // levo dole napred
            0.5f, -0.5f, 0.5f, // desno dole napred
            -0.5f, 0.5f, 0.5f, // levo gore napred
            0.5f, 0.5f, 0.5f //desno gore napred
    };

    unsigned int indices[] = {
            0, 1, 2, //iza
            1, 2, 3,

            4, 5, 6,//ispred
            5, 6, 7,

            0, 4, 2,//levo
            4, 2, 6,

            1, 5, 3,//desno
            5, 3, 7,

            2, 3, 6, //gore
            3, 6, 7,

            0, 1, 4,//dole
            1, 4, 5
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    return VAO;
}

void renderLightCube(unsigned int VAO, Shader shader,Camera camera, glm::vec3 pointLightPosition){

    shader.use();

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader.setMat4("projection", projection);

    glm::mat4 view = camera.GetViewMatrix();
    shader.setMat4("view", view);

    glm::mat4 model = glm::mat4(1.f);
    model = glm::mat4(1.f);
    model = glm::translate(model, pointLightPosition);
    model = glm::scale(model, glm::vec3(0.1f));

    shader.setMat4("model", model);
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}
// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            std::cout << "Faild to load cube map: " << faces[i] << std::endl;
        }
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(key==GLFW_KEY_B && action==GLFW_PRESS) {
        if(flag) {
            flag=false;
        }
        else {
            flag=true;
        }
    }


}


unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{

    if (quadVAO == 0)
    {
        float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
