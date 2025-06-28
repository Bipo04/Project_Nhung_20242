#ifndef __PAJ7620_H__
#define __PAJ7620_H__

#include "stm32f4xx_hal.h" // Thư viện HAL của STM32
#include <stdbool.h>      // Cho kiểu bool
#include <stdint.h>       // Cho các kiểu int cố định

// Định nghĩa I2C Handle của bạn (ví dụ: hi2c3)
// Bạn cần khai báo hi2c3 trong main.c sau khi khởi tạo I2C trong CubeMX
extern I2C_HandleTypeDef hi2c3; // Thay thế hi2c3 bằng tên I2C handle của bạn nếu khác

/**
  * @brief Địa chỉ I2C của thiết bị PAJ7620U2
  */
#define PAJ7620_I2C_BUS_ADDR      0x73 << 1 // Địa chỉ 7-bit, cần shift trái 1 bit cho địa chỉ 8-bit HAL

/**
  * @brief Các hằng số định nghĩa thanh ghi (Register Addresses)
  * Được lấy từ datasheet và file RevEng_PAJ7620.h gốc
  */
#define PAJ7620_ADDR_BASE             0x00
#define PAJ7620_REGISTER_BANK_SEL     (PAJ7620_ADDR_BASE + 0xEF)

// REGISTERS BANK 0
#define PAJ7620_ADDR_PART_ID_0        (PAJ7620_ADDR_BASE + 0x00)
#define PAJ7620_ADDR_PART_ID_1        (PAJ7620_ADDR_BASE + 0x01)
#define PAJ7620_ADDR_GES_PS_DET_FLAG_0  (PAJ7620_ADDR_BASE + 0x43) // Readonly
#define PAJ7620_ADDR_GES_PS_DET_FLAG_1  (PAJ7620_ADDR_BASE + 0x44) // Readonly
#define PAJ7620_ADDR_WAVE_COUNT       (PAJ7620_ADDR_BASE + 0xB7)
#define PAJ7620_ADDR_NO_OBJECT_COUNT  (PAJ7620_ADDR_BASE + 0xB8)
#define PAJ7620_ADDR_NO_MOTION_COUNT  (PAJ7620_ADDR_BASE + 0xB9)
#define PAJ7620_ADDR_OBJECT_BRIGHTNESS (PAJ7620_ADDR_BASE + 0xB0)
#define PAJ7620_ADDR_OBJECT_SIZE_LSB  (PAJ7620_ADDR_BASE + 0xB1)
#define PAJ7620_ADDR_OBJECT_SIZE_MSB  (PAJ7620_ADDR_BASE + 0xB2)
#define PAJ7620_ADDR_OBJECT_CENTER_X_LSB (PAJ7620_ADDR_BASE + 0xAC)
#define PAJ7620_ADDR_OBJECT_CENTER_X_MSB (PAJ7620_ADDR_BASE + 0xAD)
#define PAJ7620_ADDR_OBJECT_CENTER_Y_LSB (PAJ7620_ADDR_BASE + 0xAE)
#define PAJ7620_ADDR_OBJECT_CENTER_Y_MSB (PAJ7620_ADDR_BASE + 0xAF)
#define PAJ7620_ADDR_OBJECT_VEL_X_LSB (PAJ7620_ADDR_BASE + 0xC3)
#define PAJ7620_ADDR_OBJECT_VEL_X_MSB (PAJ7620_ADDR_BASE + 0xC4)
#define PAJ7620_ADDR_OBJECT_VEL_Y_LSB (PAJ7620_ADDR_BASE + 0xC5)
#define PAJ7620_ADDR_OBJECT_VEL_Y_MSB (PAJ7620_ADDR_BASE + 0xC6)

// REGISTERS BANK 1
#define PAJ7620_ADDR_OPERATION_ENABLE (PAJ7620_ADDR_BASE + 0x72)
#define PAJ7620_ADDR_R_IDLE_TIME_0    (PAJ7620_ADDR_BASE + 0x65)

/**
  * @brief Giá trị ID phần cứng của thiết bị
  */
#define PAJ7620_PART_ID_LSB           0x20
#define PAJ7620_PART_ID_MSB           0x76

/**
  * @brief Lệnh điều khiển hoạt động
  */
#define PAJ7620_ENABLE                0x01
#define PAJ7620_DISABLE               0x00

