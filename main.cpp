#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "GLM/glm/gtc/matrix_transform.hpp"
#include "GLM/glm/gtc/type_ptr.hpp"
#include "GLM/glm/glm.hpp"

#include "Button.h"
#include "Board.h"
#include "Pawn.h"
#include "Queen.h"
#include "Rook.h"
#include "Bishop.h"
#include "Knight.h"
#include "King.h"
#include "GLCall.h"
#include "Piece.h"
#include "Object.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void cursor_pos_callback(GLFWwindow* window, double xPos, double yPos);
//void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void Make_Move(GLFWwindow* window);
void PieceToCursor(int index, float xPos, float yPos);
bool PieceReleased(int index, float xPos, float yPos);
bool Case_Check(bool turn);
void Case_Pins(bool turn);
bool Case_Capture(bool turn, int pieceIndex);
void Case_EnPassant(bool turn, int pieceIndex);
void Case_Promotion(int pieceIndex);
void Case_Castles(bool turn);
void Execute_Castles(bool turn, bool long_short);
void End_Game(bool turn);
float xToMouse(float x);
template <typename T> T MouseToX(T x);
float yToMouse(float y);
template <typename T> T MouseToY(T y);

static Board* Boardptr;
static Shader* Shader1ptr; 
static float ObjectScaler1 = 2.5f;
static int Screen_Width = 800;
static int Screen_Height = 600;
static bool Grabbed = false;
static bool GameInProgress = 0;
static bool FirstMove = 1;


float PieceData[] = {
//vertex Pos                      texcoords
 12.5f,  12.5f, 0.0f,             1.0f, 1.0f,         //top right
-12.5f,  12.5f, 0.0f,             0.0f, 1.0f,         //top left
-12.5f, -12.5f, 0.0f,             0.0f, 0.0f,         //bottom left
 12.5f, -12.5f, 0.0f,             1.0f, 0.0f,         //bottom right
};

