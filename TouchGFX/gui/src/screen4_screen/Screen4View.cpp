#include <gui/screen4_screen/Screen4View.hpp>
#include "cmsis_os2.h"
#include <cstring>
#include <touchgfx/Color.hpp>
#include <texts/TextKeysAndLanguages.hpp>

extern osMessageQueueId_t Queue1Handle;

// Định nghĩa mảng TETROMINOS với các hình Tetris
const uint8_t Screen4View::TETROMINOS[TETRIS_SHAPES][4][4] = {
    // Hình I
    {
        {0,0,0,0},
        {1,1,1,1},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Hình J 
    {
        {1,0,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Hình L
    {
        {0,0,1,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Hình O (vuông)
    {
        {1,1,0,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Hình S
    {
        {0,1,1,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Hình T
    {
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Hình Z
    {
        {1,1,0,0},
        {0,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    }
};

const touchgfx::BitmapId Screen4View::TETROMINO_IMAGES[TETRIS_SHAPES] = {
    BITMAP_BLUE_ID,         // I shape
    BITMAP_RED_ID,          // J shape  
    BITMAP_ORANGE_ID,       // L shape
    BITMAP_YELLOW_ID,       // O shape
    BITMAP_GREEN_ID,        // S shape
    BITMAP_PURPLE_ID,       // T shape
    BITMAP_BLUE_BRIGHT_ID   // Z shape
};

// Mảng bitmap cho khối tiếp theo (9PX)
const touchgfx::BitmapId Screen4View::TETROMINO_NEXT_IMAGES[TETRIS_SHAPES] = {
    BITMAP_BLUE9PX_ID,         // I shape
    BITMAP_RED9PX_ID,          // J shape  
    BITMAP_ORANGE9PX_ID,       // L shape
    BITMAP_YELLOW9PX_ID,       // O shape
    BITMAP_GREEN9PX_ID,        // S shape
    BITMAP_PURPLE9PX_ID,       // T shape
    BITMAP_BLUE_BRIGHT9PX_ID   // Z shape
};

//Mảng các khối cố đinh
const uint8_t Screen4View::TETROMINOS_INIT[TETRIS_SHAPES_INIT][4][4] = {
    // Hình I - ngang
    {
        {0,0,0,0},
        {1,1,1,1},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Hình I - dọc
    {
        {1,0,0,0},
        {1,0,0,0},
        {1,0,0,0},
        {1,0,0,0}
    },
    // Hình J - 0 độ
    {
        {1,0,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Hình J - 90 độ
    {
        {1,1,0,0},
        {1,0,0,0},
        {1,0,0,0},
        {0,0,0,0}
    },
    // Hình J - 180 độ
    {
        {1,1,1,0},
        {0,0,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Hình J - 270 độ
    {
        {0,1,0,0},
        {0,1,0,0},
        {1,1,0,0},
        {0,0,0,0}
    },
    // Hình L - 0 độ
    {
        {0,0,1,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Hình L - 90 độ
    {
        {1,0,0,0},
        {1,0,0,0},
        {1,1,0,0},
        {0,0,0,0}
    },
    // Hình L - 180 độ
    {
        {1,1,1,0},
        {1,0,0,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Hình L - 270 độ
    {
        {1,1,0,0},
        {0,1,0,0},
        {0,1,0,0},
        {0,0,0,0}
    },
    // Hình O (vuông)
    {
        {1,1,0,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Hình S - 0 độ
    {
        {0,1,1,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Hình S - 90 độ
    {
        {1,0,0,0},
        {1,1,0,0},
        {0,1,0,0},
        {0,0,0,0}
    },
    // Hình T - 0 độ
    {
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Hình T - 90 độ
    {
        {1,0,0,0},
        {1,1,0,0},
        {1,0,0,0},
        {0,0,0,0}
    },
    // Hình T - 180 độ
    {
        {1,1,1,0},
        {0,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Hình T - 270 độ
    {
        {0,1,0,0},
        {1,1,0,0},
        {0,1,0,0},
        {0,0,0,0}
    },
    // Hình Z - 0 độ
    {
        {1,1,0,0},
        {0,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Hình Z - 90 độ
    {
        {0,1,0,0},
        {1,1,0,0},
        {1,0,0,0},
        {0,0,0,0}
    }
};

const int Screen4View::INIT_BLOCKS[NUM_FIXED_BLOCKS][3] = {
    // X, Y,   Shape
    {20,  265,  3 },   
    {35,  235,  6 },   
    {50,  265,  11},   
    {65,  280,  0 },   
    {95,  235,  16},   
    {110, 280,  15},   
    {125, 205,  7 },
    {140, 250,  12},
};

const uint8_t Screen4View::INIT_BLOCKS_TYPE[TETRIS_SHAPES_INIT] = {
    0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 4, 4, 5, 5, 5, 5, 6, 6
};

Screen4View::Screen4View() : 
    currentX(80),
    currentY(10),
    currentShape(0),
    nextShape(1),  
    tickCount(0),
    isGameOver(false),
    isWin(false),
    bagIndex(0),
    randomSeed(1324)
{
    // Khởi tạo Bag Randomizer
    initializeBag();
    nextShape = getNextShape();

    memset(board, 0, sizeof(board));
    
    memcpy(currentTetromino, TETROMINOS[currentShape], sizeof(currentTetromino));
    
    drawTetromino(currentShape, currentX, currentY);

    drawNextTetromino();

    for(int i = 0; i < NUM_FIXED_BLOCKS; i++) {
        int blockX = INIT_BLOCKS[i][0];
        int blockY = INIT_BLOCKS[i][1];
        int shape = INIT_BLOCKS[i][2];
        for(int y = 0; y < 4; y++) {
            for(int x = 0; x < 4; x++) {
                if(TETROMINOS_INIT[shape][y][x]) {
                    int boardX = (blockX - 20) / BLOCK_SIZE + x;
                    int boardY = (blockY - 10) / BLOCK_SIZE + y;
                    board[boardY][boardX] = true;
                    staticBlocks[boardY][boardX].setXY(
                        blockX + (x * BLOCK_SIZE),
                        blockY + (y * BLOCK_SIZE)
                    );
                    staticBlocks[boardY][boardX].setBitmap(Bitmap(TETROMINO_IMAGES[INIT_BLOCKS_TYPE[shape]]));
                    add(staticBlocks[boardY][boardX]);
                }
            }
        }
    }

    score = 0;
    Unicode::snprintf(textArea1Buffer, sizeof(textArea1Buffer), "%d", score);

    textArea1.setWildcard(textArea1Buffer);
    textArea1.setTypedText(touchgfx::TypedText(T_TOADOX));
    textArea1.invalidate();

    targetScore = 300;
    Unicode::snprintf(textArea3Buffer, sizeof(textArea3Buffer), "%d", targetScore);

    textArea3.setWildcard(textArea3Buffer);
    textArea3.setTypedText(touchgfx::TypedText(T_TARGET4));
    textArea3.invalidate();
}

void Screen4View::setupScreen()
{
    Screen4ViewBase::setupScreen();
}

void Screen4View::tearDownScreen()
{
    Screen4ViewBase::tearDownScreen();
}

void Screen4View::drawTetromino(int shapeIndex, int startX, int startY)
{
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            remove(blocks[y][x]);
        }
    }

    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            if(currentTetromino[y][x] == 1) {
                blocks[y][x].setXY(
                    startX + (x * BLOCK_SIZE),
                    startY + (y * BLOCK_SIZE)
                );
                blocks[y][x].setBitmap(Bitmap(TETROMINO_IMAGES[currentShape]));
                add(blocks[y][x]);
            }
        }
    }
}

void Screen4View::drawNextTetromino()
{
    const int NEXT_BLOCK_SIZE = 9; 
    
    int NEXT_START_X, NEXT_START_Y;
    
    switch(nextShape) {
        case 0: // Hình I
            NEXT_START_X = 187;
            NEXT_START_Y = 127;
            break;
        case 3: // Hình O
            NEXT_START_X = 195;
            NEXT_START_Y = 123;
            break;
        default: // Các hình còn lại (J, L, S, T, Z)
            NEXT_START_X = 192;
            NEXT_START_Y = 123;
            break;
    }
    
    // Xóa các block cũ
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            remove(nextBlocks[y][x]);
        }
    }

    // Vẽ khối tiếp theo với bitmap 9PX
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            if(TETROMINOS[nextShape][y][x] == 1) {
                nextBlocks[y][x].setXY(
                    NEXT_START_X + (x * NEXT_BLOCK_SIZE),
                    NEXT_START_Y + (y * NEXT_BLOCK_SIZE)
                );
                nextBlocks[y][x].setBitmap(Bitmap(TETROMINO_NEXT_IMAGES[nextShape]));
                add(nextBlocks[y][x]);
            }
        }
    }
}

bool Screen4View::checkCollision(int newX, int newY, int shape)
{
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            if(currentTetromino[y][x]) {
                int boardX = (newX - 20) / BLOCK_SIZE + x;
                int boardY = (newY - 10) / BLOCK_SIZE + y;
                
                // Kiểm tra chạm đáy
                if(boardY >= BOARD_HEIGHT) return true;
                
                // Kiểm tra chạm biên
                if(boardX < 0 || boardX >= BOARD_WIDTH) return true;
                
                // Kiểm tra chạm khối khác
                if(board[boardY][boardX]) return true;
            }
        }
    }
    return false;
}

void Screen4View::checkAndClearLines()
{
    int fullLines[BOARD_HEIGHT] = {0};
    int numFullLines = 0;
    
    for(int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        bool isLineFull = true;
        for(int x = 0; x < BOARD_WIDTH; x++) {
            if(!board[y][x]) {
                isLineFull = false;
                break;
            }
        }
        if(isLineFull) {
            fullLines[numFullLines++] = y;
        }
    }

    if(numFullLines > 0) {
        // Update score
        if (numFullLines == 1) {
            score += 10;
        } else if (numFullLines == 2) {
            score += 30; 
        } else if (numFullLines == 3) {
            score += 50;
        } else if (numFullLines >= 4) {
            score += 100; 
        }
        Unicode::snprintf(textArea1Buffer, sizeof(textArea1Buffer), "%d", score);

        textArea1.setWildcard(textArea1Buffer);
        textArea1.setTypedText(touchgfx::TypedText(T_TOADOX));
        textArea1.invalidate();

        for(int i = 0; i < numFullLines; i++) {
            int lineY = fullLines[i];
            for(int x = 0; x < BOARD_WIDTH; x++) {
                remove(staticBlocks[lineY][x]);
                board[lineY][x] = false;
            }
        }

        for(int y = fullLines[0]; y >= 0; y--) {
            int shiftDown = 0;
            for(int i = 0; i < numFullLines; i++) {
                if(fullLines[i] > y) shiftDown++;
            }
            
            if(shiftDown > 0) {
                for(int x = 0; x < BOARD_WIDTH; x++) {
                    if(board[y][x]) {
                        board[y + shiftDown][x] = true;
                        board[y][x] = false;
                        
                        staticBlocks[y + shiftDown][x] = staticBlocks[y][x];
                        staticBlocks[y + shiftDown][x].setXY(
                            20 + (x * BLOCK_SIZE),
                            10 + ((y + shiftDown) * BLOCK_SIZE)
                        );
                        add(staticBlocks[y + shiftDown][x]);
                        remove(staticBlocks[y][x]);
                    }
                }
            }
        }
        
        invalidate();
    }
    if (score >= targetScore) {
        isWin = true;
        clearScreen();
        image3.setVisible(true);
        flexButton1.setVisible(false);
        flexButton4.setVisible(true);
        flexButton5.setVisible(true);
        invalidate();
    }
}

void Screen4View::freezeTetromino()
{
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            if(currentTetromino[y][x]) {
                int boardX = (currentX - 20) / BLOCK_SIZE + x;
                int boardY = (currentY - 10) / BLOCK_SIZE + y;
                board[boardY][boardX] = true;
                
                staticBlocks[boardY][boardX].setXY(
                    currentX + (x * BLOCK_SIZE),
                    currentY + (y * BLOCK_SIZE)
                );
                staticBlocks[boardY][boardX].setBitmap(Bitmap(TETROMINO_IMAGES[currentShape]));
                add(staticBlocks[boardY][boardX]);
            }
        }
    }
    checkAndClearLines();
}

void Screen4View::handleTickEvent()
{
    if (isGameOver || isWin) {
        return;
    }
    tickCount++;
    if (tickCount % 25 == 12) {
        uint8_t res = 0;
        uint32_t count = osMessageQueueGetCount(Queue1Handle);
        if (count > 0)
        {
            osMessageQueueGet(Queue1Handle, &res, NULL, osWaitForever);
            if (res == 'L') 
            {
                if (!checkCollision(currentX - BLOCK_SIZE, currentY, currentShape)) 
                {
                    currentX -= BLOCK_SIZE;
                    drawTetromino(currentShape, currentX, currentY);
                }
            } 
            else if (res == 'R') 
            {
                if (!checkCollision(currentX + BLOCK_SIZE, currentY, currentShape)) 
                {
                    currentX += BLOCK_SIZE;
                    drawTetromino(currentShape, currentX, currentY);
                }
            }
            else if (res == 'D') 
            {
                while (!checkCollision(currentX, currentY + BLOCK_SIZE, currentShape)) 
                {
                    currentY += BLOCK_SIZE;
                    drawTetromino(currentShape, currentX, currentY);
                }
                freezeTetromino();
                createNewTetromino();
            }
            else if (res == 'C')
            {
                rotateTetromino();
            }
        }
    }
    if (tickCount % 50 == 0) 
    {
        if (checkCollision(currentX, currentY + BLOCK_SIZE, currentShape))
        {
            freezeTetromino();
            createNewTetromino();
        }
        else
        {
            currentY += BLOCK_SIZE;
            drawTetromino(currentShape, currentX, currentY);
        }
    }
    invalidate();
}

void Screen4View::createNewTetromino()
{
    if (isGameOver || isWin) {
        return;
    }

    currentY = 10;
    currentX = 80;
    
    currentShape = nextShape;
    nextShape = getNextShape();
    
    memcpy(currentTetromino, TETROMINOS[currentShape], sizeof(currentTetromino));

    drawNextTetromino();

    bool canCreate = true;
    for(int y = 0; y < 4 && canCreate; y++) {
        for(int x = 0; x < 4; x++) {
            if(currentTetromino[y][x]) { 
                int boardX = (currentX - 20) / BLOCK_SIZE + x;
                int boardY = (currentY - 10) / BLOCK_SIZE + y;
                
                // Kiểm tra xem vị trí này đã có khối chưa
                if(boardX >= 0 && boardX < BOARD_WIDTH && 
                   boardY >= 0 && boardY < BOARD_HEIGHT && 
                   board[boardY][boardX]) {
                    canCreate = false;
                    break;
                }
            }
        }
    }
    if (!canCreate) {
        // Game Over
        isGameOver = true;
        clearScreen();
        image2.setVisible(true);
        flexButton1.setVisible(false);
        flexButton2.setVisible(true);
        flexButton3.setVisible(true);

        Unicode::snprintf(textArea2Buffer, sizeof(textArea2Buffer), "%d", score);

        textArea2.setVisible(true);
        textArea2.setWildcard(textArea2Buffer);
        textArea2.setTypedText(touchgfx::TypedText(T_MAN3SCORE));
        textArea2.invalidate();
        invalidate();
    }
    else {
        drawTetromino(currentShape, currentX, currentY);
    }
}

void Screen4View::rotateTetromino()
{
    // Hình O không cần xoay
    if (currentShape == 3) return;
    
    uint8_t temp[4][4] = {0};
    uint8_t rotated[4][4] = {0};
    
    memcpy(temp, currentTetromino, sizeof(temp));

    // Xác định điểm trung tâm để xoay dựa vào loại hình
    int centerX, centerY;
    if (currentShape == 0) { // Hình I
        centerX = 2;
        centerY = 2;
    } else {  // Các hình khác (J, L, S, T, Z)
        centerX = 1;
        centerY = 1;
    }

    // Xoay 90 độ quanh điểm trung tâm
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            if (currentShape == 0) { // Xử lý đặc biệt cho hình I
                rotated[x][3-y] = temp[y][x];
            } else {
                // Tính toạ độ tương đối so với điểm trung tâm
                int relativeY = y - centerY;
                int relativeX = x - centerX;
                
                // Công thức xoay 90 độ: (x,y) -> (-y,x)
                int newX = centerX - relativeY;
                int newY = centerY + relativeX;
                
                // Kiểm tra giới hạn mảng
                if(newX >= 0 && newX < 4 && newY >= 0 && newY < 4) {
                    rotated[newY][newX] = temp[y][x];
                }
            }
        }
    }

    // Kiểm tra va chạm với hình đã xoay
    bool canRotate = true;
    for(int y = 0; y < 4 && canRotate; y++) {
        for(int x = 0; x < 4; x++) {
            if(rotated[y][x]) {
                int boardX = (currentX - 20) / BLOCK_SIZE + x;
                int boardY = (currentY - 10) / BLOCK_SIZE + y;
                
                if(boardX < 0 || boardX >= BOARD_WIDTH || 
                   boardY < 0 || boardY >= BOARD_HEIGHT ||
                   board[boardY][boardX]) {
                    canRotate = false;
                    break;
                }
            }
        }
    }
    
    if(canRotate) {
        // Cập nhật currentTetromino với hình đã xoay
        memcpy(currentTetromino, rotated, sizeof(currentTetromino));
        drawTetromino(currentShape, currentX, currentY);
        invalidate();
    }
}

void Screen4View::clearScreen()
{
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            remove(blocks[y][x]);
        }
    }
    
    for(int y = 0; y < BOARD_HEIGHT; y++) {
        for(int x = 0; x < BOARD_WIDTH; x++) {
            if(board[y][x]) {
                remove(staticBlocks[y][x]);
                board[y][x] = false;
            }
        }
    }

    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            remove(nextBlocks[y][x]);
        }
    }
}

// ========================= BAG RANDOMIZER IMPLEMENTATION =========================

void Screen4View::initializeBag()
{
    // Khởi tạo túi với 7 hình tetromino (0-6)
    for(int i = 0; i < BAG_SIZE; i++) {
        bag[i] = i;
    }
    shuffleBag();
    bagIndex = 0;
}

void Screen4View::shuffleBag()
{
    // Fisher-Yates shuffle algorithm
    for(int i = BAG_SIZE - 1; i > 0; i--) {
        int j = simpleRandom() % (i + 1);
        
        // Swap bag[i] và bag[j]
        uint8_t temp = bag[i];
        bag[i] = bag[j];
        bag[j] = temp;
    }
}

int Screen4View::getNextShape()
{
    // Nếu đã hết túi, khởi tạo túi mới
    if(bagIndex >= BAG_SIZE) {
        shuffleBag();
        bagIndex = 0;
    }
    
    return bag[bagIndex++];
}

uint32_t Screen4View::simpleRandom()
{
    // Linear Congruential Generator (LCG)
    // Sử dụng công thức: next = (a * seed + c) % m
    // a = 1664525, c = 1013904223, m = 2^32
    randomSeed = randomSeed * 1664525 + 1013904223 + score - 2611;
    return randomSeed;
}