/**
  * @brief Chế độ FPS của cảm biến
  */
#define PAJ7620_NORMAL_SPEED          0xAC // 120 fps
#define PAJ7620_GAME_SPEED            0x30 // 240 fps

/**
  * @brief Định nghĩa các kết quả cử chỉ
  */
typedef enum {
    GES_NONE = 0,
    GES_UP,
    GES_DOWN,
    GES_LEFT,
    GES_RIGHT,
    GES_FORWARD,
    GES_BACKWARD,
    GES_CLOCKWISE,
    GES_ANTICLOCKWISE,
    GES_WAVE
} PAJ7620_Gesture_t;

/**
  * @brief Bit mask cho các cờ cử chỉ đọc từ thanh ghi
  */
#define GES_UP_FLAG             0x01
#define GES_DOWN_FLAG           0x02
#define GES_LEFT_FLAG           0x04
#define GES_RIGHT_FLAG          0x08
#define GES_FORWARD_FLAG        0x10
#define GES_BACKWARD_FLAG       0x20
#define GES_CLOCKWISE_FLAG      0x40
#define GES_ANTI_CLOCKWISE_FLAG 0x80
#define GES_WAVE_FLAG           0x01 // Read from Bank0 - 0x44


/**
  * @brief Cấu trúc dữ liệu để khởi tạo và cấu hình các thanh ghi
  * Được lấy từ file RevEng_PAJ7620.h gốc
  */
typedef struct {
    uint8_t address;
    uint8_t value;
} PAJ7620_RegSetting_t;

// Mảng khởi tạo ban đầu (chế độ cử chỉ)
// Điều chỉnh các giá trị để phù hợp với việc bạn chỉ muốn chế độ cử chỉ
// Các mảng này không cần PROGMEM trong STM32 vì bộ nhớ Flash lớn hơn
static const PAJ7620_RegSetting_t initRegisterArray[] = {
    {0xEF, 0x00}, // Bank 0
    {0x41, 0x00}, // Disable interrupts for first 8 gestures
    {0x42, 0x00}, // Disable wave (and other modes') interrupt(s)
    {0x37, 0x07},
    {0x38, 0x17},
    {0x39, 0x06},
    {0x42, 0x01},
    {0x46, 0x2D},
    {0x47, 0x0F},
    {0x48, 0x3C},
    {0x49, 0x00},
    {0x4A, 0x1E},
    {0x4C, 0x22},
    {0x51, 0x10},
    {0x5E, 0x10},
    {0x60, 0x27},
    {0x80, 0x42},
    {0x81, 0x44},
    {0x82, 0x04},
    {0x8B, 0x01},
    {0x90, 0x06},
    {0x95, 0x0A},
    {0x96, 0x0C},
    {0x97, 0x05},
    {0x9A, 0x14},
    {0x9C, 0x3F},
    {0xA5, 0x19},
    {0xCC, 0x19},
    {0xCD, 0x0B},
    {0xCE, 0x13},
    {0xCF, 0x64},
    {0xD0, 0x21},
    {0xEF, 0x01}, // Bank 1
    {0x02, 0x0F},
    {0x03, 0x10},
    {0x04, 0x02},
    {0x25, 0x01},
    {0x27, 0x39},
    {0x28, 0x7F},
    {0x29, 0x08},
    {0x3E, 0xFF},
    {0x5E, 0x3D},
    {0x65, 0x96}, // R_IDLE_TIME LSB - Set sensor speed to 'normal speed' - 120 fps
    {0x67, 0x97},
    {0x69, 0xCD},
    {0x6A, 0x01},
    {0x6D, 0x2C},
    {0x6E, 0x01},
    {0x72, 0x01},
    {0x73, 0x35},
    {0x74, 0x00}, // Set to gesture mode
    {0x77, 0x01},
    {0xEF, 0x00}, // Bank 0
    {0x41, 0xFF}, // Re-enable interrupts for first 8 gestures
    {0x42, 0x01}  // Re-enable interrupts for wave gesture
};
#define INIT_REG_ARRAY_SIZE (sizeof(initRegisterArray)/sizeof(PAJ7620_RegSetting_t))

