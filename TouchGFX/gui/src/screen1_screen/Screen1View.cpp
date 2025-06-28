#include <gui/screen1_screen/Screen1View.hpp>
#include "cmsis_os2.h"
#include <cstring>
#include <touchgfx/Color.hpp>

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

Screen1View::Screen1View() : 
    currentX(20),
    currentY(10),
    currentShape(3),
    tickCount(0)
{
    // Khởi tạo board trống
    memset(board, 0, sizeof(board));
    
    // Copy hình đầu tiên vào currentTetromino
    memcpy(currentTetromino, TETROMINOS[currentShape], sizeof(currentTetromino));
    
    // Vẽ khối đang di chuyển
    drawTetromino(currentShape, currentX, currentY);
    
    // Tạo 4 khối O cố định ở dưới để test xóa hàng
    int bottomY = 310 - BLOCK_SIZE - 15; // Vị trí y của hàng dưới cùng
    for(int i = 0; i < 4; i++) {
        int xPos = 50 + (i * BLOCK_SIZE * 2); // Mỗi hình O chiếm 2 ô
        
        // Vẽ hình O (2x2)
        for(int y = 0; y < 2; y++) {
            for(int x = 0; x < 2; x++) {
                int boardX = (xPos - 20) / BLOCK_SIZE + x;
                int boardY = (bottomY - 10) / BLOCK_SIZE + y;
                
                board[boardY][boardX] = true;
                staticBlocks[boardY][boardX].setXY(
                    xPos + (x * BLOCK_SIZE),
                    bottomY + (y * BLOCK_SIZE)
                );
                staticBlocks[boardY][boardX].setBitmap(Bitmap(TETROMINO_IMAGES[3])); // O shape
                add(staticBlocks[boardY][boardX]);
            }
        }
    }
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
    int linesCleared = 0;
    bool needUpdate = false;
    
    // Đầu tiên, đánh dấu tất cả các hàng đầy
    bool fullLines[BOARD_HEIGHT] = {false};
    for(int y = BOARD_HEIGHT-1; y >= 0; y--) {
        bool isLineFull = true;
        for(int x = 0; x < BOARD_WIDTH; x++) {
            if(!board[y][x]) {
                isLineFull = false;
                break;
            }
        }
        if(isLineFull) {
            fullLines[y] = true;
            linesCleared++;
        }
    }

    // Nếu có hàng cần xóa
    if(linesCleared > 0) {
        // Xử lý từng hàng từ dưới lên
        int writeY = BOARD_HEIGHT - 1;  // Vị trí đích để di chuyển các khối
        
        // Duyệt từ dưới lên để di chuyển các hàng
        for(int readY = BOARD_HEIGHT - 1; readY >= 0; readY--) {
            // Nếu là hàng đầy thì bỏ qua
            if(fullLines[readY]) {
                // Xóa các khối trong hàng đầy
                for(int x = 0; x < BOARD_WIDTH; x++) {
                    remove(staticBlocks[readY][x]);
                }
                continue;
            }
            
            // Di chuyển hàng không đầy xuống vị trí writeY
            if(writeY != readY) {
                for(int x = 0; x < BOARD_WIDTH; x++) {
                    // Copy trạng thái
                    board[writeY][x] = board[readY][x];
                    
                    if(board[writeY][x]) {
                        // Di chuyển khối xuống vị trí mới
                        staticBlocks[writeY][x] = staticBlocks[readY][x];
                        staticBlocks[writeY][x].setPosition(
                            20 + (x * BLOCK_SIZE),
                            10 + (writeY * BLOCK_SIZE),
                            BLOCK_SIZE,
                            BLOCK_SIZE
                        );
                        add(staticBlocks[writeY][x]);
                    }
                }
            }
            writeY--;
        }
        
        // Xóa các hàng trên cùng
        for(int y = 0; y <= writeY; y++) {
            for(int x = 0; x < BOARD_WIDTH; x++) {
                board[y][x] = false;
            }
        }
        
        invalidate();
    }
}

void Screen1View::moveLinesDown(int clearedLine)
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
    tickCount++;
    if (tickCount % 50 == 0) 
    {
        uint8_t res = 0;
        uint32_t count = osMessageQueueGetCount(Queue1Handle);
        if (count > 0)
        {
            osMessageQueueGet(Queue1Handle, &res, NULL, osWaitForever);
            if (res == 'P')
            {
                rotateTetromino(); // Xoay hình nếu nhận được lệnh
            }
        }
//         Kiểm tra va chạm khi di chuyển xuống
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

void Screen1View::createNewTetromino()
{
    currentY = 10;
    currentX = 20;
    currentShape = (currentShape + 1) % TETRIS_SHAPES;
    // Copy hình mới vào currentTetromino
    memcpy(currentTetromino, TETROMINOS[currentShape], sizeof(currentTetromino));
    drawTetromino(currentShape, currentX, currentY);
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
