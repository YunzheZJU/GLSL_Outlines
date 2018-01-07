#include "main.h"

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("GPU Based Rendering: A2");

    // Set the background color - dark grey
    glClearColor(0.1, 0.1, 0.1, 0.0);

    glutDisplayFunc(Redraw);
    glutReshapeFunc(Reshape);
    glutMouseFunc(ProcessMouseClick);
    glutPassiveMotionFunc(ProcessMouseMove);
    glutEntryFunc(ProcessFocus);
    glutKeyboardFunc(ProcessNormalKey);
    glutIdleFunc(Idle);

    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        cerr << "Error occurred when initializing GLEW: " << glewGetErrorString(glewErr) << endl;
        exit(1);
    }
    if (!glewIsSupported("GL_VERSION_4_3")) {
        cerr << "OpenGL 4.3 is not supported" << endl;
        exit(1);
    }
    GLint maxVertAttrs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertAttrs);
    if (maxVertAttrs < 16) {
        printf("Max vertex attributes available: %d. Minimum required: 16\n", maxVertAttrs);
        exit(0);
    }

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initShader();
    initVBO();
    setShader();

    glutMainLoop();

    return 0;
}