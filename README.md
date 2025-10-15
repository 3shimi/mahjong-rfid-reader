# 🀄 智能麻將讀牌器

一個基於 ESP32 的智能麻將讀牌器，能夠自動識別麻將牌並透過藍牙播放對應的語音提示。

## ✨ 特色功能

- 🔍 **自動識別**: 使用 RFID 技術精確識別麻將牌型
- 🎵 **語音播報**: 透過藍牙 A2DP 播放清晰的語音提示
- 💾 **離線運作**: 所有音檔儲存在 SD 卡中，無需網路連線
- 🎯 **快速反應**: 毫秒級識別速度，即放即播
- 📱 **無線傳輸**: 支援任何藍牙音響或耳機

## 🛠 硬體需求

### 主要元件
- ESP32 開發板 × 1
- MFRC522 RFID 讀取模組 × 1  
- Micro SD 卡模組 × 1
- SD 卡 (建議 8GB 以上) × 1
- 內嵌 RFID 標籤的麻將牌一套

### 連接線材
- 杜邦線若干
- 麵包板或 PCB 板

## 🔌 硬體連接

### MFRC522 RFID 模組
```
ESP32    →  MFRC522
GPIO21   →  SDA/SS
GPIO18   →  SCK
GPIO23   →  MOSI
GPIO22   →  MISO
GPIO15   →  RST
3.3V     →  VCC
GND      →  GND
```

### SD 卡模組  
```
ESP32    →  SD 卡模組
GPIO32   →  CS
GPIO25   →  SCK
GPIO27   →  MOSI
GPIO26   →  MISO
5V       →  VCC
GND      →  GND
```

## 📦 安裝步驟

### 1. 環境準備

#### 方法 A: 使用 PlatformIO（推薦）
1. 安裝 [Visual Studio Code](https://code.visualstudio.com/)
2. 安裝 [PlatformIO IDE 擴充套件](https://platformio.org/install/ide?install=vscode)
3. 下載本專案
4. 用 VS Code 開啟專案資料夾
5. PlatformIO 會自動下載所有依賴庫

#### 方法 B: 使用 PlatformIO CLI
```bash
# 安裝 PlatformIO Core
pip install platformio

# 下載專案
git clone https://github.com/3shimi/mahjong-rfid-reader.git
cd mahjong-rfid-reader

# 編譯專案
pio run
```

### 2. 程式上傳
```bash
# PlatformIO 上傳
pio run --target upload

# 或使用 VS Code 的 PlatformIO 工具列按鈕上傳
```

所有依賴庫會自動安裝：
- MFRC522 @ 1.4.11
- AudioTools @ 0.9.8
- ESP32-A2DP

### 3. 音檔準備
在 SD 卡根目錄建立 `audio` 資料夾，並放入對應的音檔：

```
/audio/
├── MAHJONG_TON_1.wav    (一筒)
├── MAHJONG_TON_2.wav    (二筒)
├── ...
├── MAHJONG_SUO_1.wav    (一索)
├── MAHJONG_SUO_2.wav    (二索)  
├── ...
├── MAHJONG_WAN_1.wav    (一萬)
├── MAHJONG_WAN_2.wav    (二萬)
├── ...
└── MAHJONG_ZI_EAST.wav  (東風)
```

#### 音檔規格
- **格式**: WAV
- **採樣率**: 22050 Hz
- **聲道**: 單聲道
- **位元深度**: 16-bit PCM

## 🎮 使用方法

### 第一次使用
1. 插入準備好的 SD 卡
2. 給 ESP32 供電
3. 開啟序列監視器 (波特率 115200)
4. 確認系統初始化成功

### 配對藍牙設備
1. 在手機或音響上搜尋藍牙設備
2. 連接名為 "i21" 的設備
3. 連接成功後即可開始使用

### 讀取麻將牌
1. 將含有 RFID 標籤的麻將牌靠近讀取器
2. 系統會自動識別並播放對應語音
3. 每次識別間隔約 0.1 秒

## 📊 支援的麻將牌

### 數字牌
- **筒子**: 一筒 ~ 九筒 (9 張)
- **索子**: 一索 ~ 九索 (9 張)  
- **萬子**: 一萬 ~ 九萬 (9 張)

### 字牌  
- **風牌**: 東、南、西、北 (4 張)
- **三元牌**: 白、發、中 (3 張)

*總計 34 種不同的麻將牌型*

## 🔧 故障排解

### SD 卡無法讀取
- ✅ 確認 SD 卡格式為 FAT32
- ✅ 檢查 SD 卡接線是否正確
- ✅ 嘗試更換 SD 卡

### 藍牙無法連接
- ✅ 確認設備名稱為 "i21"  
- ✅ 清除手機藍牙快取
- ✅ 重新啟動 ESP32

### 無法識別麻將牌
- ✅ 確認麻將牌內有 RFID 標籤
- ✅ 檢查 MFRC522 接線
- ✅ 調整麻將牌與讀取器的距離 (建議 2-3 公分)

### 沒有聲音輸出
- ✅ 確認音檔格式正確
- ✅ 檢查音檔命名是否符合規範
- ✅ 確認藍牙設備已正確連接

## 📈 進階功能

### 自訂音檔
您可以錄製自己的語音或使用不同語言的音檔：
1. 確保音檔格式符合規範
2. 按照命名規則重新命名
3. 覆蓋 SD 卡中的原有音檔

### 序列監視器輸出
透過序列監視器可以查看：
- 系統初始化狀態  
- RFID 讀取資訊
- 音檔播放詳細資訊
- 錯誤訊息和除錯資訊

## 🎯 技術原理

本系統採用 RFID 技術識別麻將牌，每張麻將牌內嵌獨特的 RFID 標籤。當麻將牌靠近讀取器時，系統會：

1. 讀取 RFID 標籤的唯一 ID
2. 根據預設的編碼表解析牌型  
3. 從 SD 卡載入對應的音檔
4. 進行音訊格式轉換 (單聲道→立體聲，22kHz→44kHz)
5. 透過藍牙 A2DP 傳輸至音響設備

## 🤝 貢獻

歡迎提交 Issue 和 Pull Request 來改善這個項目！

## 📄 授權

### 軟體程式碼
本專案的程式碼（`src/`, `include/`, `platformio.ini`）採用 **MIT License + Commons Clause** 授權。

**您可以**：
- ✅ 個人使用、學習、研究
- ✅ 教育機構用於教學
- ✅ 非營利組織使用
- ✅ 修改和分發程式碼

**您不可以**（除非獲得商業授權）：
- ❌ 將本軟體作為商品銷售
- ❌ 提供基於本軟體的付費服務
- ❌ 整合到商業產品中銷售

詳見 [LICENSE](LICENSE) 檔案。

### 文檔與音檔
文檔（README 等）和音檔採用 **CC BY-NC-SA 4.0** 授權。
詳見 [LICENSE-DOCS](LICENSE-DOCS) 檔案。

### 商業授權
如需將本專案用於商業用途，歡迎聯絡洽談商業授權：
- 📧 Email: [請填入您的電子郵件]
- 💬 GitHub Issues: [開啟討論](../../issues)

我們樂意與您討論合作方案！

---

## 🎯 專案資訊

- **原創作者**: [@3shimi](https://github.com/3shimi)
- **開發平台**: PlatformIO + ESP32
- **硬體**: ESP32 + MFRC522 RFID + SD Card + Bluetooth A2DP

---

*讓科技為傳統麻將帶來新的體驗！* 🀄