unsigned int PieceElements[] = {
   0, 1, 2, 2, 3, 0,
};
int main()
{
	/*create window
	-------------------------------------------------------------------------------------*/
	if (!glfwInit())
		std::cerr << "ERROR: glfw was not initialized properly." << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(Screen_Width, Screen_Height, "Chess_v2", 0, 0);
	if (!window)
	{
		std::cerr << "ERROR: failed to craete window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

    //set callbacks
    //--------------------------------------------------------
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  //  glfwSetCursorPosCallback(window, cursor_pos_callback);
  //  glfwSetMouseButtonCallback(window, mouse_button_callback);
	GLenum err = glewInit();
	if (err != GLEW_OK)
		std::cerr << "ERROR: failed to initialize glew" << std::endl;

	/*code
	------------------------------------------------------------------------------------------*/
    // build and compile shader program
    // ------------------------------------
    Shader Shader1("VertexShader.txt", "FragmentShader.txt");
    Shader1.use();
    Shader1ptr = &Shader1;

    Shader Shader2("VertexShader.txt", "FragmentShader2.txt");

    //user interface creation
    //----------------------------------------------------------------------
    float NewGameButton_Vertices[] = {
        //     vertex positions                  TexCoords
               25.0f,  25.0f,  0.0f,           1.0f, 1.0f,              //top right
               25.0f, -25.0f,  0.0f,           1.0f, 0.0f,              //bottom right
              -25.0f, -25.0f,  0.0f,           0.0f, 0.0f,              //bottom left
              -25.0f,  25.0f,  0.0f,           0.0f, 1.0f,              //top left
    };

    VertexArray NewGameButtonVAO;
    VertexBuffer NewGameButtonVBO(NewGameButton_Vertices, sizeof(NewGameButton_Vertices));
    IndexBuffer NewGameButtonEBO(PieceElements, sizeof(PieceElements));
    Texture NewGameButtonTex("Textures\\Chessking.png");
    NewGameButtonTex.setWrapParam(GL_CLAMP_TO_EDGE);
    NewGameButtonVAO.attrib(3);
    NewGameButtonVAO.attrib(2);
    NewGameButtonVAO.generate();

    Button NewGameButton(&Shader1, &NewGameButtonVAO, &NewGameButtonTex, 50.0f, 25.0f, { 330.0f, 0.0f, 0.0f }, Screen_Width, Screen_Height);
    NewGameButton.scale(ObjectScaler1, ObjectScaler1);


    //Board
    //----------------------------------------------------------------------
    float board_vertices[] = {
     //     vertex positions                  TexCoords
            100.0f,  100.0f,  0.0f,           1.0f, 1.0f,              //top right
            100.0f, -100.0f,  0.0f,           1.0f, 0.0f,              //bottom right
           -100.0f, -100.0f,  0.0f,           0.0f, 0.0f,              //bottom left
           -100.0f,  100.0f,  0.0f,           0.0f, 1.0f,              //top left
    };

    unsigned int board_elements[] = { 0, 1, 3, 1, 2, 3 };

    VertexArray board_VAO;
    VertexBuffer board_VBO(board_vertices, sizeof(board_vertices));
    IndexBuffer board_EBO(board_elements, sizeof(board_elements));
    Texture BoardTex("Textures//chessboardLight.png");
    board_VAO.attrib(3);
    board_VAO.attrib(2);
    board_VAO.generate();

    Board Board(&Shader1, &board_VAO, &BoardTex, 6);
    Boardptr = &Board;
    Board.scale(ObjectScaler1, ObjectScaler1);

NewGame:
    //Black and White King creation
    //-----------------------------
    //Black King
    VertexArray BlackKingVao;
    VertexBuffer BlackKingVbo(PieceData, sizeof(PieceData));
    IndexBuffer BlackKingEbo(PieceElements, sizeof(PieceElements));
    Texture BlackKingTex("Textures\\Chess_kdt60.png");
    BlackKingTex.setWrapParam(GL_CLAMP_TO_EDGE);
    BlackKingVao.attrib(3);
    BlackKingVao.attrib(2);
    BlackKingVao.generate();

    King BlackKing(&Shader1, &BlackKingVao, &BlackKingTex, 0, 6);
    BlackKing.scale(ObjectScaler1, ObjectScaler1);
    BlackKing.transpose(xToMouse(Piece::Grid.SquareDiameter), yToMouse(Piece::Grid.SquareDiameter * 3.5), 0.1f);

    //White King
    VertexArray WhiteKingVao;
    VertexBuffer WhiteKingVbo(PieceData, sizeof(PieceData));
    IndexBuffer WhiteKingEbo(PieceElements, sizeof(PieceElements));
    Texture WhiteKingTex("Textures\\Chess_klt60.png");
    WhiteKingTex.setWrapParam(GL_CLAMP_TO_EDGE);
    WhiteKingVao.attrib(3);
    WhiteKingVao.attrib(2);
    WhiteKingVao.generate();

    King WhiteKing(&Shader1, &WhiteKingVao, &WhiteKingTex, 1, 6);
    WhiteKing.scale(ObjectScaler1, ObjectScaler1);
    WhiteKing.transpose(xToMouse(Piece::Grid.SquareDiameter), yToMouse(-Piece::Grid.SquareDiameter * 3.5), 0.1f);

    //Black and White Knights
    //-----------------------
    //Black Knight 1
    VertexArray BlackKnight1Vao;
    VertexBuffer BlackKnight1Vbo(PieceData, sizeof(PieceData));
    IndexBuffer BlackKnight1Ebo(PieceElements, sizeof(PieceElements));
    Texture BlackKnight1Tex("Textures//Chess_ndt60.png");
    BlackKnight1Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    BlackKnight1Vao.attrib(3);
    BlackKnight1Vao.attrib(2);
    BlackKnight1Vao.generate();

    Knight BlackKnight1(&Shader1, &BlackKnight1Vao, &BlackKnight1Tex, 0, 6);
    BlackKnight1.scale(ObjectScaler1, ObjectScaler1);
    BlackKnight1.transpose(xToMouse(Piece::Grid.SquareDiameter * 2.5), yToMouse(Piece::Grid.SquareDiameter * 3.5), 0.1f);

    //Black Knight 2
    VertexArray BlackKnight2Vao;
    VertexBuffer BlackKnight2Vbo(PieceData, sizeof(PieceData));
    IndexBuffer BlackKnight2Ebo(PieceElements, sizeof(PieceElements));
    Texture BlackKnight2Tex("Textures//Chess_ndt60.png");
    BlackKnight2Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    BlackKnight2Vao.attrib(3);
    BlackKnight2Vao.attrib(2);
    BlackKnight2Vao.generate();

    Knight BlackKnight2(&Shader1, &BlackKnight2Vao, &BlackKnight2Tex, 0, 6);
    BlackKnight2.scale(ObjectScaler1, ObjectScaler1);
    BlackKnight2.transpose(xToMouse(-Piece::Grid.SquareDiameter * 2.5), yToMouse(Piece::Grid.SquareDiameter * 3.5), 0.1f);

    //White Knight 1
    VertexArray WhiteKnight1Vao;
    VertexBuffer WhiteKnight1Vbo(PieceData, sizeof(PieceData));
    IndexBuffer WhiteKnight1Ebo(PieceElements, sizeof(PieceElements));
    Texture WhiteKnight1Tex("Textures//Chess_nlt60.png");
    WhiteKnight1Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    WhiteKnight1Vao.attrib(3);
    WhiteKnight1Vao.attrib(2);
    WhiteKnight1Vao.generate();

    Knight WhiteKnight1(&Shader1, &WhiteKnight1Vao, &WhiteKnight1Tex, 1, 6);
    WhiteKnight1.scale(ObjectScaler1, ObjectScaler1);
    WhiteKnight1.transpose(xToMouse(Piece::Grid.SquareDiameter * 2.5), yToMouse(-Piece::Grid.SquareDiameter * 3.5), 0.1f);

    //White Knight 2
    VertexArray WhiteKnight2Vao;
    VertexBuffer WhiteKnight2Vbo(PieceData, sizeof(PieceData));
    IndexBuffer WhiteKnight2Ebo(PieceElements, sizeof(PieceElements));
    Texture WhiteKnight2Tex("Textures//Chess_nlt60.png");
    WhiteKnight2Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    WhiteKnight2Vao.attrib(3);
    WhiteKnight2Vao.attrib(2);
    WhiteKnight2Vao.generate();

    Knight WhiteKnight2(&Shader1, &WhiteKnight2Vao, &WhiteKnight2Tex, 1, 6);
    WhiteKnight2.scale(ObjectScaler1, ObjectScaler1);
    WhiteKnight2.transpose(xToMouse(-Piece::Grid.SquareDiameter * 2.5), yToMouse(-Piece::Grid.SquareDiameter * 3.5), 0.1f);

    //Black and White Bishop creation
    //--------------------------------
    //Black Bishop 1
    VertexArray BlackBishop1Vao;
    VertexBuffer BlackBishop1Vbo(PieceData, sizeof(PieceData));
    IndexBuffer BlackBishop1Ebo(PieceElements, sizeof(PieceElements));
    Texture BlackBishop1Tex("Textures//Chess_bdt60.png");
    BlackBishop1Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    BlackBishop1Vao.attrib(3);
    BlackBishop1Vao.attrib(2);
    BlackBishop1Vao.generate();

    Bishop BlackBishop1(&Shader1, &BlackBishop1Vao, &BlackBishop1Tex, 0, 6);
    BlackBishop1.scale(ObjectScaler1, ObjectScaler1);
    BlackBishop1.transpose(xToMouse(Piece::Grid.SquareDiameter * 1.5f), yToMouse(Piece::Grid.SquareDiameter * 3.5f), 0.1f);

    //Black Bishop 2
    VertexArray BlackBishop2Vao;
    VertexBuffer BlackBishop2Vbo(PieceData, sizeof(PieceData));
    IndexBuffer BlackBishop2Ebo(PieceElements, sizeof(PieceElements));
    Texture BlackBishop2Tex("Textures//Chess_bdt60.png");
    BlackBishop2Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    BlackBishop2Vao.attrib(3);
    BlackBishop2Vao.attrib(2);
    BlackBishop2Vao.generate();

    Bishop BlackBishop2(&Shader1, &BlackBishop2Vao, &BlackBishop2Tex, 0, 6);
    BlackBishop2.scale(ObjectScaler1, ObjectScaler1);
    BlackBishop2.transpose(xToMouse(-Piece::Grid.SquareDiameter * 1.5f), yToMouse(Piece::Grid.SquareDiameter * 3.5f), 0.1f);

    //White Bishop 1
    VertexArray WhiteBishop1Vao;
    VertexBuffer WhiteBishop1Vbo(PieceData, sizeof(PieceData));
    IndexBuffer WhiteBishop1Ebo(PieceElements, sizeof(PieceElements));
    Texture WhiteBishop1Tex("Textures//Chess_blt60.png");
    WhiteBishop1Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    WhiteBishop1Vao.attrib(3);
    WhiteBishop1Vao.attrib(2);
    WhiteBishop1Vao.generate();

    Bishop WhiteBishop1(&Shader1, &WhiteBishop1Vao, &WhiteBishop1Tex, 1, 6);
    WhiteBishop1.scale(ObjectScaler1, ObjectScaler1);
    WhiteBishop1.transpose(xToMouse(Piece::Grid.SquareDiameter * 1.5f), yToMouse(-Piece::Grid.SquareDiameter * 3.5f), 0.1f);

    //White Bishop 2
    VertexArray WhiteBishop2Vao;
    VertexBuffer WhiteBishop2Vbo(PieceData, sizeof(PieceData));
    IndexBuffer WhiteBishop2Ebo(PieceElements, sizeof(PieceElements));
    Texture WhiteBishop2Tex("Textures//Chess_blt60.png");
    WhiteBishop2Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    WhiteBishop2Vao.attrib(3);
    WhiteBishop2Vao.attrib(2);
    WhiteBishop2Vao.generate();

    Bishop WhiteBishop2(&Shader1, &WhiteBishop2Vao, &WhiteBishop2Tex, 1, 6);
    WhiteBishop2.scale(ObjectScaler1, ObjectScaler1);
    WhiteBishop2.transpose(xToMouse(-Piece::Grid.SquareDiameter * 1.5f), yToMouse(-Piece::Grid.SquareDiameter * 3.5f), 0.1f);

    //Rooks
    //------------------
    //Black Rook 1
    VertexArray BlackRook1Vao;
    VertexBuffer BlackRook1Vbo(PieceData, sizeof(PieceData));
    IndexBuffer BlackRook1Ebo(PieceElements, sizeof(PieceElements));
    Texture BlackRook1Tex("Textures//Chess_rdt60.png");
    BlackRook1Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    BlackRook1Vao.attrib(3);
    BlackRook1Vao.attrib(2);
    BlackRook1Vao.generate();

    Rook BlackRook1(&Shader1, &BlackRook1Vao, &BlackRook1Tex, 0, 6);
    BlackRook1.scale(ObjectScaler1, ObjectScaler1);
    BlackRook1.transpose(xToMouse(-Piece::Grid.SquareDiameter * 3.5f), yToMouse(Piece::Grid.SquareDiameter * 3.5f), 0.1f);

    //Black Rook 2
    VertexArray BlackRook2Vao;
    VertexBuffer BlackRook2Vbo(PieceData, sizeof(PieceData));
    IndexBuffer BlackRook2Ebo(PieceElements, sizeof(PieceElements));
    Texture BlackRook2Tex("Textures//Chess_rdt60.png");
    BlackRook2Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    BlackRook2Vao.attrib(3);
    BlackRook2Vao.attrib(2);
    BlackRook2Vao.generate();

    Rook BlackRook2(&Shader1, &BlackRook1Vao, &BlackRook1Tex, 0, 6);
    BlackRook2.scale(ObjectScaler1, ObjectScaler1);
    BlackRook2.transpose(xToMouse(Piece::Grid.SquareDiameter * 3.5f), yToMouse(Piece::Grid.SquareDiameter * 3.5f), 0.1f);

    //White Rook 1
    VertexArray WhiteRook1Vao;
    VertexBuffer WhiteRook1Vbo(PieceData, sizeof(PieceData));
    IndexBuffer WhiteRook1Ebo(PieceElements, sizeof(PieceElements));
    Texture WhiteRook1Tex("Textures//Chess_rlt60.png");
    WhiteRook1Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    WhiteRook1Vao.attrib(3);
    WhiteRook1Vao.attrib(2);
    WhiteRook1Vao.generate();

    Rook WhiteRook1(&Shader1, &WhiteRook1Vao, &WhiteRook1Tex, 1, 6);
    WhiteRook1.scale(ObjectScaler1, ObjectScaler1);
    WhiteRook1.transpose(xToMouse(-Piece::Grid.SquareDiameter * 3.5f), yToMouse(-Piece::Grid.SquareDiameter * 3.5f), 0.1f);

    //White Rook 2
    VertexArray WhiteRook2Vao;
    VertexBuffer WhiteRook2Vbo(PieceData, sizeof(PieceData));
    IndexBuffer WhiteRook2Ebo(PieceElements, sizeof(PieceElements));
    Texture WhiteRook2Tex("Textures//Chess_rlt60.png");
    WhiteRook2Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    WhiteRook2Vao.attrib(3);
    WhiteRook2Vao.attrib(2);
    WhiteRook2Vao.generate();

    Rook WhiteRook2(&Shader1, &WhiteRook1Vao, &WhiteRook1Tex, 1, 6);
    WhiteRook2.scale(ObjectScaler1, ObjectScaler1);
    WhiteRook2.transpose(xToMouse(Piece::Grid.SquareDiameter * 3.5f), yToMouse(-Piece::Grid.SquareDiameter * 3.5f), 0.1f);

    //Queens
    //----------------------------
    //Black Queen
    VertexArray BlackQueenVao;
    VertexBuffer BlackQueenVbo(PieceData, sizeof(PieceData));
    IndexBuffer BlackQueenEbo(PieceElements, sizeof(PieceElements));
    Texture BlackQueenTex("Textures//Chess_qdt60.png");
    BlackQueenTex.setWrapParam(GL_CLAMP_TO_EDGE);
    BlackQueenVao.attrib(3);
    BlackQueenVao.attrib(2);
    BlackQueenVao.generate();

    Queen BlackQueen(&Shader1, &BlackQueenVao, &BlackQueenTex, 0, 6);
    BlackQueen.scale(ObjectScaler1, ObjectScaler1);
    BlackQueen.transpose(xToMouse(-Piece::Grid.SquareDiameter * 0.5f), yToMouse(Piece::Grid.SquareDiameter * 3.5f), 0.1f);

    //White Queen
    VertexArray WhiteQueenVao;
    VertexBuffer WhiteQueenVbo(PieceData, sizeof(PieceData));
    IndexBuffer WhiteQueenEbo(PieceElements, sizeof(PieceElements));
    Texture WhiteQueenTex("Textures//Chess_qlt60.png");
    WhiteQueenTex.setWrapParam(GL_CLAMP_TO_EDGE);
    WhiteQueenVao.attrib(3);
    WhiteQueenVao.attrib(2);
    WhiteQueenVao.generate();

    Queen WhiteQueen(&Shader1, &WhiteQueenVao, &WhiteQueenTex, 1, 6);
    WhiteQueen.scale(ObjectScaler1, ObjectScaler1);
    WhiteQueen.transpose(xToMouse(-Piece::Grid.SquareDiameter * 0.5f), yToMouse(-Piece::Grid.SquareDiameter * 3.5f), 0.1f);

    //Pawn creation
    //---------------------------------------
    //Black Pawns
    VertexArray BlackPawn1Vao;
    VertexBuffer BlackPawn1Vbo(PieceData, sizeof(PieceData));
    IndexBuffer BlackPawn1Ebo(PieceElements, sizeof(PieceElements));
    Texture BlackPawn1Tex("Textures//Chess_pdt60.png");
    BlackPawn1Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    BlackPawn1Vao.attrib(3);
    BlackPawn1Vao.attrib(2);
    BlackPawn1Vao.generate();

    Pawn BlackPawn1(&Shader1, &BlackPawn1Vao, &BlackPawn1Tex, 0, 6);
    BlackPawn1.scale(ObjectScaler1, ObjectScaler1);
    BlackPawn1.transpose(xToMouse(-Piece::Grid.SquareDiameter * 3.5f), yToMouse(Piece::Grid.SquareDiameter * 2.5f), 0.1f);

    VertexArray BlackPawn2Vao;
    VertexBuffer BlackPawn2Vbo(PieceData, sizeof(PieceData));
    IndexBuffer BlackPawn2Ebo(PieceElements, sizeof(PieceElements));
    Texture BlackPawn2Tex("Textures//Chess_pdt60.png");
    BlackPawn2Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    BlackPawn2Vao.attrib(3);
    BlackPawn2Vao.attrib(2);
    BlackPawn2Vao.generate();

    Pawn BlackPawn2(&Shader1, &BlackPawn2Vao, &BlackPawn2Tex, 0, 6);
    BlackPawn2.scale(ObjectScaler1, ObjectScaler1);
    BlackPawn2.transpose(xToMouse(-Piece::Grid.SquareDiameter * 2.5f), yToMouse(Piece::Grid.SquareDiameter * 2.5f), 0.1f);

    VertexArray BlackPawn3Vao;
    VertexBuffer BlackPawn3Vbo(PieceData, sizeof(PieceData));
    IndexBuffer BlackPawn3Ebo(PieceElements, sizeof(PieceElements));
    Texture BlackPawn3Tex("Textures//Chess_pdt60.png");
    BlackPawn3Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    BlackPawn3Vao.attrib(3);
    BlackPawn3Vao.attrib(2);
    BlackPawn3Vao.generate();

    Pawn BlackPawn3(&Shader1, &BlackPawn3Vao, &BlackPawn3Tex, 0, 6);
    BlackPawn3.scale(ObjectScaler1, ObjectScaler1);
    BlackPawn3.transpose(xToMouse(-Piece::Grid.SquareDiameter * 1.5f), yToMouse(Piece::Grid.SquareDiameter * 2.5f), 0.1f);

    VertexArray BlackPawn4Vao;
    VertexBuffer BlackPawn4Vbo(PieceData, sizeof(PieceData));
    IndexBuffer BlackPawn4Ebo(PieceElements, sizeof(PieceElements));
    Texture BlackPawn4Tex("Textures//Chess_pdt60.png");
    BlackPawn4Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    BlackPawn4Vao.attrib(3);
    BlackPawn4Vao.attrib(2);
    BlackPawn4Vao.generate();

    Pawn BlackPawn4(&Shader1, &BlackPawn4Vao, &BlackPawn4Tex, 0, 6);
    BlackPawn4.scale(ObjectScaler1, ObjectScaler1);
    BlackPawn4.transpose(xToMouse(-Piece::Grid.SquareDiameter * 0.5f), yToMouse(Piece::Grid.SquareDiameter * 2.5f), 0.1f);

    VertexArray BlackPawn5Vao;
    VertexBuffer BlackPawn5Vbo(PieceData, sizeof(PieceData));
    IndexBuffer BlackPawn5Ebo(PieceElements, sizeof(PieceElements));
    Texture BlackPawn5Tex("Textures//Chess_pdt60.png");
    BlackPawn5Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    BlackPawn5Vao.attrib(3);
    BlackPawn5Vao.attrib(2);
    BlackPawn5Vao.generate();

    Pawn BlackPawn5(&Shader1, &BlackPawn5Vao, &BlackPawn5Tex, 0, 6);
    BlackPawn5.scale(ObjectScaler1, ObjectScaler1);
    BlackPawn5.transpose(xToMouse(Piece::Grid.SquareDiameter * 0.5f), yToMouse(Piece::Grid.SquareDiameter * 2.5f), 0.1f);

    VertexArray BlackPawn6Vao;
    VertexBuffer BlackPawn6Vbo(PieceData, sizeof(PieceData));
    IndexBuffer BlackPawn6Ebo(PieceElements, sizeof(PieceElements));
    Texture BlackPawn6Tex("Textures//Chess_pdt60.png");
    BlackPawn6Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    BlackPawn6Vao.attrib(3);
    BlackPawn6Vao.attrib(2);
    BlackPawn6Vao.generate();

    Pawn BlackPawn6(&Shader1, &BlackPawn6Vao, &BlackPawn6Tex, 0, 6);
    BlackPawn6.scale(ObjectScaler1, ObjectScaler1);
    BlackPawn6.transpose(xToMouse(Piece::Grid.SquareDiameter * 1.5f), yToMouse(Piece::Grid.SquareDiameter * 2.5f), 0.1f);

    VertexArray BlackPawn7Vao;
    VertexBuffer BlackPawn7Vbo(PieceData, sizeof(PieceData));
    IndexBuffer BlackPawn7Ebo(PieceElements, sizeof(PieceElements));
    Texture BlackPawn7Tex("Textures//Chess_pdt60.png");
    BlackPawn7Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    BlackPawn7Vao.attrib(3);
    BlackPawn7Vao.attrib(2);
    BlackPawn7Vao.generate();

    Pawn BlackPawn7(&Shader1, &BlackPawn7Vao, &BlackPawn7Tex, 0, 6);
    BlackPawn7.scale(ObjectScaler1, ObjectScaler1);
    BlackPawn7.transpose(xToMouse(Piece::Grid.SquareDiameter * 2.5f), yToMouse(Piece::Grid.SquareDiameter * 2.5f), 0.1f);

    VertexArray BlackPawn8Vao;
    VertexBuffer BlackPawn8Vbo(PieceData, sizeof(PieceData));
    IndexBuffer BlackPawn8Ebo(PieceElements, sizeof(PieceElements));
    Texture BlackPawn8Tex("Textures//Chess_pdt60.png");
    BlackPawn8Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    BlackPawn8Vao.attrib(3);
    BlackPawn8Vao.attrib(2);
    BlackPawn8Vao.generate();

    Pawn BlackPawn8(&Shader1, &BlackPawn8Vao, &BlackPawn8Tex, 0, 6);
    BlackPawn8.scale(ObjectScaler1, ObjectScaler1);
    BlackPawn8.transpose(xToMouse(Piece::Grid.SquareDiameter * 3.5f), yToMouse(Piece::Grid.SquareDiameter * 2.5f), 0.1f);

    //White Pawns
    VertexArray WhitePawn1Vao;
    VertexBuffer WhitePawn1Vbo(PieceData, sizeof(PieceData));
    IndexBuffer WhitePawn1Ebo(PieceElements, sizeof(PieceElements));
    Texture WhitePawn1Tex("Textures//Chess_plt60.png");
    WhitePawn1Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    WhitePawn1Vao.attrib(3);
    WhitePawn1Vao.attrib(2);
    WhitePawn1Vao.generate();

    Pawn WhitePawn1(&Shader1, &WhitePawn1Vao, &WhitePawn1Tex, 1, 6);
    WhitePawn1.scale(ObjectScaler1, ObjectScaler1);
    WhitePawn1.transpose(xToMouse(-Piece::Grid.SquareDiameter * 3.5f), yToMouse(-Piece::Grid.SquareDiameter * 2.5f), 0.1f);

    VertexArray WhitePawn2Vao;
    VertexBuffer WhitePawn2Vbo(PieceData, sizeof(PieceData));
    IndexBuffer WhitePawn2Ebo(PieceElements, sizeof(PieceElements));
    Texture WhitePawn2Tex("Textures//Chess_plt60.png");
    WhitePawn2Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    WhitePawn2Vao.attrib(3);
    WhitePawn2Vao.attrib(2);
    WhitePawn2Vao.generate();

    Pawn WhitePawn2(&Shader1, &WhitePawn2Vao, &WhitePawn2Tex, 1, 6);
    WhitePawn2.scale(ObjectScaler1, ObjectScaler1);
    WhitePawn2.transpose(xToMouse(-Piece::Grid.SquareDiameter * 2.5f), yToMouse(-Piece::Grid.SquareDiameter * 2.5f), 0.1f);

    VertexArray WhitePawn3Vao;
    VertexBuffer WhitePawn3Vbo(PieceData, sizeof(PieceData));
    IndexBuffer WhitePawn3Ebo(PieceElements, sizeof(PieceElements));
    Texture WhitePawn3Tex("Textures//Chess_plt60.png");
    WhitePawn3Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    WhitePawn3Vao.attrib(3);
    WhitePawn3Vao.attrib(2);
    WhitePawn3Vao.generate();

    Pawn WhitePawn3(&Shader1, &WhitePawn3Vao, &WhitePawn3Tex, 1, 6);
    WhitePawn3.scale(ObjectScaler1, ObjectScaler1);
    WhitePawn3.transpose(xToMouse(-Piece::Grid.SquareDiameter * 1.5f), yToMouse(-Piece::Grid.SquareDiameter * 2.5f), 0.1f);

    VertexArray WhitePawn4Vao;
    VertexBuffer WhitePawn4Vbo(PieceData, sizeof(PieceData));
    IndexBuffer WhitePawn4Ebo(PieceElements, sizeof(PieceElements));
    Texture WhitePawn4Tex("Textures//Chess_plt60.png");
    WhitePawn4Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    WhitePawn4Vao.attrib(3);
    WhitePawn4Vao.attrib(2);
    WhitePawn4Vao.generate();

    Pawn WhitePawn4(&Shader1, &WhitePawn4Vao, &WhitePawn4Tex, 1, 6);
    WhitePawn4.scale(ObjectScaler1, ObjectScaler1);
    WhitePawn4.transpose(xToMouse(-Piece::Grid.SquareDiameter * 0.5f), yToMouse(-Piece::Grid.SquareDiameter * 2.5f), 0.1f);

    VertexArray WhitePawn5Vao;
    VertexBuffer WhitePawn5Vbo(PieceData, sizeof(PieceData));
    IndexBuffer WhitePawn5Ebo(PieceElements, sizeof(PieceElements));
    Texture WhitePawn5Tex("Textures//Chess_plt60.png");
    WhitePawn5Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    WhitePawn5Vao.attrib(3);
    WhitePawn5Vao.attrib(2);
    WhitePawn5Vao.generate();

    Pawn WhitePawn5(&Shader1, &WhitePawn5Vao, &WhitePawn5Tex, 1, 6);
    WhitePawn5.scale(ObjectScaler1, ObjectScaler1);
    WhitePawn5.transpose(xToMouse(Piece::Grid.SquareDiameter * 0.5f), yToMouse(-Piece::Grid.SquareDiameter * 2.5f), 0.1f);

    VertexArray WhitePawn6Vao;
    VertexBuffer WhitePawn6Vbo(PieceData, sizeof(PieceData));
    IndexBuffer WhitePawn6Ebo(PieceElements, sizeof(PieceElements));
    Texture WhitePawn6Tex("Textures//Chess_plt60.png");
    WhitePawn6Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    WhitePawn6Vao.attrib(3);
    WhitePawn6Vao.attrib(2);
    WhitePawn6Vao.generate();

    Pawn WhitePawn6(&Shader1, &WhitePawn6Vao, &WhitePawn6Tex, 1, 6);
    WhitePawn6.scale(ObjectScaler1, ObjectScaler1);
    WhitePawn6.transpose(xToMouse(Piece::Grid.SquareDiameter * 1.5f), yToMouse(-Piece::Grid.SquareDiameter * 2.5f), 0.1f);

    VertexArray WhitePawn7Vao;
    VertexBuffer WhitePawn7Vbo(PieceData, sizeof(PieceData));
    IndexBuffer WhitePawn7Ebo(PieceElements, sizeof(PieceElements));
    Texture WhitePawn7Tex("Textures//Chess_plt60.png");
    WhitePawn7Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    WhitePawn7Vao.attrib(3);
    WhitePawn7Vao.attrib(2);
    WhitePawn7Vao.generate();

    Pawn WhitePawn7(&Shader1, &WhitePawn7Vao, &WhitePawn7Tex, 1, 6);
    WhitePawn7.scale(ObjectScaler1, ObjectScaler1);
    WhitePawn7.transpose(xToMouse(Piece::Grid.SquareDiameter * 2.5f), yToMouse(-Piece::Grid.SquareDiameter * 2.5f), 0.1f);

    VertexArray WhitePawn8Vao;
    VertexBuffer WhitePawn8Vbo(PieceData, sizeof(PieceData));
    IndexBuffer WhitePawn8Ebo(PieceElements, sizeof(PieceElements));
    Texture WhitePawn8Tex("Textures//Chess_plt60.png");
    WhitePawn8Tex.setWrapParam(GL_CLAMP_TO_EDGE);
    WhitePawn8Vao.attrib(3);
    WhitePawn8Vao.attrib(2);
    WhitePawn8Vao.generate();

    Pawn WhitePawn8(&Shader1, &WhitePawn8Vao, &WhitePawn8Tex, 1, 6);
    WhitePawn8.scale(ObjectScaler1, ObjectScaler1);
    WhitePawn8.transpose(xToMouse(Piece::Grid.SquareDiameter * 3.5f), yToMouse(-Piece::Grid.SquareDiameter * 2.5f), 0.1f);

    // projection & view matrix
    //-----------------------------------------------
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::ortho(-400.0f, 400.0f, -300.0f, 300.0f, -1.0f, 1.0f);
    Shader1.setUniformMat4fv("projection", projection);
    Shader2.setUniformMat4fv("projection", projection);
    //renderSettings
    //--------------
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    //last preps
    //-----------
    //Case_Check(0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        
        // input
        // -----
        
        Make_Move(window);
        //glfwSwapBuffers(window);

        // render
        // ------
        if (!FirstMove && !GameInProgress)
        {
            NewGameButton.render();
            double xPos, yPos;
            glfwGetCursorPos(window, &xPos, &yPos);
            if (NewGameButton.GetStatus(xPos, yPos, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)))
            {
                FirstMove = true;
                unsigned int j = Piece::PieceArray.size();
                for (unsigned int i = 0; i < j; i++)
                    Piece::PieceArray[0]->~Piece();
                goto NewGame;
            }
        }

        Board.render();
        for (Piece* p : Piece::PieceArray)
            p->render();


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
        GLCall(glClear(GL_DEPTH_BUFFER_BIT));
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //TODO: Fix this
	glViewport(0, 0, width, height);
    glm::mat4 projection = glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f, -1.0f, 1.0f);
    Shader1ptr->setUniformMat4fv("projection", projection);
    Boardptr->translate(0.0f, 0.0f, 0.0f);
    Boardptr->scale(height / Screen_Height);
    for (Piece* p : Piece::PieceArray)
    {
        float xPos = xToMouse(p->getGridPoint().x);
        float yPos = yToMouse(p->getGridPoint().y);
        p->snap(xPos, yPos);
    }
    Screen_Width = width;
    Screen_Height = height;
}


