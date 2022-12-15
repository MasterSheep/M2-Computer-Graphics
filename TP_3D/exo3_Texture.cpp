#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>

#include <glimac/Program.hpp> 
#include <glimac/FilePath.hpp>
#include <glimac/glm.hpp>
#include <glimac/Sphere.hpp>
#include <glimac/common.hpp>
#include <glimac/Image.hpp>

using namespace glimac;

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

    Sphere sphere(1, 32, 16);  

    std::unique_ptr<Image> earth_image = loadImage("../GLImac-Template/assets/textures/EarthMap.jpg");
    std::unique_ptr<Image> moon_image  = loadImage("../GLImac-Template/assets/textures/MoonMap.jpg");

    const GLuint VERTEX_ATTR_POSITION  = 0;
    const GLuint VERTEX_ATTR_NORMAL    = 1;
    const GLuint VERTEX_ATTR_TEXCOORDS = 2;

    // Chargement des shaders
    FilePath applicationPath(argv[0]);
    Program program = loadProgram(applicationPath.dirPath() + "shaders/" + argv[1],
                                  applicationPath.dirPath() + "shaders/" + argv[2]);
    program.use();

    // Récupération des variables shader
    GLint uMVPMatrix = glGetUniformLocation(program.getGLId(), "uMVPMatrix");
    GLint uMVMatrix = glGetUniformLocation(program.getGLId(), "uMVMatrix");
    GLint uNormalMatrix = glGetUniformLocation(program.getGLId(), "uNormalMatrix");
    GLint uTexture = glGetUniformLocation(program.getGLId(), "uTexture");

    glEnable(GL_DEPTH_TEST);


    // Création de la texture
    GLuint texture;
    glGenTextures(1, &texture);
    // Bindind de la texture
    glBindTexture(GL_TEXTURE_2D, texture);
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
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)offsetof(ShapeVertex, position));
    glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)offsetof(ShapeVertex, normal));
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

        /*********************************
         * HERE SHOULD COME THE RENDERING CODE
         *********************************/

        // Nettoyage des buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Binding de la texture 
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(uTexture, 0);
        // Envoie de la texture de la terre
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, earth_image->getWidth(), earth_image->getHeight(), 0, GL_RGBA, GL_FLOAT, earth_image->getPixels());

        // Envoie des informations aux shaders
        glm::mat4 ProjMatrix   = glm::perspective(glm::radians(70.f), (width / height), 0.1f, 100.f); 
        glm::mat4 MVMatrix     = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5)); // Translation
        glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
                  MVMatrix     = glm::rotate(MVMatrix, windowManager.getTime(), glm::vec3(0, 1, 0)); // Translation * Rotation
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        // Binding du VAO 
        glBindVertexArray(vao);
        // Envoie des triangles, pour la shere principale
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        // Envoie de la texture de la lune
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, moon_image->getWidth(), moon_image->getHeight(), 0, GL_RGBA, GL_FLOAT, moon_image->getPixels());
        // Création de toutes les lunes
        for (int i = 0; i < Rotation.size(); ++i)
        {
            MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5));
            NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
            MVMatrix = glm::rotate(MVMatrix,    windowManager.getTime(), Rotation.at(i)); // Translation * Rotation
            MVMatrix = glm::translate(MVMatrix, Translation.at(i)); // Translation * Rotation * Translation
            MVMatrix = glm::scale(MVMatrix,     glm::vec3(0.2, 0.2, 0.2)); // Translation * Rotation * Translation * Scale
            glUniformMatrix4fv(uMVPMatrix,    1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
            glUniformMatrix4fv(uMVMatrix,     1, GL_FALSE, glm::value_ptr(MVMatrix));
            glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
            glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        }
        
        /*
        MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5)); // Translation
        MVMatrix = glm::rotate(MVMatrix, windowManager.getTime(), glm::vec3(0, 1, 0)); // Translation * Rotation
        MVMatrix = glm::translate(MVMatrix, glm::vec3(-2, 0, 0)); // Translation * Rotation * Translation
        MVMatrix = glm::scale(MVMatrix, glm::vec3(0.2, 0.2, 0.2)); // Translation * Rotation * Translation * Scale
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        */

        // Débindind du VAO
        glBindVertexArray(0);
        // Débindind de la texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &vao);

    return EXIT_SUCCESS;
}
