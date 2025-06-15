#ifndef GLRENDERER_H
#define GLRENDERER_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <android/native_window.h>

class GLRenderer {
public:
    GLRenderer();
    ~GLRenderer();
    
    bool initialize(ANativeWindow* window);
    void cleanup();
    
    bool renderFrame(const unsigned char* frameData, int width, int height);
    void setViewport(int width, int height);
    
private:
    EGLDisplay m_display;
    EGLContext m_context;
    EGLSurface m_surface;
    EGLConfig m_config;
    
    GLuint m_program;
    GLuint m_vertexShader;
    GLuint m_fragmentShader;
    GLuint m_texture;
    GLuint m_vertexBuffer;
    
    GLint m_positionHandle;
    GLint m_texCoordHandle;
    GLint m_textureHandle;
    
    bool createShaders();
    bool createTexture();
    bool createVertexBuffer();
    GLuint loadShader(GLenum shaderType, const char* shaderSource);
    bool checkGLError(const char* operation);
};

#endif // GLRENDERER_H