void Make_Move(GLFWwindow* window)
{
    bool Check = false;
    static bool allow_castling = true;
    static Piece* castling_king = nullptr;
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    static bool Turn = 1;
    if (FirstMove)
        Turn = 1;

    //Hover over piece before grabbing it
    if (!Grabbed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        int not_grabbed = 0;
        for (Piece* p : Piece::PieceArray)
        {
            if (p->checkHitbox((int)xPos, (int)yPos) && p->getColor() == Turn && p->getLegalMoves()->size())
            {
                p->scale(ObjectScaler1 * 1.2f, ObjectScaler1 * 1.2f);
                Piece::GrabbedPieceIndex = p->index;
                //for checking if castling will be performed
                if (p->getType() == nKing && p->Castle)
                {
                    castling_king = p;
                    allow_castling = true;
                }
                else
                    allow_castling = false;
            }

            else
            {
                not_grabbed++;
                p->scale(ObjectScaler1, ObjectScaler1);
            }
        }
        if (not_grabbed == Piece::PieceArray.size())
                Piece::GrabbedPieceIndex = -1;
    }
    //grabbing / dragging a piece
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && Piece::GrabbedPieceIndex > -1)
    {
        PieceToCursor(Piece::GrabbedPieceIndex, (float)xPos, (float)yPos);
    }
    //letting a Piece go
    else if (Grabbed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        if (PieceReleased(Piece::GrabbedPieceIndex, (float)xPos, (float)yPos))
        {
            if (FirstMove)
            {
                FirstMove = 0;
                GameInProgress = 1;
            }

            if (Turn)
                Turn = 0;
            else
                Turn = 1;    
        }
        //executing castles
        if (castling_king && allow_castling)
        {
            if (castling_king->getGridPoint().x == Piece::Grid.origin.x + 2.5f * Piece::Grid.SquareDiameter)
                Execute_Castles(!Turn, true);
            else if (castling_king->getGridPoint().x == Piece::Grid.origin.x - 1.5f * Piece::Grid.SquareDiameter)
                Execute_Castles(!Turn, false);
        }
        //checking if special moves have been played or have become possible
        Case_Capture(Turn, Piece::GrabbedPieceIndex);
        Case_EnPassant(Turn, Piece::GrabbedPieceIndex);
        Case_Promotion(Piece::GrabbedPieceIndex);
        if (Case_Check(Turn))
            Check = true;
        else
            Case_Castles(Turn);
        Case_Pins(Turn);
        Piece::GrabbedPieceIndex = -1;
        Grabbed = false;
    }
    End_Game(Turn);
}

