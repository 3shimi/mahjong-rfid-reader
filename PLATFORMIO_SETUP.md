# PlatformIO 安裝和使用指南

## macOS 安裝 PlatformIO

### 方法 1: 使用 Homebrew (推薦)
```bash
# 安裝 PlatformIO CLI
brew install platformio

# 驗證安裝
pio --version
```

### 方法 2: 使用 pip
```bash
# 安裝 Python 包管理器 pip (如果尚未安裝)
python3 -m ensurepip --upgrade

# 安裝 PlatformIO
pip3 install --upgrade platformio

# 將 PlatformIO 添加到 PATH
echo 'export PATH="$PATH:~/.local/bin"' >> ~/.zshrc
source ~/.zshrc

# 驗證安裝
pio --version
```

### 方法 3: 使用 VS Code 擴展
1. 安裝 VS Code
2. 安裝 "PlatformIO IDE" 擴展
3. 重啟 VS Code
4. 點擊左側 PlatformIO 圖標

## 專案編譯和上傳

### 命令行操作
```bash
# 進入專案目錄
cd /Users/3shimi/Projects/Mahjong_Reader_Stream_SD_CARD

# 編譯專案
pio run

# 上傳到設備 (確保 ESP32 已連接)
pio run --target upload

# 開啟串列監控器
pio device monitor

# 清理編譯快取
pio run --target clean
```

### VS Code 操作
1. 開啟 VS Code
2. 選擇 File > Open Folder
3. 選擇專案資料夾
4. 點擊底部狀態欄的編譯/上傳按鈕

## 常見問題排解

### 1. 庫下載失敗
```bash
# 清理庫快取並重新下載
pio lib uninstall
pio run
```

### 2. ESP32 板子未識別
```bash
# 列出所有連接的設備
pio device list

# 在 platformio.ini 中指定正確的連接埠
upload_port = /dev/cu.usbserial-*
monitor_port = /dev/cu.usbserial-*
```

### 3. 編譯記憶體不足錯誤
- 關閉其他應用程式
- 或在 platformio.ini 中調整編譯選項：
```ini
build_flags = 
    -DCORE_DEBUG_LEVEL=1  ; 降低除錯層級
```

### 4. AudioTools 庫版本衝突
如果遇到 AudioTools 庫編譯錯誤：
```bash
# 手動安裝特定版本
pio lib install "https://github.com/pschatzmann/arduino-audio-tools.git#v0.9.8"
```

## 遷移完成檢查清單

- [x] 創建 platformio.ini 配置文件
- [x] 重新組織專案結構 (src/, include/, data/)
- [x] 修正程式碼相容性問題
- [x] 解決 AudioTools 庫依賴問題
- [x] 修正 A2DP API 使用方式
- [x] 測試編譯成功
- [ ] 安裝 PlatformIO (使用者操作)
- [ ] 上傳到 ESP32 (使用者操作)
- [ ] 驗證所有功能正常運作 (使用者測試)

## 專案結構說明

```
Mahjong_Reader_Stream_SD_CARD/
├── platformio.ini          # PlatformIO 配置檔
├── src/
│   └── main.cpp            # 主程式 (原 .ino 檔案)
├── include/
│   ├── pin.h               # 腳位定義
│   └── mahjong_define.h    # 麻將牌編碼
├── data/
│   └── audio/              # 音檔資料 (上傳到 ESP32 檔案系統)
├── lib/                    # 本地庫 (如有)
├── test/                   # 測試檔案 (如有)
├── CLAUDE.md               # 開發者文檔
├── README.md               # 使用者說明
└── PLATFORMIO_SETUP.md     # 本安裝指南
```

## 優勢說明

### 相較於 Arduino IDE 的優勢：
1. **精確的庫版本管理**：避免庫更新導致的相容性問題
2. **更好的專案組織**：標準化的目錄結構
3. **強大的編譯系統**：支援多環境配置
4. **豐富的除錯工具**：內建串列監控和除錯功能
5. **跨平台支援**：在 Windows、macOS、Linux 上一致的體驗

### 解決原有問題：
- **AudioTools 版本鎖定**：避免 API 變更導致的編譯失敗
- **ESP32 核心穩定性**：使用測試過的穩定版本
- **記憶體分區優化**：為 A2DP 功能提供足夠記憶體
- **編譯標誌管理**：統一管理所有編譯選項