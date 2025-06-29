#ifndef SCREEN4VIEW_HPP
#define SCREEN4VIEW_HPP

#include <gui_generated/screen4_screen/Screen4ViewBase.hpp>
#include <gui/screen4_screen/Screen4Presenter.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/BoxWithBorder.hpp>
#include <touchgfx/Bitmap.hpp>
#include <images/BitmapDatabase.hpp>

class Screen4View : public Screen4ViewBase
{
public:
    Screen4View();
    virtual ~Screen4View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

protected:
    // Constants
    static const int BLOCK_SIZE = 15;
    static const int BOARD_WIDTH = 10;
    static const int BOARD_HEIGHT = 20;
    static const int TETRIS_SHAPES = 7;
    static const int NUM_FIXED_BLOCKS = 10;
    
    // Fixed block definitions - add this
    static const int INIT_BLOCKS[NUM_FIXED_BLOCKS][3];

    // Arrays and variables
    static const uint8_t TETROMINOS[TETRIS_SHAPES][4][4];
    static const uint32_t TETROMINO_COLORS[TETRIS_SHAPES];
    static const touchgfx::BitmapId TETROMINO_IMAGES[TETRIS_SHAPES];
    static const touchgfx::BitmapId TETROMINO_NEXT_IMAGES[TETRIS_SHAPES];  // Bitmap cho next piece
    static const int fixedBlocks[NUM_FIXED_BLOCKS][3];
    
    bool board[BOARD_HEIGHT][BOARD_WIDTH];
    touchgfx::Image blocks[4][4];
    touchgfx::Image staticBlocks[BOARD_HEIGHT][BOARD_WIDTH];
    touchgfx::Image nextBlocks[4][4];  // Blocks cho hiển thị next piece
    uint8_t currentTetromino[4][4];
    
    int currentX;
    int currentY;
    int currentShape;
    int nextShape;  // Shape tiếp theo
    uint32_t tickCount;
    int score;
    bool isGameOver;
    bool isWin;

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