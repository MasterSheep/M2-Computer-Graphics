#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>
#include <glimac/Program.hpp> 
#include <glimac/FilePath.hpp>
#include <glimac/glm.hpp>
#include <glimac/Sphere.hpp>
#include <glimac/common.hpp>
#include <glimac/Image.hpp>
#include <glimac/TrackballCamera.hpp>
#include <iterator>


using namespace glimac;

struct EarthProgram {
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uEarthTexture;
    GLint uCloudTexture;

    EarthProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "shaders/multiTex3D.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uEarthTexture = glGetUniformLocation(m_Program.getGLId(), "uTexture");
        uCloudTexture = glGetUniformLocation(m_Program.getGLId(), "uTexture2");
    }
};

struct MoonProgram {
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uTexture;

    MoonProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "shaders/tex3D.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uTexture = glGetUniformLocation(m_Program.getGLId(), "uTexture");
    }
};

int main(int argc, char** argv) {
    // Initialize SDL and open a window
    float width = 800., height = 600.;
    SDLWindowManager windowManager(width, height, "GLImac");

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if(GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/

    TrackballCamera Camera;
    int moveSpeed = 0.1;

    Sphere sphere(1, 32, 16); 

    std::unique_ptr<Image> earth_image = loadImage("../GLImac-Template/assets/textures/EarthMap.jpg");
    std::unique_ptr<Image> moon_image  = loadImage("../GLImac-Template/assets/textures/MoonMap.jpg");
    std::unique_ptr<Image> cloud_image  = loadImage("../GLImac-Template/assets/textures/CloudMap.jpg");

    const GLuint VERTEX_ATTR_POSITION  = 0;
    const GLuint VERTEX_ATTR_NORMAL    = 1;
    const GLuint VERTEX_ATTR_TEXCOORDS = 2;

    // Chargement des shaders
    FilePath applicationPath(argv[0]);
    EarthProgram earthProgram(applicationPath);
    MoonProgram moonProgram(applicationPath);


    glEnable(GL_DEPTH_TEST);

    // Création de des textures
    GLuint texture_earth, texture_moon, texture_cloud;

    glGenTextures(1, &texture_earth);
    // Bindind de la texture de la terre
    glBindTexture(GL_TEXTURE_2D, texture_earth);
    // Envoie de la texture de la terre
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, earth_image->getWidth(), earth_image->getHeight(), 0, GL_RGBA, GL_FLOAT, earth_image->getPixels());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    // Débindind de la texture
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &texture_moon);
    // Bindind de la texture de la lune
    glBindTexture(GL_TEXTURE_2D, texture_moon);
    // Envoie de la texture de la lune
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, moon_image->getWidth(), moon_image->getHeight(), 0, GL_RGBA, GL_FLOAT, moon_image->getPixels());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    // Débindind de la texture
    glBindTexture(GL_TEXTURE_2D, 0);


    glGenTextures(1, &texture_cloud);
    // Bindind de la texture des nuages
    glBindTexture(GL_TEXTURE_2D, texture_cloud);
    // Envoie de la texture des nuages
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cloud_image->getWidth(), cloud_image->getHeight(), 0, GL_RGBA, GL_FLOAT, cloud_image->getPixels());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    // Débindind de la texture
    glBindTexture(GL_TEXTURE_2D, 0);


    // Création d'un VBO
    GLuint vbo;
    glGenBuffers(1, &vbo);
    // Bindind du VBO, Envoie des données, Débindind du VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sphere.getVertexCount()*sizeof(ShapeVertex), sphere.getDataPointer(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Création du VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    // Binding du VAO 
    glBindVertexArray(vao);
    // Précision de l'utilisation du VAO
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
    glEnableVertexAttribArray(VERTEX_ATTR_TEXCOORDS);

    // Bindind du VBO, Spécification des différents formats, Débinding du VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(VERTEX_ATTR_POSITION,  3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)offsetof(ShapeVertex, position));
    glVertexAttribPointer(VERTEX_ATTR_NORMAL,    3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)offsetof(ShapeVertex, normal));
    glVertexAttribPointer(VERTEX_ATTR_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)offsetof(ShapeVertex, texCoords));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Débindind du VAO
    glBindVertexArray(0);

    std::vector<glm::vec3> Rotation, Translation;
    for (int i = 0; i < 32; ++i)
    { 
        Rotation.emplace_back(glm::sphericalRand(1.0f));
        Translation.emplace_back(glm::sphericalRand(2.0f));
    }

    // Application loop:
    bool done = false;
    while(!done) {
        // Event loop:
        SDL_Event e;
        while(windowManager.pollEvent(e)) {
            if(e.type == SDL_QUIT) {
                done = true; // Leave the loop after this iteration
            }
        }

        // Récupérations des informations sur la souris
        if (windowManager.isMouseButtonPressed(SDL_BUTTON_MIDDLE))
        {
            //float delta = std::distance(glm::vec2(windowManager.getMousePosition()), glm::vec2(400.f, 300.f)) >= 0 ? .005f : -.005f;
            Camera.moveFront(0.1);
        }

        if (windowManager.isMouseButtonPressed(SDL_BUTTON_RIGHT))
        {
            auto angles = (glm::vec2(windowManager.getMousePosition()) - glm::vec2(400.f, 300.f));
            Camera.rotateLeft(angles.x < 0 ? moveSpeed * windowManager.getTime() : angles.x > 0 ? -moveSpeed * windowManager.getTime()
                                                                                                : 0.f);
            Camera.rotateUp(angles.y < 0 ? moveSpeed * windowManager.getTime() : angles.y > 0 ? -moveSpeed * windowManager.getTime()
                                                                                              : 0.f);
        }
        /*********************************
         * HERE SHOULD COME THE RENDERING CODE
         *********************************/

        // Nettoyage des buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        earthProgram.m_Program.use();
        glUniform1i(earthProgram.uEarthTexture, 0);
        glUniform1i(earthProgram.uCloudTexture, 1);

        // Envoie des informations aux shaders
        glm::mat4 globalMVMatrix = Camera.getViewMatrix(); //glm::translate(glm::mat4(1), glm::vec3(0, 0, -5)); // Translation
        glm::mat4 projMatrix     = glm::perspective(glm::radians(70.f), (width / height), 0.1f, 100.f); 
        glm::mat4 earthMVMatrix  = glm::rotate(globalMVMatrix, windowManager.getTime(), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(earthProgram.uMVMatrix,     1, GL_FALSE, glm::value_ptr(earthMVMatrix));
        glUniformMatrix4fv(earthProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(earthMVMatrix))));
        glUniformMatrix4fv(earthProgram.uMVPMatrix,    1, GL_FALSE, glm::value_ptr(projMatrix * earthMVMatrix));

        // Binding des textures 
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_earth); // la texture texture_earth est bindée sur l'unité GL_TEXTURE0
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_cloud); // la texture texture_cloud est bindée sur l'unité GL_TEXTURE1

        // Binding du VAO 
        glBindVertexArray(vao);
        // Envoie des triangles, pour la shere principale
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());

        // Débindind de la deuxième textures
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0); // débind sur l'unité GL_TEXTURE1

        // Binding des textures 
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_moon); // la texture texture_moon est bindée sur l'unité GL_TEXTURE0

        moonProgram.m_Program.use();
        // Création de toutes les lunes
        for (int i = 0; i < Rotation.size(); ++i)
        {
            glm::mat4 moonMVMatrix = glm::rotate(globalMVMatrix,  windowManager.getTime(), Rotation.at(i)); // Translation * Rotation
                      moonMVMatrix = glm::translate(moonMVMatrix, Translation.at(i)); // Translation * Rotation * Translation
                      moonMVMatrix = glm::scale(moonMVMatrix,     glm::vec3(0.2, 0.2, 0.2)); // Translation * Rotation * Translation * Scale
            glm::mat4 moonNormalMatrix = glm::transpose(glm::inverse(moonMVMatrix));
            glUniformMatrix4fv(moonProgram.uMVPMatrix,    1, GL_FALSE, glm::value_ptr(projMatrix * moonMVMatrix));
            glUniformMatrix4fv(moonProgram.uMVMatrix,     1, GL_FALSE, glm::value_ptr(moonMVMatrix));
            glUniformMatrix4fv(moonProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(moonNormalMatrix));
            glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        }

        // Débindind du VAO
        glBindVertexArray(0);
        // Débindind de la première textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0); // débind sur l'unité GL_TEXTURE0     

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &texture_earth);
    glDeleteTextures(1, &texture_moon);
    glDeleteTextures(1, &texture_cloud);
    glDeleteVertexArrays(1, &vao);

    return EXIT_SUCCESS;
}