void PieceToCursor(int index, float xPos, float yPos)
{
    Piece* p = Piece::PieceArray.at(index);
    p->translate(xPos - Screen_Width / 2, -yPos + Screen_Height / 2, 0.2f);
    p->scale(ObjectScaler1 * 1.2f, ObjectScaler1 * 1.2f);
    Grabbed = true;
    Piece::GrabbedPieceIndex = index;
}

bool PieceReleased(int index, float xPos, float yPos)
{
    float X = xPos;
    float Y = yPos;
    Piece* p = Piece::PieceArray.at(index);
    float originX = p->getGridPoint().x;
    float originY = p->getGridPoint().y;
    p->snap(X, Y);
    p->translate(X, Y, 0.1f);

    if (X != originX || Y != originY)
    {
        //for castling
        p->Castle = false;
        p->FirstMove = false;

        return 1;
    }
    else 
        return 0;
}


bool Case_Check(bool turn)
{
    //search for King of Color of next turn, save it
    //Piece Dont_Bitch_Around_Dear_Compiler(Shader1ptr, &VertexArray(), &Texture(), 0, 6, glm::vec2(0.0f, 6000.0f));
    Piece* King_of_interest = nullptr;
    std::vector<Piece*> checkGivingPieces;
    for (Piece* p : Piece::PieceArray)
    {
        King_of_interest = p;
        if (p->getType() == nKing && p->getColor() == turn)
            break;
    }
    
    if (!King_of_interest)
    {
        std::cerr << "ERROR::Case_Check::NO KING FOUND" << std::endl;
        return false;
    }
    //search for legal moves of opposite pieces, as they have been updated before
    //if position of King is equal to a legal move, set check true
    for (Piece* p : Piece::PieceArray)
        if (p->getColor() != turn)
            for (unsigned int v = 0; v < p->getLegalMoves()->size(); v++)
                if (p->getLegalMoves()->at(v).x == King_of_interest->getGridPoint().x &&
                    p->getLegalMoves()->at(v).y == King_of_interest->getGridPoint().y)
                    checkGivingPieces.push_back(p);
 


    //if check, then erase all moves of color to move and return true
    if (checkGivingPieces.size())
    {
        for (Piece* p : Piece::PieceArray)
            if (p->getColor() == turn)
            {
    //unless in case of double check, delete all moves that aren't King moves
                if (checkGivingPieces.size() > 1 && p->getType() != nKing)
                {
                    p->getLegalMoves()->clear();
                    continue;
                }
               
                unsigned int v = 0;
                while (v < p->getLegalMoves()->size())
                {
    //unless the move can capture the checkgiving piece and there is only one checkgiving piece
                    if (p->getLegalMoves()->at(v) == checkGivingPieces[0]->getGridPoint())
                    {
                        v++; continue;
                    }

    //unless the king can move out of check that way
                    if (p->getType() == nKing)
                    {
                        //check if move of king moves into check via legal moves
                        for (unsigned int i = 0; i < checkGivingPieces[0]->getLegalMoves()->size(); i++)
                        {
                            if (p->getLegalMoves()->at(v) == checkGivingPieces[0]->getLegalMoves()->at(i))
                                goto Del;
                            else
                                continue;
                        }

                        //check if king moves into check via check line
                        for (unsigned int i = 0; i < checkGivingPieces[0]->getCheckLine(King_of_interest->getGridPoint()).size(); i++)
                        {
                            if (checkGivingPieces[0]->getType() != nKnight
                                && checkGivingPieces[0]->getType() != nPawn)
                            {
                                if (p->getLegalMoves()->at(v) ==
                                    checkGivingPieces[0]->getCheckLine(King_of_interest->getGridPoint()).at(i))
                                    goto Del;
                                else
                                    continue;
                            }
                            else
                                break;
                        }
                        v++;
                        continue;
                    }

    //unless the move can block the check
                    if (checkGivingPieces[0]->getType() != nKnight
                        && checkGivingPieces[0]->getType() != nPawn)
                    {
                        bool Cont1 = false;
                        bool Cont2 = false;
                        for (glm::vec2 l : checkGivingPieces[0]->getCheckLine(King_of_interest->getGridPoint()))
                            if (p->getLegalMoves()->at(v) == l)
                            {
                                Cont1 = true;
                                break;
                            }
                        for (unsigned int l = 0; l < checkGivingPieces.at(0)->getLegalMoves()->size(); l++)
                            if (p->getLegalMoves()->at(v) == checkGivingPieces[0]->getLegalMoves()->at(l))
                            {
                                Cont2 = true;
                                break;
                            }
                        if (Cont1 && Cont2)
                        {
                            v++; continue;
                        }
                    }

                    //default is erase
                    Del:
                        p->eraseLegalMove(v);

                }
            }
        return true;
    }
    //else, return false
    else return false;
}

