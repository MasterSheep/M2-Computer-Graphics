#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>

#include <glimac/Program.hpp> 
#include <glimac/FilePath.hpp>

#include <glimac/glm.hpp>
#include <vector>

//#include <cstddef>

using namespace glimac;

struct Vertex2DColor {
    glm::vec2 position;
    glm::vec3 color;

    Vertex2DColor() {}

    Vertex2DColor(glm::vec2 _position, glm::vec3 _color)
        : position { _position }
        , color { _color }
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
    
    Program program = loadProgram(applicationPath.dirPath() + "shaders/triangle.vs.glsl",
                                  applicationPath.dirPath() + "shaders/triangle.fs.glsl");
    program.use();

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/

    GLuint vbo, vao;
    const GLuint VERTEX_ATTR_POSITION = 3;
    const GLuint VERTEX_ATTR_COULEUR = 8;
    const unsigned int nb_triangle = 100;
    const float rayon = 0.75;

    glGenBuffers(1, &vbo);

    //
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    std::vector<Vertex2DColor> vertices;

    for (int i = 0; i < (nb_triangle + 2); i++) {
        vertices.emplace_back( Vertex2DColor( glm::vec2( rayon * glm::cos(i *  ((2 * glm::pi<float>()) / (nb_triangle + 2))),
                                                         rayon * glm::sin(i *  ((2 * glm::pi<float>()) / (nb_triangle + 2)))), 
                                              glm::vec3(1, 1 / (i + 1), 1 / (i + 1))));
    }

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2DColor), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //

    //////

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_COULEUR);

    //
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), offsetof(Vertex2DColor, position));
    glVertexAttribPointer(VERTEX_ATTR_COULEUR, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const GLvoid*) offsetof(Vertex2DColor, color));
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

        // DESSIN
        glBindVertexArray(vao);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());

        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }

    // NETTOYAGE
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao); 

    return EXIT_SUCCESS;
}
