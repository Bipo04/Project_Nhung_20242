#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/BoxWithBorder.hpp>
#include <touchgfx/Bitmap.hpp>
#include <images/BitmapDatabase.hpp>

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

protected:
    // Constants need to be defined first
    static const int BLOCK_SIZE = 15;
    static const int BOARD_WIDTH = 10;  // Moved up
    static const int BOARD_HEIGHT = 20; // Moved up
    static const int TETRIS_SHAPES = 7;
    static const int BOTTOM_MARGIN = 288;

    // Arrays and variables
    static const uint8_t TETROMINOS[TETRIS_SHAPES][4][4];
    static const uint32_t TETROMINO_COLORS[TETRIS_SHAPES];
    static const touchgfx::BitmapId TETROMINO_IMAGES[TETRIS_SHAPES];
    static const touchgfx::BitmapId TETROMINO_NEXT_IMAGES[TETRIS_SHAPES];  // Bitmap cho next piece
    
    bool board[BOARD_HEIGHT][BOARD_WIDTH];
    touchgfx::Image blocks[4][4];
    touchgfx::Image staticBlocks[BOARD_HEIGHT][BOARD_WIDTH];
    touchgfx::Image nextBlocks[4][4];  // Blocks cho hiển thị next piece
    touchgfx::BoxWithBorder scoreBlocks[4];
    uint8_t currentTetromino[4][4];
    
    int currentX;
    int currentY;
    int currentShape;
    int nextShape;  // Shape tiếp theo
    uint32_t tickCount;
    int score;
    bool isGameOver;

    // Function declarations
    void drawTetromino(int shapeIndex, int x, int y);
    void drawNextTetromino();  // Hàm vẽ khối tiếp theo
    bool checkCollision();
    bool checkCollision(int newX, int newY, int shape);
    void createNewTetromino();
    void freezeTetromino();
    void rotateTetromino();
    void checkAndClearLines();
    void moveLinesDown(int clearedLine);
    void clearScreen();

    Unicode::UnicodeChar textArea1Buffer[10];
};

#endif
