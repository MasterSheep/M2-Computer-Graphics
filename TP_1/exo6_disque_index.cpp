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

    GLuint vbo, vao, ibo;
    const GLuint VERTEX_ATTR_POSITION = 3;
    const GLuint VERTEX_ATTR_COULEUR = 8;
    const unsigned int nb_triangle = 50;
    const float rayon = 0.75;

    glGenBuffers(1, &vbo);

    //
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    std::vector<Vertex2DColor> vertices;
    
    vertices.emplace_back( Vertex2DColor(glm::vec2( 0, 0), glm::vec3(1, 1, 1)));

    for (int i = 0; i < nb_triangle; i++) {
        vertices.emplace_back( Vertex2DColor( glm::vec2( rayon * glm::cos(i * ((2 * glm::pi<float>()) / nb_triangle)),
                                                         rayon * glm::sin(i * ((2 * glm::pi<float>()) / nb_triangle))), 
                                              glm::vec3(1, 0, 0)));
    }


    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2DColor), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //

    //////

    glGenBuffers(1, &ibo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    std::vector<uint32_t> indices;
    for (int i = 1; i <= nb_triangle; i++) {
        indices.emplace_back(0);
        indices.emplace_back(i);
        indices.emplace_back(i + 1);
    }
    indices.emplace_back(0);
    indices.emplace_back(nb_triangle);
    indices.emplace_back(1);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_COULEUR);

    //
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*) offsetof(Vertex2DColor, position));
    glVertexAttribPointer(VERTEX_ATTR_COULEUR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*) offsetof(Vertex2DColor, color));
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
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }

    // NETTOYAGE
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao); 

    return EXIT_SUCCESS;
}
