#include "main.h"

int main(int argc, char *argv[]) {
    string stringNumOfThreads;
    cout << "Enter the number of threads you want to use (Default is " << DEFAULT_NUM_OF_THREADS << "): " << endl;
    getline(cin, stringNumOfThreads);
    int decision = string2int(stringNumOfThreads);
    if (decision > 0 && decision <= MAX_NUM_OF_THREADS) {
        cout << "Your decision: " << decision << "." << endl;
        numOfThreads = decision;
    } else {
        cout << "Invalid input: " << stringNumOfThreads << ". Default number will be used." << endl;
        numOfThreads = DEFAULT_NUM_OF_THREADS;
    }

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
    printf("Max vertex attributes available: %d.\n", maxVertAttrs);
    if (maxVertAttrs < 8) {
        printf("Minimum required: 8\n");
        exit(EXIT_FAILURE);
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