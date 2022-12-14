#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>

#include <glimac/Program.hpp> 
#include <glimac/FilePath.hpp>
#include <glimac/Image.hpp>
#include <glimac/glm.hpp>

using namespace glimac;
using namespace glm;

struct Vertex2DUV {
    glm::vec2 position;
    glm::vec2 texture;

    Vertex2DUV() {}

    Vertex2DUV(glm::vec2 _position, glm::vec2 _texture)
        : position { _position }
        , texture { _texture }
    {}
};

mat3 translate(float tx, float ty) {
    return mat3(vec3(1, 0, 0), vec3(0, 1, 0), vec3(tx, ty, 1));
}

mat3 scale(float sx, float sy) {
    return mat3(vec3(sx, 0, 0), vec3(0, sy, 0), vec3(0, 0, 1));
}

mat3 rotate(float a) {
    return mat3(vec3(cos(a), sin(a), 0), vec3(-sin(a), cos(a), 0), vec3(0, 0, 1));
}


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

    GLint uModelMatrix = glGetUniformLocation(program.getGLId(), "uModelMatrix");
    GLint uColor = glGetUniformLocation(program.getGLId(), "uColor");
    GLint uTexture = glGetUniformLocation(program.getGLId(), "uTexture");

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/

    std::unique_ptr<Image> image = loadImage("../GLImac-Template/assets/textures/triforce.png");
    
    GLuint vbo, vao, texture;
    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_ATTR_COULEUR = 1;
    const GLuint VERTEX_ATTR_TEXTURE = 2;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->getWidth(), image->getHeight(), 0, GL_RGBA, GL_FLOAT, image->getPixels());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);


    glGenBuffers(1, &vbo);

    //
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    Vertex2DUV vertices[] = { 
        Vertex2DUV(glm::vec2(-1., -1.), glm::vec2(0. , 1.)),
        Vertex2DUV(glm::vec2( 1., -1.), glm::vec2(1. , 1.)),
        Vertex2DUV(glm::vec2( 0.,  1.), glm::vec2(0.5, 0.))
    };

    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vertex2DUV), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /////

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    //glEnableVertexAttribArray(VERTEX_ATTR_COULEUR);
    glEnableVertexAttribArray(VERTEX_ATTR_TEXTURE);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DUV), offsetof(Vertex2DUV, position));
    //glVertexAttribPointer(VERTEX_ATTR_COULEUR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2DUV), (const GLvoid*) offsetof(Vertex2DUV, texture));
    glVertexAttribPointer(VERTEX_ATTR_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DUV), (const GLvoid*) offsetof(Vertex2DUV, texture));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //

    glBindVertexArray(0);

    float time = 0.;
    float time_update = 0.01;
    mat3 transformed;

    // Application loop:
    bool done = false;
    while(!done) {
        //glUniform1f(uTime, time);

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

        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(uTexture, 0);

        //
        glBindVertexArray(vao);
        glClear(GL_COLOR_BUFFER_BIT);

        transformed = rotate(time) * translate(-0.5, -0.5) * scale(0.25, 0.25) * rotate(time);
        glUniformMatrix3fv(uModelMatrix, 1, GL_FALSE, value_ptr(transformed));
        glUniform3fv(uColor, 1, value_ptr(glm::vec3(1,1,0))); 
        glDrawArrays(GL_TRIANGLES, 0, 3);

        transformed = rotate(time) * translate(0.5, 0.5) * scale(0.25, 0.25) * rotate(-2*time);
        glUniformMatrix3fv(uModelMatrix, 1, GL_FALSE, value_ptr(transformed));
        glUniform3fv(uColor, 1, value_ptr(glm::vec3(1,0,1)));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        transformed = rotate(time) * translate(-0.5, 0.5) * scale(0.25, 0.25) * rotate(time);
        glUniformMatrix3fv(uModelMatrix, 1, GL_FALSE, value_ptr(transformed));
        glUniform3fv(uColor, 1, value_ptr(glm::vec3(0,1,1)));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        transformed = rotate(time) * translate(0.5, -0.5) * scale(0.25, 0.25) * rotate(-2*time);
        glUniformMatrix3fv(uModelMatrix, 1, GL_FALSE, value_ptr(transformed));
        glUniform3fv(uColor, 1, value_ptr(glm::vec3(1,1,1)));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);
        //

        glBindTexture(GL_TEXTURE_2D, 0);

        // Update the display
        windowManager.swapBuffers();
        // Update time
        time += time_update;
    }
    
    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &vao);

    return EXIT_SUCCESS;
}
