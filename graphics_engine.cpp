#include "graphics_engine.h"
#

#define SCR_W 800
#define SCR_H 600
#define SCALE 4

graphics_engine::graphics_engine()
{
    /* Initialize the library */
    if (!glfwInit())
        std::cerr << "ERROR::GRAPHICS_ENGINE::GLFW INIT FAILED" << std::endl;

    //get screen size
    monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);

    scr_w = mode->width;
    scr_h = mode->height;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(scr_w, scr_h, "Play against Slow Loris", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        std::cerr << "ERROR::GRAPHICS_ENGINE::WINDOW CREATION FAILED" << std::endl;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //initialize glew to use opengl
    GLenum err = glewInit();
    if (err != GLEW_OK)
        std::cerr << "ERROR::GRAPHICS_ENGINE::GLEW INIT FAILED" << std::endl;

    glViewport(0, 0, scr_w, scr_h);

    shader = new Shader("VertexShader.txt", "FragmentShader.txt");
    shader->use();

    //Shader Shader2("VertexShader.txt", "FragmentShader2.txt");

    //create graphics for board and pieces
    board = new Board(shader, "Textures//chessboardLight.png");
    board->scale(SCALE, SCALE);

    white_pieces = white_pieces_init(SCALE);
    black_pieces = black_pieces_init(SCALE);

    //shader setup
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::ortho(-scr_w / 2.0f, scr_w / 2.0f, -scr_h / 2.0f, scr_h / 2.0f, -1.0f, 1.0f);
    shader->setUniformMat4fv("projection", projection);
    shader->setUniformMat4fv("projection", projection);

    //renderSettings
    //--------------
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
}

graphics_engine::~graphics_engine()
{
    delete board;
    delete shader;
    delete[] white_pieces;
    delete[] black_pieces;
    glfwTerminate();
}

void graphics_engine::render(const state& s, int grabbed_piece, double mx, double my)
{
    /* Render here */
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
    GLCall(glClear(GL_DEPTH_BUFFER_BIT));

    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
        glfwSetWindowMonitor(window, NULL, 0, 0.1 * scr_h, 0.9 * scr_w, 0.9 * scr_h, mode->refreshRate);
    board->render();

    //render the pieces for each square if there are any on it
    for (int i = 0; i < 64; i++)
    {
        int type = s.position[i].get_type();
        int color = s.position[i].get_color();

        if (grabbed_piece - 1 == i) {
            if (color == 1) {
                white_pieces[type - 1].translate(mx, my, .3f);
                white_pieces[type - 1].render();
            }
            else if (color == -1) {
                black_pieces[type - 1].translate(mx, my, .3f);
                black_pieces[type - 1].render();
            }
        }

        else {
            glm::vec2 square = board->get_square(i);
            if (type) {
                if (color == 1) {
                    white_pieces[type - 1].translate(square.x, square.y, .2f);
                    white_pieces[type - 1].render();
                }
                else if (color == -1) {
                    black_pieces[type - 1].translate(square.x, square.y, .2f);
                    black_pieces[type - 1].render();
                }
            }
        }
    }

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
}

GLFWwindow* graphics_engine::get_window()
{
    return window;
}

int graphics_engine::get_scr_w()
{
    return scr_w;
}

int graphics_engine::get_scr_h()
{
    return scr_h;
}

Board* graphics_engine::get_board()
{
    return board;
}

Object* graphics_engine::white_pieces_init(int scaler)
{
    int d = 25;
    Object* list = new Object[6]{
        {shader, d, "Textures//Chess_klt60.png"},   //king
        {shader, d, "Textures//Chess_qlt60.png"},   //queen
        {shader, d, "Textures//Chess_blt60.png"},   //bishop
        {shader, d, "Textures//Chess_nlt60.png"},   //knight
        {shader, d, "Textures//Chess_rlt60.png"},   //rook
        {shader, d, "Textures//Chess_plt60.png"},   //pawn
    };
    for (int i = 0; i < 6; i++)
        list[i].scale(scaler);
    return list;
}

Object* graphics_engine::black_pieces_init(int scaler)
{
    int d = 25;
    Object* list = new Object[6]{
        {shader, d, "Textures//Chess_kdt60.png"},   //king
        {shader, d, "Textures//Chess_qdt60.png"},   //queen
        {shader, d, "Textures//Chess_bdt60.png"},   //bishop
        {shader, d, "Textures//Chess_ndt60.png"},   //knight
        {shader, d, "Textures//Chess_rdt60.png"},   //rook
        {shader, d, "Textures//Chess_pdt60.png"},   //pawn
    };
    for (int i = 0; i < 6; i++)
        list[i].scale(scaler);
    return list;
}




