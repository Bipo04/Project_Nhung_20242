#include <gui/screen1_screen/Screen1View.hpp>
#include "cmsis_os2.h"
#include <cstring>
#include <touchgfx/Color.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <cstdlib>  // Để sử dụng abs()

extern osMessageQueueId_t Queue1Handle;

// Định nghĩa mảng TETROMINOS với các hình Tetris
const uint8_t Screen1View::TETROMINOS[TETRIS_SHAPES][4][4] = {
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

const touchgfx::BitmapId Screen1View::TETROMINO_IMAGES[TETRIS_SHAPES] = {
    BITMAP_BLUE_ID,         // I shape
    BITMAP_RED_ID,          // J shape  
    BITMAP_ORANGE_ID,       // L shape
    BITMAP_YELLOW_ID,       // O shape
    BITMAP_GREEN_ID,        // S shape
    BITMAP_PURPLE_ID,       // T shape
    BITMAP_BLUE_BRIGHT_ID   // Z shape
};

// Mảng bitmap cho khối tiếp theo (9PX - nhỏ hơn)
const touchgfx::BitmapId Screen1View::TETROMINO_NEXT_IMAGES[TETRIS_SHAPES] = {
    BITMAP_BLUE9PX_ID,         // I shape
    BITMAP_RED9PX_ID,          // J shape  
    BITMAP_ORANGE9PX_ID,       // L shape
    BITMAP_YELLOW9PX_ID,       // O shape
    BITMAP_GREEN9PX_ID,        // S shape
    BITMAP_PURPLE9PX_ID,       // T shape
    BITMAP_BLUE_BRIGHT9PX_ID   // Z shape
};

Screen1View::Screen1View() : 
    currentX(80),
    currentY(10),
    currentShape(0),
    tickCount(0),
    isGameOver(false),
    bagIndex(0),
    randomSeed(2611)
{
    // Khởi tạo Bag Randomizer
    initializeBag();
    nextShape = getNextShape();
    
    // Khởi tạo board trống
    memset(board, 0, sizeof(board));
    
    // Copy hình đầu tiên vào currentTetromino
    memcpy(currentTetromino, TETROMINOS[currentShape], sizeof(currentTetromino));
    
    // Vẽ khối đang di chuyển
    drawTetromino(currentShape, currentX, currentY);

    // Vẽ khối tiếp theo
    drawNextTetromino();

    score = 0;
    Unicode::snprintf(textArea1Buffer, sizeof(textArea1Buffer), "%d", score);

    textArea1.setWildcard(textArea1Buffer);
    textArea1.setTypedText(touchgfx::TypedText(T_MAN1));
    textArea1.invalidate(); // Vẽ lại nội dung
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

// Use color array instead of images since we're using BoxWithBorder
void Screen1View::drawTetromino(int shapeIndex, int startX, int startY)
{
    // Clear old blocks
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            remove(blocks[y][x]);
        }
    }

    // Draw new blocks with corresponding image
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

void Screen1View::drawNextTetromino()
{
    const int NEXT_BLOCK_SIZE = 9; // Kích thước block nhỏ hơn (9px)
    
    // Tọa độ hiển thị khối tiếp theo tùy theo loại hình
    int NEXT_START_X, NEXT_START_Y;
    
    switch(nextShape) {
        case 0: // Hình I
            NEXT_START_X = 187;
            NEXT_START_Y = 127;
            break;
        case 3: // Hình O (vuông)
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

bool Screen1View::checkCollision(int newX, int newY, int shape)
{
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            if(currentTetromino[y][x]) {  // Thay đổi từ TETROMINOS sang currentTetromino
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

void Screen1View::checkAndClearLines()
{
    int fullLines[BOARD_HEIGHT] = {0};  // Track which lines are full
    int numFullLines = 0;
    
    // First identify all full lines from bottom up
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

    // If we found full lines
    if(numFullLines > 0) {
        if (numFullLines == 1) {
            score += 10; // 1 line cleared
        } else if (numFullLines == 2) {
            score += 30; // 2 lines cleared
        } else if (numFullLines == 3) {
            score += 50; // 3 lines cleared
        } else if (numFullLines >= 4) {
            score += 100; // 4 or more lines cleared
        }
        Unicode::snprintf(textArea1Buffer, sizeof(textArea1Buffer), "%d", score);

        textArea1.setWildcard(textArea1Buffer);
        textArea1.setTypedText(touchgfx::TypedText(T_MAN1));
        textArea1.invalidate(); // Vẽ lại nội dung

        for(int i = 0; i < numFullLines; i++) {
            int lineY = fullLines[i];
            for(int x = 0; x < BOARD_WIDTH; x++) {
                remove(staticBlocks[lineY][x]);
                board[lineY][x] = false;
            }
        }

        for(int y = fullLines[0]; y >= 0; y--) {
            // Calculate how many lines this row should move down
            int shiftDown = 0;
            for(int i = 0; i < numFullLines; i++) {
                if(fullLines[i] > y) shiftDown++;
            }
            
            if(shiftDown > 0) {
                for(int x = 0; x < BOARD_WIDTH; x++) {
                    if(board[y][x]) {
                        // Move block down by shiftDown lines
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
}

void Screen1View::freezeTetromino()
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

void Screen1View::handleTickEvent()
{
    // Nếu game over thì không xử lý tick event nữa
    if (isGameOver) {
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
                // Di chuyển khối sang trái
                if (!checkCollision(currentX - BLOCK_SIZE, currentY, currentShape)) 
                {
                    currentX -= BLOCK_SIZE;
                    drawTetromino(currentShape, currentX, currentY);
                }
            } 
            else if (res == 'R') 
            {
                // Di chuyển khối sang phải
                if (!checkCollision(currentX + BLOCK_SIZE, currentY, currentShape)) 
                {
                    currentX += BLOCK_SIZE;
                    drawTetromino(currentShape, currentX, currentY);
                }
            }
            else if (res == 'D') 
            {
                // Di chuyển khối xuống đến khi gặp vật cản
                while (!checkCollision(currentX, currentY + BLOCK_SIZE, currentShape)) 
                {
                    currentY += BLOCK_SIZE;
                    drawTetromino(currentShape, currentX, currentY);
                }
                // Đóng băng khối ngay lập tức sau khi chạm đáy
                freezeTetromino();
                createNewTetromino();
            }
            else if (res == 'C')
            {
                rotateTetromino(); // Xoay hình nếu nhận được lệnh
            }
        }
    }
    if (tickCount % 50 == 0) 
    {
        // Kiểm tra va chạm khi di chuyển xuống
        if (checkCollision(currentX, currentY + BLOCK_SIZE, currentShape))
        {
            // Đóng băng khối hiện tại
            freezeTetromino();
            // Tạo khối mới
            createNewTetromino();
        }
        else
        {
            // Di chuyển khối xuống
            currentY += BLOCK_SIZE;
            drawTetromino(currentShape, currentX, currentY);
        }
    }
    invalidate();
}

void Screen1View::createNewTetromino()
{
    if (isGameOver) {
        return;
    }
    currentY = 10;
    currentX = 80;
    
    // Sử dụng nextShape làm currentShape
    currentShape = nextShape;
    
    // Lấy hình tiếp theo từ Bag Randomizer
    nextShape = getNextShape();
    
    // Copy hình mới vào currentTetromino
    memcpy(currentTetromino, TETROMINOS[currentShape], sizeof(currentTetromino));
    
    // Cập nhật hiển thị khối tiếp theo
    drawNextTetromino();
    
    // Kiểm tra từng khối của hình muốn vẽ
    bool canCreate = true;
    for(int y = 0; y < 4 && canCreate; y++) {
        for(int x = 0; x < 4; x++) {
            if(currentTetromino[y][x]) {  // Nếu ô này có khối
                int boardX = (currentX - 20) / BLOCK_SIZE + x;
                int boardY = (currentY - 10) / BLOCK_SIZE + y;
                
                // Kiểm tra xem vị trí này đã có khối chưa
                if(boardX >= 0 && boardX < BOARD_WIDTH && 
                   boardY >= 0 && boardY < BOARD_HEIGHT && 
                   board[boardY][boardX]) {
                    // Vị trí đã bị chiếm - Game Over
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
        textArea2.setTypedText(touchgfx::TypedText(T_MAN1SCORE));
        textArea2.invalidate();
        
        invalidate();
    }
    else {
        drawTetromino(currentShape, currentX, currentY);
    }
}

void Screen1View::rotateTetromino()
{
    // Hình O không cần xoay
    if (currentShape == 3) return;
    
    uint8_t temp[4][4] = {0};
    uint8_t rotated[4][4] = {0};
    
    // Copy currentTetromino vào temp
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

void Screen1View::clearScreen()
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

void Screen1View::initializeBag()
{
    // Khởi tạo túi với 7 hình tetromino (0-6)
    for(int i = 0; i < BAG_SIZE; i++) {
        bag[i] = i;
    }
    shuffleBag();
    bagIndex = 0;
}

void Screen1View::shuffleBag()
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

int Screen1View::getNextShape()
{
    // Nếu đã hết túi, khởi tạo túi mới
    if(bagIndex >= BAG_SIZE) {
        shuffleBag();
        bagIndex = 0;
    }
    
    return bag[bagIndex++];
}

uint32_t Screen1View::simpleRandom()
{
    // Linear Congruential Generator (LCG)
    // Sử dụng công thức: next = (a * seed + c) % m
    // a = 1664525, c = 1013904223, m = 2^32
    randomSeed = randomSeed * 1664525 + 1013904223 + score - 2611;
    return randomSeed;
}