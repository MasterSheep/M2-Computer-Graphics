#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>

#include <glimac/Program.hpp> 
#include <glimac/FilePath.hpp>

#include <glimac/glm.hpp>

using namespace glimac;

struct Vertex2DUV {
    glm::vec2 position;
    glm::vec2 texture;

    Vertex2DUV() {}

    Vertex2DUV(glm::vec2 _position, glm::vec2 _texture)
        : position { _position }
        , texture { _texture }
    {}
};


int main(int argc, char** argv) {
    // Initialize SDL and open a window
    SDLWindowManager windowManager(800, 600, "GLImac");

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if(GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    FilePath applicationPath(argv[0]);
    Program program = loadProgram(applicationPath.dirPath() + "shaders/" + argv[1],
                                  applicationPath.dirPath() + "shaders/" + argv[2]);
    program.use();

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/

    GLuint vbo, vao;
    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_ATTR_COULEUR = 1;

    glGenBuffers(1, &vbo);

    //
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    Vertex2DUV vertices[] = { 
        Vertex2DUV(glm::vec2(-1.,  1.), glm::vec2(0, 0)),
        Vertex2DUV(glm::vec2( 1., -1.), glm::vec2(0, 0)),
        Vertex2DUV(glm::vec2( 0.,  1.), glm::vec2(0, 0))
    };

    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vertex2DUV), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
 
    //////

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_COULEUR);

    //
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DUV), offsetof(Vertex2DUV, position));
    glVertexAttribPointer(VERTEX_ATTR_COULEUR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2DUV), (const GLvoid*) offsetof(Vertex2DUV, texture));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //

    glBindVertexArray(0);

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
        glBindVertexArray(vao);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }

    return EXIT_SUCCESS;
}
