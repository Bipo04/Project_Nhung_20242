#include "PAJ7620.h"
#include "main.h" // Bao gồm main.h để có quyền truy cập vào hi2c3 (nếu đã khai báo ở đó)

// --- Internal (Private) Functions ---

/**
  * @brief Ghi một byte dữ liệu vào thanh ghi của PAJ7620U2 qua I2C.
  * @param reg_addr: Địa chỉ thanh ghi cần ghi.
  * @param data: Dữ liệu cần ghi.
  * @retval HAL_StatusTypeDef: Trạng thái HAL_OK nếu thành công.
  */
static HAL_StatusTypeDef PAJ7620_WriteRegister(uint8_t reg_addr, uint8_t data) {
    return HAL_I2C_Mem_Write(&hi2c3, PAJ7620_I2C_BUS_ADDR, reg_addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
}

/**
  * @brief Đọc một byte dữ liệu từ thanh ghi của PAJ7620U2 qua I2C.
  * @param reg_addr: Địa chỉ thanh ghi cần đọc.
  * @param data: Con trỏ tới biến để lưu dữ liệu đọc được.
  * @retval HAL_StatusTypeDef: Trạng thái HAL_OK nếu thành công.
  */
static HAL_StatusTypeDef PAJ7620_ReadRegister(uint8_t reg_addr, uint8_t *data) {
    return HAL_I2C_Mem_Read(&hi2c3, PAJ7620_I2C_BUS_ADDR, reg_addr, I2C_MEMADD_SIZE_8BIT, data, 1, 100);
}

/**
  * @brief Đọc nhiều byte dữ liệu từ thanh ghi của PAJ7620U2 qua I2C.
  * @param reg_addr: Địa chỉ thanh ghi bắt đầu cần đọc.
  * @param pData: Con trỏ tới mảng để lưu dữ liệu đọc được.
  * @param Size: Số lượng byte cần đọc.
  * @retval HAL_StatusTypeDef: Trạng thái HAL_OK nếu thành công.
  */
static HAL_StatusTypeDef PAJ7620_ReadRegisters(uint8_t reg_addr, uint8_t *pData, uint16_t Size) {
    return HAL_I2C_Mem_Read(&hi2c3, PAJ7620_I2C_BUS_ADDR, reg_addr, I2C_MEMADD_SIZE_8BIT, pData, Size, 100);
}


/**
  * @brief Chọn bank thanh ghi (Bank 0 hoặc Bank 1) của PAJ7620U2.
  * @param bank: 0 cho Bank 0, 1 cho Bank 1.
  * @retval HAL_StatusTypeDef: Trạng thái HAL_OK nếu thành công.
  */
static HAL_StatusTypeDef PAJ7620_SelectRegisterBank(uint8_t bank) {
    return PAJ7620_WriteRegister(PAJ7620_REGISTER_BANK_SEL, bank);
}

/**
  * @brief Kiểm tra ID của thiết bị PAJ7620U2.
  * @param None
  * @retval bool: true nếu ID khớp, false nếu không.
  */
static bool PAJ7620_IsDeviceValid(void) {
    uint8_t data0 = 0, data1 = 0;

    PAJ7620_SelectRegisterBank(0); // ID ở Bank 0
    PAJ7620_ReadRegister(PAJ7620_ADDR_PART_ID_0, &data0);
    PAJ7620_ReadRegister(PAJ7620_ADDR_PART_ID_1, &data1);

    if ((data0 != PAJ7620_PART_ID_LSB) || (data1 != PAJ7620_PART_ID_MSB)) {
        return false;
    }
    return true;
}

/**
  * @brief Ghi một mảng các cặp địa chỉ/giá trị vào các thanh ghi.
  * @param settings: Mảng các cấu hình thanh ghi.
  * @param size: Kích thước của mảng.
  * @retval HAL_StatusTypeDef: Trạng thái HAL_OK nếu thành công.
  */
static HAL_StatusTypeDef PAJ7620_WriteRegisterArray(const PAJ7620_RegSetting_t settings[], uint16_t size) {
    HAL_StatusTypeDef status;
    for (uint16_t i = 0; i < size; i++) {
        status = PAJ7620_WriteRegister(settings[i].address, settings[i].value);
        if (status != HAL_OK) {
            return status;
        }
    }
    PAJ7620_SelectRegisterBank(0); // Luôn về Bank 0 sau khi cấu hình
    return HAL_OK;
}

/**
  * @brief Kiểm tra các cử chỉ Tiến/Lùi.
  * @param initialGesture: Cử chỉ ban đầu được phát hiện.
  * @retval PAJ7620_Gesture_t: Cử chỉ cuối cùng (có thể là Tiến/Lùi).
  */
static PAJ7620_Gesture_t PAJ7620_ForwardBackwardGestureCheck(PAJ7620_Gesture_t initialGesture) {
    uint8_t data = 0;
    // Delay ngắn để cho cử chỉ Z-axis (tiến/lùi) có thời gian hoàn thành
    HAL_Delay(10); // Có thể điều chỉnh delay này nếu cần

    PAJ7620_ReadRegister(PAJ7620_ADDR_GES_PS_DET_FLAG_0, &data); // Đọc lại thanh ghi cử chỉ

    if (data == GES_FORWARD_FLAG) {
        HAL_Delay(200); // Delay sau khi phát hiện để người dùng rút tay ra
        return GES_FORWARD;
    } else if (data == GES_BACKWARD_FLAG) {
        HAL_Delay(200); // Delay sau khi phát hiện để người dùng rút tay ra
        return GES_BACKWARD;
    }
    return initialGesture;
}


// --- Public Functions ---

/**
  * @brief Khởi tạo module PAJ7620U2.
  * @param None
  * @retval HAL_StatusTypeDef: HAL_OK nếu thành công, lỗi nếu không.
  */
HAL_StatusTypeDef PAJ7620_Init(void) {
    HAL_StatusTypeDef status;

    HAL_Delay(1); // Chờ 700us trong thư viện gốc, HAL_Delay(1) là đủ cho 1ms. Cảm biến cần thời gian ổn định.

    // Giao tiếp I2C lần đầu để đánh thức cảm biến (làm 2 lần như thư viện gốc)
    PAJ7620_SelectRegisterBank(0);
    PAJ7620_SelectRegisterBank(0);

    if (!PAJ7620_IsDeviceValid()) {
        return HAL_ERROR; // Thiết bị không khớp
    }

    // Ghi mảng khởi tạo thiết bị
    status = PAJ7620_WriteRegisterArray(initRegisterArray, INIT_REG_ARRAY_SIZE);
    if (status != HAL_OK) return status;

    // Đặt cảm biến vào chế độ cử chỉ
    PAJ7620_SetGestureMode();

    return HAL_OK;
}

/**
  * @brief Đặt cảm biến vào chế độ cử chỉ.
  * @param None
  * @retval None
  */
void PAJ7620_SetGestureMode(void) {
    PAJ7620_WriteRegisterArray(setGestureModeRegisterArray, SET_GES_MODE_REG_ARRAY_SIZE);
}

/**
  * @brief Đọc cử chỉ hiện tại từ cảm biến.
  * @param None
  * @retval PAJ7620_Gesture_t: Cử chỉ được phát hiện (ví dụ: GES_UP), hoặc GES_NONE nếu không có.
  */
PAJ7620_Gesture_t PAJ7620_ReadGesture(void) {
    uint8_t data0 = 0, data1 = 0;
    PAJ7620_Gesture_t result = GES_NONE;

    PAJ7620_ReadRegister(PAJ7620_ADDR_GES_PS_DET_FLAG_0, &data0); // Đọc thanh ghi cử chỉ chính

    switch (data0) {
        case GES_RIGHT_FLAG:
            result = PAJ7620_ForwardBackwardGestureCheck(GES_RIGHT);
            break;
        case GES_LEFT_FLAG:
            result = PAJ7620_ForwardBackwardGestureCheck(GES_LEFT);
            break;
        case GES_UP_FLAG:
            result = PAJ7620_ForwardBackwardGestureCheck(GES_UP);
            break;
        case GES_DOWN_FLAG:
            result = PAJ7620_ForwardBackwardGestureCheck(GES_DOWN);
            break;
        case GES_FORWARD_FLAG:
            HAL_Delay(200); // Delay sau khi phát hiện
            result = GES_FORWARD;
            break;
        case GES_BACKWARD_FLAG:
            HAL_Delay(200); // Delay sau khi phát hiện
            result = GES_BACKWARD;
            break;
        case GES_CLOCKWISE_FLAG:
            result = GES_CLOCKWISE;
            break;
        case GES_ANTI_CLOCKWISE_FLAG:
            result = GES_ANTICLOCKWISE;
            break;
        default:
            // Kiểm tra cử chỉ WAVE (nằm ở thanh ghi khác)
            PAJ7620_ReadRegister(PAJ7620_ADDR_GES_PS_DET_FLAG_1, &data1);
            if (data1 == GES_WAVE_FLAG) {
                result = GES_WAVE;
            }
            break;
    }
    return result;
}

/**
  * @brief Đặt tốc độ lấy mẫu cảm biến thành 120fps (chế độ bình thường).
  * @param None
  * @retval None
  */
void PAJ7620_SetNormalSpeed(void) {
    PAJ7620_SelectRegisterBank(1);
    PAJ7620_WriteRegister(PAJ7620_ADDR_R_IDLE_TIME_0, PAJ7620_NORMAL_SPEED);
    PAJ7620_SelectRegisterBank(0);
}

/**
  * @brief Đặt tốc độ lấy mẫu cảm biến thành 240fps (chế độ chơi game).
  * @param None
  * @retval None
  */
void PAJ7620_SetGameSpeed(void) {
    PAJ7620_SelectRegisterBank(1);
    PAJ7620_WriteRegister(PAJ7620_ADDR_R_IDLE_TIME_0, PAJ7620_GAME_SPEED);
    PAJ7620_SelectRegisterBank(0);
}

/**
  * @brief Xóa các cờ ngắt cử chỉ.
  * @param None
  * @retval None
  */
void PAJ7620_ClearGestureInterrupts(void) {
    uint8_t dummy_data;
    // Đọc để xóa cờ ngắt
    PAJ7620_ReadRegister(PAJ7620_ADDR_GES_PS_DET_FLAG_0, &dummy_data);
    PAJ7620_ReadRegister(PAJ7620_ADDR_GES_PS_DET_FLAG_1, &dummy_data);
}

/**
  * @brief Lấy số lần vẫy tay được cảm biến phát hiện.
  * @param None
  * @retval int: Số lần vẫy (0-15).
  */
int PAJ7620_GetWaveCount(void) {
    uint8_t waveCount = 0;
    PAJ7620_ReadRegister(PAJ7620_ADDR_WAVE_COUNT, &waveCount);
    waveCount &= 0x0F; // Count is [3:0] bits - values in 0..15
    return (int)waveCount;
}

/**
  * @brief Kiểm tra xem có đối tượng nào trong tầm nhìn của cảm biến hay không.
  * @param None
  * @retval bool: true nếu có đối tượng, false nếu không.
  */
bool PAJ7620_IsObjectInView(void) {
    uint8_t noObjectCount = 0;
    PAJ7620_ReadRegister(PAJ7620_ADDR_NO_OBJECT_COUNT, &noObjectCount);
    return (noObjectCount == 0);
}

/**
  * @brief Lấy độ sáng của đối tượng được phát hiện.
  * @param None
  * @retval int: Giá trị độ sáng (0-255).
  */
int PAJ7620_GetObjectBrightness(void) {
    uint8_t brightness = 0;
    PAJ7620_ReadRegister(PAJ7620_ADDR_OBJECT_BRIGHTNESS, &brightness);
    return (int)brightness;
}

/**
  * @brief Lấy kích thước của đối tượng được phát hiện.
  * @param None
  * @retval int: Giá trị kích thước (0-900).
  */
int PAJ7620_GetObjectSize(void) {
    uint8_t data[2] = {0,0};
    PAJ7620_ReadRegisters(PAJ7620_ADDR_OBJECT_SIZE_LSB, data, 2);
    return ((data[1] << 8) | data[0]);
}

/**
  * @brief Lấy tọa độ X của tâm đối tượng.
  * @param None
  * @retval int: Tọa độ X.
  */
int PAJ7620_GetObjectCenterX(void) {
    uint8_t data[2] = {0,0};
    PAJ7620_ReadRegisters(PAJ7620_ADDR_OBJECT_CENTER_X_LSB, data, 2);
    // data[1] chứa MSB (4 bit thấp), data[0] chứa LSB (8 bit)
    return (((uint16_t)data[1] & 0x1F) << 8) | data[0];
}

/**
  * @brief Lấy tọa độ Y của tâm đối tượng.
  * @param None
  * @retval int: Tọa độ Y.
  */
int PAJ7620_GetObjectCenterY(void) {
    uint8_t data[2] = {0,0};
    PAJ7620_ReadRegisters(PAJ7620_ADDR_OBJECT_CENTER_Y_LSB, data, 2);
    // data[1] chứa MSB (4 bit thấp), data[0] chứa LSB (8 bit)
    return (((uint16_t)data[1] & 0x1F) << 8) | data[0];
}

/**
  * @brief Lấy vận tốc X của đối tượng.
  * @param None
  * @retval int: Vận tốc X.
  */
int PAJ7620_GetObjectVelocityX(void) {
    if(!PAJ7620_IsObjectInView()) {
        return 0;
    }
    uint8_t data[2] = {0,0};
    PAJ7620_ReadRegisters(PAJ7620_ADDR_OBJECT_VEL_X_LSB, data, 2);
    int result = (int)(data[0] & 0x3F); // LSB
    if(data[1] & 0x01) { // Bit 0 của MSB là bit dấu
        result *= -1;
    }
    return result;
}

/**
  * @brief Lấy vận tốc Y của đối tượng.
  * @param None
  * @retval int: Vận tốc Y.
  */
int PAJ7620_GetObjectVelocityY(void) {
    if(!PAJ7620_IsObjectInView()) {
        return 0;
    }
    uint8_t data[2] = {0,0};
    PAJ7620_ReadRegisters(PAJ7620_ADDR_OBJECT_VEL_Y_LSB, data, 2);
    int result = (int)(data[0] & 0x3F); // LSB
    if(data[1] & 0x01) { // Bit 0 của MSB là bit dấu
        result *= -1;
    }
    return result;
}