void Case_Pins(bool turn)
{
    //check if any piece of color to move is between the king of the same color
    //and an opposing piece of type nBishop, nQueen or nRook
    //get position of King
    Piece* King_of_interest = nullptr;
    std::vector<Piece*> potentiallyPinningPieces;
    for (Piece* p : Piece::PieceArray)
    {
        King_of_interest = p;
        if (p->getType() == nKing && p->getColor() == turn)
            break;
    }

    if (!King_of_interest)
    {
        std::cerr << "ERROR::Case_Pins::NO KING FOUND" << std::endl;
        return;
    }
    
    //first check if any opposing pieces xray hits the king, push them into vector
    for (Piece* p : Piece::PieceArray)
        if (p->getColor() != turn)
            if (p->getType() == nBishop || p->getType() == nQueen || p->getType() == nRook)
                for (glm::vec2 v : p->getCheckLine(King_of_interest->getGridPoint()))
                    if (King_of_interest->getGridPoint() == v)
                        potentiallyPinningPieces.push_back(p);

    //for each potentially pinning piece:
    //make vector of squares in checkline leading up to the king
    for (Piece* p : potentiallyPinningPieces)
    {
        std::vector<glm::vec2> pinningLine;
        for (glm::vec2 v : p->getCheckLine(King_of_interest->getGridPoint()))
        {
            if (v != King_of_interest->getGridPoint())
                pinningLine.push_back(v);
            else
                break;
        }

    //next push back every allied piece on that line
        std::vector<Piece*> potentiallyPinnedPieces;
        for (Piece* p : Piece::PieceArray)
            if (p->getColor() == turn)
                for (glm::vec2 v : pinningLine)
                    if (p->getGridPoint() == v)
                        potentiallyPinnedPieces.push_back(p);

    //if the piece is alone on the line, erase all moves that are not on that diagonal
        if (potentiallyPinnedPieces.size() == 1)
        {
            unsigned int i = 0;
            while (i < potentiallyPinnedPieces[0]->getLegalMoves()->size())
            {
                bool Del = false;
                if (p->getGridPoint() == potentiallyPinnedPieces[0]->getLegalMoves()->at(i))
                {}
                else
                {
                    for (glm::vec2 v : pinningLine)
                    {
                        if (v == potentiallyPinnedPieces[0]->getLegalMoves()->at(i))
                        {
                            Del = false;
                            break;
                        }
                        else
                            Del = true;
                    }
                }

                if (Del)
                    potentiallyPinnedPieces[0]->eraseLegalMove(i);
                else
                    i++;
            }
        }
    }
}