// Mảng cấu hình chế độ cử chỉ
static const PAJ7620_RegSetting_t setGestureModeRegisterArray[] = {
    {0xEF, 0x00}, // Bank 0
    {0x41, 0x00}, // Disable interrupts for first 8 gestures
    {0x42, 0x00}, // Disable wave (and other mode's) interrupt(s)
    {0x48, 0x3C},
    {0x49, 0x00},
    {0x51, 0x10},
    {0x83, 0x20},
    {0x9f, 0xf9},
    {0xEF, 0x01}, // Bank 1
    {0x01, 0x1E},
    {0x02, 0x0F},
    {0x03, 0x10},
    {0x04, 0x02},
    {0x41, 0x40},
    {0x43, 0x30},
    {0x65, 0x96}, // R_IDLE_TIME  - Normal mode LSB "120 fps" (supposedly)
    {0x66, 0x00},
    {0x67, 0x97},
    {0x68, 0x01},
    {0x69, 0xCD},
    {0x6A, 0x01},
    {0x6b, 0xb0},
    {0x6c, 0x04},
    {0x6D, 0x2C},
    {0x6E, 0x01},
    {0x74, 0x00}, // Set gesture mode
    {0xEF, 0x00}, // Bank 0
    {0x41, 0xFF}, // Re-enable interrupts for first 8 gestures
    {0x42, 0x01}  // Re-enable interrupts for wave gesture
};
#define SET_GES_MODE_REG_ARRAY_SIZE (sizeof(setGestureModeRegisterArray)/sizeof(PAJ7620_RegSetting_t))


/**
  * @brief Hàm khởi tạo module PAJ7620U2.
  * @param None
  * @retval HAL_StatusTypeDef: HAL_OK nếu thành công, lỗi nếu không.
  */
HAL_StatusTypeDef PAJ7620_Init(void);

/**
  * @brief Đặt cảm biến vào chế độ cử chỉ.
  * @param None
  * @retval None
  */
void PAJ7620_SetGestureMode(void);

/**
  * @brief Đọc cử chỉ hiện tại từ cảm biến.
  * @param None
  * @retval PAJ7620_Gesture_t: Cử chỉ được phát hiện (ví dụ: GES_UP), hoặc GES_NONE nếu không có.
  */
PAJ7620_Gesture_t PAJ7620_ReadGesture(void);

/**
  * @brief Đặt tốc độ lấy mẫu cảm biến thành 120fps (chế độ bình thường).
  * @param None
  * @retval None
  */
void PAJ7620_SetNormalSpeed(void);

/**
  * @brief Đặt tốc độ lấy mẫu cảm biến thành 240fps (chế độ chơi game).
  * @param None
  * @retval None
  */
void PAJ7620_SetGameSpeed(void);

/**
  * @brief Xóa các cờ ngắt cử chỉ.
  * @param None
  * @retval None
  */
void PAJ7620_ClearGestureInterrupts(void);

/**
  * @brief Lấy số lần vẫy tay được cảm biến phát hiện.
  * @param None
  * @retval int: Số lần vẫy (0-15).
  */
int PAJ7620_GetWaveCount(void);

/**
  * @brief Kiểm tra xem có đối tượng nào trong tầm nhìn của cảm biến hay không.
  * @param None
  * @retval bool: true nếu có đối tượng, false nếu không.
  */
bool PAJ7620_IsObjectInView(void);

/**
  * @brief Lấy độ sáng của đối tượng được phát hiện.
  * @param None
  * @retval int: Giá trị độ sáng (0-255).
  */
int PAJ7620_GetObjectBrightness(void);

/**
  * @brief Lấy kích thước của đối tượng được phát hiện.
  * @param None
  * @retval int: Giá trị kích thước (0-900).
  */
int PAJ7620_GetObjectSize(void);

/**
  * @brief Lấy tọa độ X của tâm đối tượng.
  * @param None
  * @retval int: Tọa độ X.
  */
int PAJ7620_GetObjectCenterX(void);

/**
  * @brief Lấy tọa độ Y của tâm đối tượng.
  * @param None
  * @retval int: Tọa độ Y.
  */
int PAJ7620_GetObjectCenterY(void);

/**
  * @brief Lấy vận tốc X của đối tượng.
  * @param None
  * @retval int: Vận tốc X.
  */
int PAJ7620_GetObjectVelocityX(void);

/**
  * @brief Lấy vận tốc Y của đối tượng.
  * @param None
  * @retval int: Vận tốc Y.
  */
int PAJ7620_GetObjectVelocityY(void);


#endif // __PAJ7620_H__
