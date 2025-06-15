#include "GLRenderer.h"
#include <android/log.h>
#include <cstring>

#define LOG_TAG "GLRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Vertex shader source
const char* vertexShaderSource = R"(
attribute vec4 a_position;
attribute vec2 a_texCoord;
varying vec2 v_texCoord;

void main() {
    gl_Position = a_position;
    v_texCoord = a_texCoord;
}
)";

// Fragment shader source
const char* fragmentShaderSource = R"(
precision mediump float;
varying vec2 v_texCoord;
uniform sampler2D u_texture;

void main() {
    gl_FragColor = texture2D(u_texture, v_texCoord);
}
)";

// Vertex data for a full-screen quad
const float vertices[] = {
    // Position     // Texture coordinates
    -1.0f, -1.0f,   0.0f, 1.0f,
     1.0f, -1.0f,   1.0f, 1.0f,
    -1.0f,  1.0f,   0.0f, 0.0f,
     1.0f,  1.0f,   1.0f, 0.0f
};

GLRenderer::GLRenderer() 
    : m_display(EGL_NO_DISPLAY)
    , m_context(EGL_NO_CONTEXT)
    , m_surface(EGL_NO_SURFACE)
    , m_program(0)
    , m_vertexShader(0)
    , m_fragmentShader(0)
    , m_texture(0)
    , m_vertexBuffer(0)
    , m_positionHandle(-1)
    , m_texCoordHandle(-1)
    , m_textureHandle(-1) {
}

GLRenderer::~GLRenderer() {
    cleanup();
}

bool GLRenderer::initialize(ANativeWindow* window) {
    // Initialize EGL
    m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_display == EGL_NO_DISPLAY) {
        LOGE("Failed to get EGL display");
        return false;
    }
    
    if (!eglInitialize(m_display, nullptr, nullptr)) {
        LOGE("Failed to initialize EGL");
        return false;
    }
    
    // Choose EGL config
    const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };
    
    EGLint numConfigs;
    if (!eglChooseConfig(m_display, configAttribs, &m_config, 1, &numConfigs)) {
        LOGE("Failed to choose EGL config");
        return false;
    }
    
    // Create EGL context
    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    
    m_context = eglCreateContext(m_display, m_config, EGL_NO_CONTEXT, contextAttribs);
    if (m_context == EGL_NO_CONTEXT) {
        LOGE("Failed to create EGL context");
        return false;
    }
    
    // Create EGL surface
    m_surface = eglCreateWindowSurface(m_display, m_config, window, nullptr);
    if (m_surface == EGL_NO_SURFACE) {
        LOGE("Failed to create EGL surface");
        return false;
    }
    
    // Make context current
    if (!eglMakeCurrent(m_display, m_surface, m_surface, m_context)) {
        LOGE("Failed to make EGL context current");
        return false;
    }
    
    // Initialize OpenGL ES resources
    if (!createShaders() || !createTexture() || !createVertexBuffer()) {
        LOGE("Failed to initialize OpenGL ES resources");
        return false;
    }
    
    LOGI("GLRenderer initialized successfully");
    return true;
}

void GLRenderer::cleanup() {
    if (m_program) {
        glDeleteProgram(m_program);
        m_program = 0;
    }
    
    if (m_texture) {
        glDeleteTextures(1, &m_texture);
        m_texture = 0;
    }
    
    if (m_vertexBuffer) {
        glDeleteBuffers(1, &m_vertexBuffer);
        m_vertexBuffer = 0;
    }
    
    if (m_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        
        if (m_surface != EGL_NO_SURFACE) {
            eglDestroySurface(m_display, m_surface);
            m_surface = EGL_NO_SURFACE;
        }
        
        if (m_context != EGL_NO_CONTEXT) {
            eglDestroyContext(m_display, m_context);
            m_context = EGL_NO_CONTEXT;
        }
        
        eglTerminate(m_display);
        m_display = EGL_NO_DISPLAY;
    }
}

bool GLRenderer::renderFrame(const unsigned char* frameData, int width, int height) {
    if (!frameData) {
        LOGE("Frame data is null");
        return false;
    }
    
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Use shader program
    glUseProgram(m_program);
    
    // Bind texture and upload frame data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, frameData);
    
    // Set texture uniform
    glUniform1i(m_textureHandle, 0);
    
    // Bind vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    
    // Set vertex attributes
    glVertexAttribPointer(m_positionHandle, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(m_positionHandle);
    
    glVertexAttribPointer(m_texCoordHandle, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(m_texCoordHandle);
    
    // Draw quad
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    // Disable vertex attributes
    glDisableVertexAttribArray(m_positionHandle);
    glDisableVertexAttribArray(m_texCoordHandle);
    
    // Swap buffers
    eglSwapBuffers(m_display, m_surface);
    
    return checkGLError("renderFrame");
}

void GLRenderer::setViewport(int width, int height) {
    glViewport(0, 0, width, height);
}

bool GLRenderer::createShaders() {
    // Load vertex shader
    m_vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderSource);
    if (!m_vertexShader) {
        return false;
    }
    
    // Load fragment shader
    m_fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    if (!m_fragmentShader) {
        return false;
    }
    
    // Create program
    m_program = glCreateProgram();
    if (!m_program) {
        LOGE("Failed to create shader program");
        return false;
    }
    
    // Attach shaders
    glAttachShader(m_program, m_vertexShader);
    glAttachShader(m_program, m_fragmentShader);
    
    // Link program
    glLinkProgram(m_program);
    
    // Check link status
    GLint linkStatus;
    glGetProgramiv(m_program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        GLint logLength;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            char* log = new char[logLength];
            glGetProgramInfoLog(m_program, logLength, nullptr, log);
            LOGE("Program link error: %s", log);
            delete[] log;
        }
        return false;
    }
    
    // Get attribute and uniform locations
    m_positionHandle = glGetAttribLocation(m_program, "a_position");
    m_texCoordHandle = glGetAttribLocation(m_program, "a_texCoord");
    m_textureHandle = glGetUniformLocation(m_program, "u_texture");
    
    return true;
}

bool GLRenderer::createTexture() {
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    return checkGLError("createTexture");
}

bool GLRenderer::createVertexBuffer() {
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    return checkGLError("createVertexBuffer");
}

GLuint GLRenderer::loadShader(GLenum shaderType, const char* shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    if (!shader) {
        LOGE("Failed to create shader");
        return 0;
    }
    
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);
    
    // Check compile status
    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus != GL_TRUE) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            char* log = new char[logLength];
            glGetShaderInfoLog(shader, logLength, nullptr, log);
            LOGE("Shader compile error: %s", log);
            delete[] log;
        }
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

bool GLRenderer::checkGLError(const char* operation) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("OpenGL error in %s: 0x%x", operation, error);
        return false;
    }
    return true;
}