bool Case_Capture(bool turn, int pieceIndex)
{
    Piece* PieceOne = Piece::PieceArray[pieceIndex];
    Piece* PieceTwo = nullptr;
    bool Capture = false;
    for (Piece* p : Piece::PieceArray)
    {
        PieceTwo = p;
        if (PieceOne->getGridPoint() == PieceTwo->getGridPoint() && PieceOne != PieceTwo)
        {
            Capture = true;
            break;
        }

        //special case for En Passant:
        else if (PieceOne->getType() == nPawn && PieceTwo->getType() == nPawn && PieceTwo->getEnPassant())
            if (PieceOne->getGridPoint().x == PieceTwo->getGridPoint().x &&
                abs(PieceOne->getGridPoint().y - PieceTwo->getGridPoint().y) == Piece::Grid.SquareDiameter)
                Capture = true;
    }

    if (Capture)
    {
        PieceTwo->~Piece();
        for (Piece* p : Piece::PieceArray)
            p->updateLegalMoves();
    }

    return Capture;
}

void Case_EnPassant(bool turn, int pieceIndex)
{
    Piece* p = Piece::PieceArray[pieceIndex];


    //for en passant:
    static bool ResetEnPassant = false;
    if (ResetEnPassant)
    {
        for (Piece* p : Piece::PieceArray)
            p->setEnPassant(false);
        ResetEnPassant = false;
    }

    //for En Passant:
    if (p->getType() == nPawn && p->FirstMove)
    {
        if (p->getGridPoint().y == Piece::Grid.origin.y + 0.5f * Piece::Grid.SquareDiameter ||
            p->getGridPoint().y == Piece::Grid.origin.y - 0.5f * Piece::Grid.SquareDiameter)
        {
            p->setEnPassant(true);
            ResetEnPassant = true;
        }
    }

    for (Piece* q : Piece::PieceArray)
    {
        if (q->getColor() == turn && q->getType() == nPawn)
        {
            glm::vec2 left_capture;
            glm::vec2 right_capture;

            if (turn)
            {
                left_capture = { q->getGridPoint().x - Piece::Grid.SquareDiameter, q->getGridPoint().y + Piece::Grid.SquareDiameter };
                right_capture = { q->getGridPoint().x + Piece::Grid.SquareDiameter, q->getGridPoint().y + Piece::Grid.SquareDiameter };
            }
            else 
            {
                left_capture = { q->getGridPoint().x - Piece::Grid.SquareDiameter, q->getGridPoint().y - Piece::Grid.SquareDiameter };
                right_capture = { q->getGridPoint().x + Piece::Grid.SquareDiameter, q->getGridPoint().y - Piece::Grid.SquareDiameter };
            }

            glm::vec2 right(q->getGridPoint().x + Piece::Grid.SquareDiameter, q->getGridPoint().y);
            glm::vec2 left(q->getGridPoint().x - Piece::Grid.SquareDiameter, q->getGridPoint().y);

            //En Passant: if GridPoint is left or right respectively and EnPassant is true, push back the corresponding capture
            if (p->getGridPoint() == left && p->getEnPassant())
                q->getLegalMoves()->push_back(left_capture);
            if (p->getGridPoint() == right && p->getEnPassant())
                q->getLegalMoves()->push_back(right_capture);
        }

    }
}

