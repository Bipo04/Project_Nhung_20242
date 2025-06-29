#include <gui/screen3_screen/Screen3View.hpp>
#include "cmsis_os2.h"
#include <cstring>
#include <touchgfx/Color.hpp>
#include <texts/TextKeysAndLanguages.hpp>

extern osMessageQueueId_t Queue1Handle;

// Định nghĩa mảng TETROMINOS với các hình Tetris
const uint8_t Screen3View::TETROMINOS[TETRIS_SHAPES][4][4] = {
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

const touchgfx::BitmapId Screen3View::TETROMINO_IMAGES[TETRIS_SHAPES] = {
    BITMAP_BLUE_ID,         // I shape
    BITMAP_RED_ID,          // J shape  
    BITMAP_ORANGE_ID,       // L shape
    BITMAP_YELLOW_ID,       // O shape
    BITMAP_GREEN_ID,        // S shape
    BITMAP_PURPLE_ID,       // T shape
    BITMAP_BLUE_BRIGHT_ID   // Z shape
};

const int NUM_FIXED_BLOCKS = 7;
const int BOTTOM_Y = 285;
const int MIDDLE_Y = 200;

// Mảng chứa thông tin khối cố định: x, y, shape
const int Screen3View::INIT_BLOCKS[NUM_FIXED_BLOCKS][3] = {
    // X,    Y,    Shape
    {20,   250,    0},   // I shape - bottom row
    {65,   280,    6},   // J shape
    {110,  280,    2},   // L shape
    {110,  265,    4},   // S shape
    {20,   280,    3},   // O shape - middle row
    {20,   235,    5},   // T shape
    {125,  235,    6}    // Z shape
};

Screen3View::Screen3View() : 
    currentX(80),
    currentY(10),
    currentShape(0),
    tickCount(0)
{
    // Khởi tạo board trống
    memset(board, 0, sizeof(board));
    
    // Copy hình đầu tiên vào currentTetromino
    memcpy(currentTetromino, TETROMINOS[currentShape], sizeof(currentTetromino));
    
    // Vẽ khối đang di chuyển
    drawTetromino(currentShape, currentX, currentY);

    // Đặt các khối cố định vào board
    for(int i = 0; i < 7; i++) {
        int blockX = INIT_BLOCKS[i][0];
        int blockY = INIT_BLOCKS[i][1];
        int shape = INIT_BLOCKS[i][2];
        // Đặt từng ô của khối vào board
        for(int y = 0; y < 4; y++) {
            for(int x = 0; x < 4; x++) {
                if(TETROMINOS[shape][y][x]) {
                    int boardX = (blockX - 20) / BLOCK_SIZE + x;
                    int boardY = (blockY - 10) / BLOCK_SIZE + y;
                    board[boardY][boardX] = true;
                    staticBlocks[boardY][boardX].setXY(
                        blockX + (x * BLOCK_SIZE),
                        blockY + (y * BLOCK_SIZE)
                    );
                    staticBlocks[boardY][boardX].setBitmap(Bitmap(TETROMINO_IMAGES[shape]));
                    add(staticBlocks[boardY][boardX]);
                }
            }
        }
    }
    score = 0;
    Unicode::snprintf(textArea1Buffer, sizeof(textArea1Buffer), "%d", score);

    textArea1.setWildcard(textArea1Buffer);
    textArea1.setTypedText(touchgfx::TypedText(T_TOADOX));
    textArea1.invalidate(); // Vẽ lại nội dung};
}

void Screen3View::setupScreen()
{
    Screen3ViewBase::setupScreen();
}

void Screen3View::tearDownScreen()
{
    Screen3ViewBase::tearDownScreen();
}

// Use color array instead of images since we're using BoxWithBorder
void Screen3View::drawTetromino(int shapeIndex, int startX, int startY)
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

bool Screen3View::checkCollision(int newX, int newY, int shape)
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

void Screen3View::checkAndClearLines()
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
        // Update score
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
        textArea1.setTypedText(touchgfx::TypedText(T_TOADOX));
        textArea1.invalidate(); // Vẽ lại nội dung};
        // First remove all full lines
        for(int i = 0; i < numFullLines; i++) {
            int lineY = fullLines[i];
            for(int x = 0; x < BOARD_WIDTH; x++) {
                remove(staticBlocks[lineY][x]);
                board[lineY][x] = false;
            }
        }

        // Then move remaining blocks down
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

void Screen3View::moveLinesDown(int clearedLine)
{
    // Di chuyển các hàng phía trên xuống
    for(int y = clearedLine; y > 0; y--) {
        for(int x = 0; x < BOARD_WIDTH; x++) {
            board[y][x] = board[y-1][x];
            if(board[y][x]) {
                staticBlocks[y][x].setPosition(
                    20 + (x * BLOCK_SIZE),
                    10 + (y * BLOCK_SIZE),
                    BLOCK_SIZE,
                    BLOCK_SIZE
                );
                add(staticBlocks[y][x]);
            }
        }
    }
    
    // Xóa hàng trên cùng
    for(int x = 0; x < BOARD_WIDTH; x++) {
        board[0][x] = false;
    }
}

void Screen3View::freezeTetromino()
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

void Screen3View::handleTickEvent()
{
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
        
        invalidate();
    }
}

void Screen3View::createNewTetromino()
{
    currentY = 10;
    currentX = 80;
    currentShape = (currentShape + 1) % TETRIS_SHAPES;
    // Copy hình mới vào currentTetromino
    memcpy(currentTetromino, TETROMINOS[currentShape], sizeof(currentTetromino));
    
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
        // Game Over - có thể thêm xử lý game over ở đây
        application().gotoScreen2ScreenNoTransition(); // Hàm này không tồn tại
        // Tạm thời comment lại, có thể sử dụng gotoScreen1ScreenNoTransition() hoặc reset game
    }
    else {
        // Nếu có thể tạo hình mới, vẽ nó
        drawTetromino(currentShape, currentX, currentY);
    }
}

void Screen3View::rotateTetromino()
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
