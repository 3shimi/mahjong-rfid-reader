# 麻將讀牌器 - Claude 開發文檔

## 項目概述
這是一個基於 Arduino ESP32 的智能麻將讀牌器系統，能夠識別麻將牌並透過藍牙播放對應的語音。

## 技術規格
- **微控制器**: ESP32
- **RFID 讀取器**: MFRC522  
- **音訊輸出**: 藍牙 A2DP
- **儲存**: SD 卡
- **程式語言**: C++ (Arduino框架)

## 核心功能
- RFID 麻將牌識別
- 音檔播放 (WAV格式)
- 藍牙音訊傳輸
- SD卡檔案管理

## 開發環境設置

### 開發平台
- **IDE**: PlatformIO (已從 Arduino IDE 遷移)
- **框架**: Arduino Framework
- **平台**: espressif32 @ 6.4.0

### 必要庫依賴
```
- Arduino.h
- SPI.h
- MFRC522.h @ ^1.4.11
- SD.h
- BluetoothA2DPSource.h (ESP32-A2DP 庫)
- AudioTools @ v0.9.8
```

### PlatformIO 設定
詳見 `platformio.ini` 配置檔案，包含:
- A2DP 相關編譯標誌
- PSRAM 支援
- 記憶體分區 (huge_app.csv)
- 除錯和監控設定

### 硬體連接
**MFRC522 (VSPI)**
- RST_PIN: 15
- SS_PIN: 21  
- MISO_PIN: 22
- MOSI_PIN: 23
- SCK_PIN: 18

**SD 卡模組 (HSPI)**
- MISO_PIN: 26
- MOSI_PIN: 27
- SCK_PIN: 25
- CS_PIN: 32

## 麻將牌編碼系統

### 筒子 (TON): 0x01-0x09
- 一筒: 0x01, 二筒: 0x02, ..., 九筒: 0x09

### 索子 (SUO): 0x11-0x19  
- 一索: 0x11, 二索: 0x12, ..., 九索: 0x19

### 萬子 (WAN): 0x21-0x29
- 一萬: 0x21, 二萬: 0x22, ..., 九萬: 0x29

### 字牌 (ZI): 0x31-0x37
- 東: 0x31, 南: 0x32, 西: 0x33, 北: 0x34
- 白: 0x35, 發: 0x36, 中: 0x37

## 音檔格式要求
- 格式: WAV
- 採樣率: 22050 Hz (會自動重採樣至 44100 Hz)
- 聲道: 單聲道 (會自動轉換為立體聲)
- 位元深度: 16-bit PCM
- 檔案命名: `/audio/MAHJONG_{牌型}_{數字}.wav`

## 常用指令

### 編譯和上傳 (PlatformIO)
```bash
# 編譯專案
pio run

# 編譯並上傳至開發板
pio run --target upload

# 編譯、上傳並開啟序列監控
pio run --target upload && pio device monitor

# 清除編譯快取
pio run --target clean
```

### 序列監控
```bash
# 開啟序列監控 (波特率: 115200)
pio device monitor

# 特性: 自動包含 ESP32 例外解碼器和時間戳記
```

## 故障排解

### SD 卡問題
- 檢查 SD 卡格式 (建議 FAT32)
- 確認音檔路徑正確
- 檢查 SD 卡腳位連接

### 藍牙連接問題  
- 確認設備名稱 "i21"
- 檢查 A2DP 相容性
- 重新啟動藍牙連接

### RFID 讀取問題
- 檢查 MFRC522 接線
- 確認麻將牌內嵌 RFID 標籤
- 調整讀取距離

## 程式碼結構

### 目錄架構
```
Mahjong_Reader_Stream_SD_CARD/
├── platformio.ini          # PlatformIO 配置檔
├── src/
│   └── main.cpp           # 主程式 (原 .ino)
├── include/
│   ├── pin.h              # 腳位定義
│   └── mahjong_define.h   # 麻將牌編碼定義
├── data/                  # SD 卡資料 (音檔)
├── lib/                   # 自定義庫
└── test/                  # 測試程式
```

### 核心函數 (main.cpp)
- `getMahjongType()`: 判斷牌型 (筒/索/萬/字)
- `getMahjongNumber()`: 取得數字 (1-9)
- `playAudio()`: 播放音檔主函數
- `get_sound_data()`: A2DP 音訊資料回調函數
- `convertMonoToStereo()`: 音訊格式轉換 (單→立體聲)
- `resample22to44()`: 重採樣 (22050→44100 Hz)
- `print_wav_header()`: 顯示 WAV 檔案資訊 (除錯用)

## 開發注意事項
- **記憶體管理**: ESP32 記憶體有限，注意緩衝區大小和 PSRAM 使用
- **SPI 衝突**: RFID 和 SD 卡共用 SPI，需正確初始化腳位
- **音檔處理**:
  - 音檔大小影響載入和播放時間
  - 22kHz 單聲道會即時轉換為 44kHz 立體聲
  - 使用 `get_sound_data()` 回調進行串流播放
- **RFID 讀取**:
  - 有效讀取距離約 2-3 公分
  - 需要 100ms 延遲避免重複讀取
- **藍牙 A2DP**:
  - 使用 ESP32-A2DP 庫而非舊版 AudioTools A2DP
  - 需要 `huge_app.csv` 分區以容納完整程式
- **除錯層級**: CORE_DEBUG_LEVEL=3，生產環境建議降至 2

## 測試建議
1. **SD 卡測試**: 檢查格式、檔案系統和音檔路徑
2. **藍牙連接測試**: 確認 "i21" 設備可見並可配對
3. **RFID 功能測試**: 驗證卡片識別和數據讀取
4. **音訊播放測試**: 完整流程從識別到播放
5. **序列監控**: 觀察 WAV header 資訊和系統狀態

## 相關文檔
- `README.md`: 使用者說明和硬體連接
- `PLATFORMIO_SETUP.md`: PlatformIO 環境設置指南
- `MIGRATION_SUMMARY.md`: 從 Arduino IDE 遷移記錄