void Case_Promotion(int pieceIndex)
{
    Piece* Pawn = Piece::PieceArray[pieceIndex];
    bool promotion = false;
    //check if the piece is a pawn
    if (Pawn->getType() != nPawn)
        return;

    //depending on color, check if it should be promoted
    if (Pawn->getColor() && Pawn->getGridPoint().y == Piece::Grid.origin.y + 3.5f * Piece::Grid.SquareDiameter)
        promotion = true;
    else if (!Pawn->getColor() && Pawn->getGridPoint().y == Piece::Grid.origin.y - 3.5f * Piece::Grid.SquareDiameter)
        promotion = true;

    //if promotion is true:
    if (promotion)
    {
    //go into rendering loop, where the player can select the piece he would like to promote to:
        bool Selected = false;
        int PieceType = nQueen;

    //destroy pawn and place selected piece in its place
        switch (PieceType)
        {
            case (nQueen):
            {
                VertexArray* BlackQueenVao = new VertexArray;
                VertexBuffer* BlackQueenVbo = new VertexBuffer(PieceData, sizeof(PieceData));
                IndexBuffer* BlackQueenEbo = new IndexBuffer(PieceElements, sizeof(PieceElements));
                Texture* BlackQueenTex = new Texture;
                if (Pawn->getColor())
                    BlackQueenTex->load("Textures//Chess_qlt60.png", GL_RGBA);
                else
                    BlackQueenTex->load("Textures//Chess_qdt60.png", GL_RGBA);
                BlackQueenTex->setWrapParam(GL_CLAMP_TO_EDGE);
                BlackQueenVao->attrib(3);
                BlackQueenVao->attrib(2);
                BlackQueenVao->generate();

                Queen* BlackQueen = new Queen(Shader1ptr, BlackQueenVao, BlackQueenTex, Pawn->getColor(), 6);
                BlackQueen->scale(ObjectScaler1, ObjectScaler1);
                BlackQueen->transpose(xToMouse(Pawn->getGridPoint().x), yToMouse(Pawn->getGridPoint().y), 0.1f);
            }

            default:
                break;
        }
        Pawn->~Piece();
    }
}

