#include "openglcanvas.h"

OpenGLCanvas::OpenGLCanvas(wxWindow *parent, const wxGLAttributes &canvasAttrs, const Settings &settings)
    : wxGLCanvas(parent, canvasAttrs), settings(settings)
{
    wxGLContextAttrs ctxAttrs;
    ctxAttrs.PlatformDefaults().CoreProfile().OGLVersion(3, 3).EndList();
    openGLContext = new wxGLContext(this, nullptr, &ctxAttrs);

    if (!openGLContext->IsOK())
    {
        wxMessageBox("This sample needs an OpenGL 3.3 capable driver.",
                     "OpenGL version error", wxOK | wxICON_INFORMATION, this);
        delete openGLContext;
        openGLContext = nullptr;
    }

    Bind(wxEVT_PAINT, &OpenGLCanvas::OnPaint, this);
    Bind(wxEVT_SIZE, &OpenGLCanvas::OnSize, this);

    Bind(wxEVT_KEY_DOWN, &OpenGLCanvas::OnKeyDown, this);
}

OpenGLCanvas::~OpenGLCanvas()
{
    delete openGLContext;
}

bool OpenGLCanvas::InitializeOpenGLFunctions()
{
    GLenum err = glewInit();

    if (GLEW_OK != err)
    {
        wxLogError("OpenGL GLEW initialization failed: %s", reinterpret_cast<const char *>(glewGetErrorString(err)));
        return false;
    }

    wxLogDebug("Status: Using GLEW %s", reinterpret_cast<const char *>(glewGetString(GLEW_VERSION)));

    return true;
}

bool OpenGLCanvas::InitializeOpenGL()
{
    if (!openGLContext)
    {
        return false;
    }

    SetCurrent(*openGLContext);

    if (!InitializeOpenGLFunctions())
    {
        wxMessageBox("Error: Could not initialize OpenGL function pointers.",
                     "OpenGL initialization error", wxOK | wxICON_INFORMATION, this);
        return false;
    }

    wxLogDebug("OpenGL version: %s", reinterpret_cast<const char *>(glGetString(GL_VERSION)));
    wxLogDebug("OpenGL vendor: %s", reinterpret_cast<const char *>(glGetString(GL_VENDOR)));

    constexpr auto vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;

        uniform mat4 modelMatrix;

        out vec3 vertexColor;

        void main()
        {
            gl_Position = modelMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
            vertexColor = aColor;
        }
    )";

    constexpr auto fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        in vec3 vertexColor;

        void main()
        {
            FragColor = vec4(vertexColor, 1.0);
        }
    )";

    // passing GL_VERTEX_SHADER is necessary and i don't think i'll find a case where I shouldn't do that
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // creates the empty shader for initialization
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    // double checks that open gl initialization succeeds
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        wxLogDebug("Vertex Shader Compilation Failed: %s", infoLog);
    }

    // same process here just in the form of fragment(pixel) shaders.
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        wxLogDebug("Fragment Shader Compilation Failed: %s", infoLog);
    }

    // now we create a shader program. aka gpu compilable code
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    // double checks that program is created properly
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        wxLogDebug("Shader Program Linking Failed: %s", infoLog);
    }

    // clears frag and vertex code to get rid of any noise.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // VAO is a glsl name
    // glsl uses a hash map where the names of objects are integers
    // technically here we create a singular object in an array tied to the name VAO
    // by increasing the first argument we can store all of our vertices inside the VAO...
    // i think that's what is happening here.
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // binds VAO to the glsl runtime
    // binds buffer to GL_ARRAY_BUFFER, the default object that will connect the buffer and VBO
    // THEN we set the data to the array and set it to draw.
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, s.xyzArray.size() * sizeof(float), s.xyzArray.data(), GL_STATIC_DRAW);
    
    // little confused here
    // setting buffer size to be 6 floats because every buffer will contain the xyz and rgb
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // second vertexAttribPointer? not sure the reason
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // element array buffer. basically the handling the indices and assigning their "names" aka ints to the float.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 s.triangleVertexIndices.size() * sizeof(unsigned int),
                 s.triangleVertexIndices.data(),
                 GL_STATIC_DRAW);

    // unbind vertex array so for next iteration??
    glBindVertexArray(0);

    // unbind
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    UpdateOpenGLSettings();

    isOpenGLInitialized = true;

    return true;
}

void OpenGLCanvas::UpdateOpenGLSettings()
{
    if (settings.zBufferEnabled)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(settings.zBufferOrder == Settings::ZBufferOrder::Less ? GL_LESS : GL_GREATER);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }

    if (settings.faceCullingEnabled)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(settings.faceCulling == Settings::FaceCulling::Front ? GL_FRONT : GL_BACK);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
}

void OpenGLCanvas::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    wxPaintDC dc(this);

    if (!isOpenGLInitialized)
    {
        return;
    }

    SetCurrent(*openGLContext);

    glClearColor(0.95f, 0.95f, 0.95f, 1.0f);

    GLint depthFuncValue;
    glGetIntegerv(GL_DEPTH_FUNC, &depthFuncValue);
    glClearDepth(depthFuncValue == GL_LESS ? 1.0f : 0.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    auto modelMatrix = glm::mat4(1.0f);

    modelMatrix = glm::rotate(modelMatrix, glm::radians(anglex), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(angley), glm::vec3(0.0f, 1.0f, 0.0f));

    auto modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, s.triangleVertexIndices.size(), GL_UNSIGNED_INT, 0);

    SwapBuffers();
}

void OpenGLCanvas::OnSize(wxSizeEvent &event)
{
    bool firstApperance = IsShownOnScreen() && !isOpenGLInitialized;

    if (firstApperance)
    {
        InitializeOpenGL();
    }

    if (isOpenGLInitialized)
    {
        auto viewPortSize = event.GetSize() * GetContentScaleFactor();
        glViewport(0, 0, viewPortSize.x, viewPortSize.y);
    }

    event.Skip();
}

void OpenGLCanvas::OnKeyDown(wxKeyEvent &event)
{
    constexpr float AngleIncrement = 4.0f;

    if (event.GetKeyCode() == WXK_LEFT)
    {
        angley -= AngleIncrement;
    }
    else if (event.GetKeyCode() == WXK_RIGHT)
    {
        angley += AngleIncrement;
    }
    else if (event.GetKeyCode() == WXK_UP)
    {
        anglex += AngleIncrement;
    }
    else if (event.GetKeyCode() == WXK_DOWN)
    {
        anglex -= AngleIncrement;
    }
    else
    {
        event.Skip();
    }

    Refresh();
}
