# 🎮 STM32F429I Tetris Game với Điều khiển Cử chỉ

Game Tetris chạy trên STM32F429I Discovery với điều khiển cử chỉ bằng cảm biến PAJ7620 và nút bấm vật lý.

## ✨ Tính năng

- 🎯 **2 chế độ chơi:** Normal, Challenge
- 👋 **Điều khiển cử chỉ:** PAJ7620 sensor
- 🕹️ **Điều khiển nút bấm:** 4 nút vật lý
- 🎨 **Giao diện:** TouchGFX UI 
- ⚡ **Real-time:** FreeRTOS multi-task

## 🔧 Yêu cầu

### Hardware
- **STM32F429I Discovery Board** (STM32F429ZIT6)
- **PAJ7620 Gesture Sensor**
- **4 nút bấm** (PG2, PG3, PB12, PB13)
- **4 Điện trở 1kΩ**
- **Dây nối**

### Software
- **STM32CubeIDE** >= 1.17.0
- **TouchGFX** 4.25.0

## 🚀 Cài đặt

```bash
git clone https://github.com/yourusername/STM32F429I-Tetris.git
```

1. Mở project trong STM32CubeIDE: `STM32CubeIDE/`
2. Build project: `Ctrl+B`
3. Kết nối hardware:
   - PAJ7620: SDA→PC9, SCL→PA8, VCC→3.3V, GND→GND
   - Buttons: LEFT→PG2, RIGHT→PG3, DOWN→PB12, ROTATE→PB13
4. Flash và chạy: `Run → STM32 Application`

## 🎮 Điều khiển

### Cử chỉ (PAJ7620)
- **LEFT/RIGHT:** Di chuyển ngang
- **DOWN:** Thả nhanh
- **CLOCKWISE:** Xoay khối

### Nút bấm
- **PG2/PG3:** Trái/Phải
- **PB12:** Thả nhanh  
- **PB13:** Xoay khối

## 📁 Cấu trúc

```
├── Core/           # STM32 HAL & main code
├── TouchGFX/       # UI framework
├── STM32CubeIDE/   # Project files
└── README.md
```

## 🐛 Xử lý sự cố

- **Lỗi build:** Tạo lại mã TouchGFX
- **Lỗi flash:** Kiểm tra kết nối ST-Link
- **Lỗi cảm biến:** Kiểm tra kết nối I2C
