#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>

using namespace glimac;

int main(int argc, char** argv) {
    // Initialize SDL and open a window
    SDLWindowManager windowManager(800, 600, "GLImac");

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
    // Création de 1 VBO
    GLuint vbo, vao;
    const GLuint VERTEX_ATTR_POSITION = 0;


    glGenBuffers(1, &vbo);

    //
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    GLfloat vertices[] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f };
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //

    //////

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);

    //
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    glBindBuffer(0, vbo);
    //

    glBindBuffer(GL_ARRAY_BUFFER, 0);

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

        // DESSIN
        glBindVertexArray(vao);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }

    // NETTOYAGE
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao); 

    return EXIT_SUCCESS;
}