void Case_Castles(bool turn)
{
    //get King and first rook
    Piece* King_of_interest = nullptr;
    Piece* Rook_of_interest = nullptr;
    for (Piece* p : Piece::PieceArray)
    {
        if (p->getColor() == turn)
        {
            if (p->getType() == nKing)
                King_of_interest = p;
            if (p->getType() == nRook)
                Rook_of_interest = p;
        }
        if (King_of_interest && Rook_of_interest)
            break;
    }
    
    //safety checks: are all pointers initialized?
    if (!Rook_of_interest || !King_of_interest)
        return;

    //is the rest of the function redundant due to the pieces having lost their right to castle? (happens in Piece_Released)
    if (!King_of_interest->Castle || !Rook_of_interest->Castle)
        goto Next_Rook;

    //is there a piece or check in between the king and rook?
    for (Piece* p : Piece::PieceArray)
    {
        if (p->getGridPoint().y == King_of_interest->getGridPoint().y)
        {
            if (Rook_of_interest->getGridPoint().x < p->getGridPoint().x && p->getGridPoint().x < King_of_interest->getGridPoint().x)
                goto Next_Rook;
            else if (King_of_interest->getGridPoint().x < p->getGridPoint().x && p->getGridPoint().x < Rook_of_interest->getGridPoint().x)
                goto Next_Rook;
        }

        if (p->getColor() != turn)
            for (glm::vec2 v : *p->getLegalMoves())
            {
                if (v.y == King_of_interest->getGridPoint().y)
                {
                    if (Rook_of_interest->getGridPoint().x + Piece::Grid.SquareDiameter < v.x && v.x < King_of_interest->getGridPoint().x)
                        goto Next_Rook;
                    else if (King_of_interest->getGridPoint().x < v.x && v.x < Rook_of_interest->getGridPoint().x)
                        goto Next_Rook;
                }
            }
    }

    //which rook has been found? push back kings right to move two squares in the respective direction.
    if (Rook_of_interest->Castle && Rook_of_interest->getGridPoint().x > King_of_interest->getGridPoint().x)
        King_of_interest->getLegalMoves()->push_back(
            { King_of_interest->getGridPoint().x + Piece::Grid.SquareDiameter * 2, King_of_interest->getGridPoint().y });

    else if (Rook_of_interest->Castle && Rook_of_interest->getGridPoint().x < King_of_interest->getGridPoint().x)
        King_of_interest->getLegalMoves()->push_back(
            { King_of_interest->getGridPoint().x - Piece::Grid.SquareDiameter * 2, King_of_interest->getGridPoint().y });

    //get the next rook, if there is one
Next_Rook:
    bool Return = true;
    for (Piece* p : Piece::PieceArray)
    {
        if (p->getColor() == turn)
        {
            if (p->getType() == nRook && p->index != Rook_of_interest->index)
            {
                Rook_of_interest = p;
                Return = false;
                break;
            }
        }
    }

    if (Return)
        return;
    
    //safety checks: are all pointers initialized?
    if (!Rook_of_interest || !King_of_interest)
        return;

    //is the rest of the function redundant due to the pieces having lost their right to castle? (happens in Piece_Released)
    if (!Rook_of_interest->Castle)
       return;

    //is there a piece or check in between the king and rook?
    for (Piece* p : Piece::PieceArray)
    {
        if (p->getGridPoint().y == King_of_interest->getGridPoint().y)
        {
            if (Rook_of_interest->getGridPoint().x < p->getGridPoint().x && p->getGridPoint().x < King_of_interest->getGridPoint().x)
                return;
            else if (King_of_interest->getGridPoint().x < p->getGridPoint().x && p->getGridPoint().x < Rook_of_interest->getGridPoint().x)
                return;
        }

        if (p->getColor() != turn)
            for (glm::vec2 v : *p->getLegalMoves())
            {
                if (v.y == King_of_interest->getGridPoint().y)
                {
                    if (Rook_of_interest->getGridPoint().x + Piece::Grid.SquareDiameter < v.x && v.x < King_of_interest->getGridPoint().x)
                        return;
                    else if (King_of_interest->getGridPoint().x < v.x && v.x < Rook_of_interest->getGridPoint().x)
                        return;
                }
            }
    }

    //push back other two squares
    if (King_of_interest->Castle)
    {
        if (Rook_of_interest->Castle && Rook_of_interest->getGridPoint().x > King_of_interest->getGridPoint().x)
            King_of_interest->getLegalMoves()->push_back(
                { King_of_interest->getGridPoint().x + Piece::Grid.SquareDiameter * 2, King_of_interest->getGridPoint().y });

        else if (Rook_of_interest->Castle && Rook_of_interest->getGridPoint().x < King_of_interest->getGridPoint().x)
            King_of_interest->getLegalMoves()->push_back(
                { King_of_interest->getGridPoint().x - Piece::Grid.SquareDiameter * 2, King_of_interest->getGridPoint().y });
    }
}

void Execute_Castles(bool turn, bool long_short)
{
    //get relevant rook
    Piece* Relevant_Rook = nullptr;
    for (Piece* p : Piece::PieceArray)
    {
        Relevant_Rook = p;
        if (long_short)
        {
            if (p->getType() == nRook && p->getColor() == turn && p->getGridPoint().x > Piece::Grid.origin.x)
                break;
        }
        else
        {
            if (p->getType() == nRook && p->getColor() == turn && p->getGridPoint().x < Piece::Grid.origin.x)
                break;
        }
    }

    //long_short will be true for short castles
    if (long_short)
    {
        Relevant_Rook->translate(Relevant_Rook->getGridPoint().x - 2 * Piece::Grid.SquareDiameter, Relevant_Rook->getGridPoint().y, 0.1f);
    }
    else
    {
        Relevant_Rook->translate(Relevant_Rook->getGridPoint().x + 3 * Piece::Grid.SquareDiameter, Relevant_Rook->getGridPoint().y, 0.1f);
    }
}

void End_Game(bool turn)
{
    GameInProgress = 0;
    for (Piece* p : Piece::PieceArray)
    {
        if (p->getLegalMoves()->size() > 0 && p->getColor() == turn)
            GameInProgress = 1;
    }
}

float xToMouse(float x)
{
    float x1 = x + Screen_Width / 2;
    return x1;
}

float yToMouse(float y)
{
    float y1 = -y + Screen_Height / 2;
    return y1;
}

/*TODO:
* implement grab & move of pieces DONE
* implement snap to place   DONE
* make blending work DONE
* fix checklines (own pieces are still skipped, king can "flee") DONE
* edit checkGivers into static array + update function (all pieces flagged with checkGiver added to list) DONE
* fix "updateLegalMoves" order and make check static/more useful DONE
* Redo Check in more reasonable manner DONE
* implement pins DONE
* implement discoveries DONE
* fix bugs in opposition of kings DONE
* test/implement double checks DONE
* implement turns DONE
* create different subclasses for each piece DONE
* implement allowed snaps for different subclasses DONE
* fix pins and pawn approach DONE
* implement captures DONE
* implement pawn promotion DONE (with queen)
* fix piece creation
* implement checkmate
* implement castles
* implement menu and win/lose screen and promotion selection screen
* implement en passant
* make framebuffer_size_callback work
*/

template<typename T>
T MouseToX(T x)
{
    T returnvalue = x - Screen_Width / 2;
    return returnvalue;
}

template<typename T>
T MouseToY(T y)
{
    T y1 = -y + Screen_Height / 2;
    return y1;
}
