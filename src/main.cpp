#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SD.h>
#include "BluetoothA2DPSource.h"
#include "pin.h"
#include "mahjong_define.h"

// 全域物件
MFRC522 rfid(SS_PIN, RST_PIN);
BluetoothA2DPSource a2dp_source;
File audioFile;

// WAV 檔案播放器 (備用，目前未使用)
// AudioWAVServer wav;  

// 用於轉換音頻格式
void convertMonoToStereo(const uint8_t* mono_data, uint8_t* stereo_data, size_t mono_size) {
    // 假設輸入是 16-bit PCM
    int16_t* mono = (int16_t*)mono_data;
    int16_t* stereo = (int16_t*)stereo_data;
    
    for(size_t i = 0; i < mono_size/2; i++) {
        stereo[i*2] = mono[i];
        stereo[i*2 + 1] = mono[i];
    }
}

// 簡單的重採樣函數 (22050 -> 44100)
void resample22to44(const uint8_t* input, uint8_t* output, size_t input_size) {
    int16_t* in = (int16_t*)input;
    int16_t* out = (int16_t*)output;
    
    for(size_t i = 0; i < input_size/2; i++) {
        out[i*2] = in[i];
        out[i*2 + 1] = in[i];
    }
}

// 麻將牌型判斷
String getMahjongType(byte value) {
    if (value >= 0x21 && value <= 0x29) return "WAN";
    if (value >= 0x11 && value <= 0x19) return "SUO";
    if (value >= 0x01 && value <= 0x09) return "TON";
    return "UNKNOWN";
}

int getMahjongNumber(byte value) {
    if (value >= 0x21 && value <= 0x29) return value - 0x20;
    if (value >= 0x11 && value <= 0x19) return value - 0x10;
    if (value >= 0x01 && value <= 0x09) return value;
    return 0;
}

void print_wav_header(File file) {
    byte header[44];
    file.read(header, 44);
    file.seek(0);  
    
    Serial.println("\n=== WAV 檔案詳細資訊 ===");
    Serial.print("檔案標識: ");
    for(int i=0; i<4; i++) {
        Serial.print((char)header[i]);
    }
    Serial.println();
    
    uint32_t file_size = header[4] | (header[5] << 8) | 
                        (header[6] << 16) | (header[7] << 24);
    Serial.printf("檔案大小: %d bytes\n", file_size + 8);
    
    Serial.print("WAVE 標記: ");
    for(int i=8; i<12; i++) {
        Serial.print((char)header[i]);
    }
    Serial.println();
    
    uint16_t audio_format = header[20] | (header[21] << 8);
    Serial.printf("音訊格式: %d (1 表示 PCM)\n", audio_format);
    
    uint16_t channels = header[22] | (header[23] << 8);
    Serial.printf("聲道數: %d\n", channels);
    
    uint32_t sample_rate = header[24] | (header[25] << 8) | 
                          (header[26] << 16) | (header[27] << 24);
    Serial.printf("採樣率: %d Hz\n", sample_rate);
    
    uint32_t byte_rate = header[28] | (header[29] << 8) | 
                        (header[30] << 16) | (header[31] << 24);
    Serial.printf("位元率: %d bytes/sec\n", byte_rate);
    
    uint16_t block_align = header[32] | (header[33] << 8);
    Serial.printf("區塊對齊: %d bytes\n", block_align);
    
    uint16_t bits_per_sample = header[34] | (header[35] << 8);
    Serial.printf("位元深度: %d bits\n", bits_per_sample);

    uint32_t data_size = header[40] | (header[41] << 8) | 
                        (header[42] << 16) | (header[43] << 24);
    Serial.printf("資料大小: %d bytes\n", data_size);
    Serial.println("=======================\n");
}

void setup() {
    Serial.begin(115200);
      
    // 初始化 SPI - 使用自訂腳位
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
    
    // 初始化 SD 卡 (使用 HSPI)
    if(!SD.begin(SD_CS_PIN)) {
        Serial.println("SD 卡初始化失敗");
        Serial.println("請檢查 SD 卡連接和格式 (建議使用 FAT32)");
        return;
    }
    Serial.println("SD 卡初始化成功");
    
    rfid.PCD_Init();
    
    // 設定和初始化 A2DP 藍牙音訊
    Serial.println("正在初始化 A2DP 藍牙音訊...");
    a2dp_source.start("i21");  // 設備名稱
    Serial.println("A2DP 藍牙音訊初始化成功");
    
    Serial.println("系統就緒，等待讀取麻將牌...");
}

// 全域變數用於音檔播放
File currentAudioFile;
size_t audioFileSize = 0;
size_t audioPosition = 0;

// A2DP 資料回調函數
int32_t get_sound_data(uint8_t* data, int32_t len) {
    if (!currentAudioFile || audioPosition >= audioFileSize) {
        return 0; // 沒有更多資料
    }
    
    size_t inputSize = len / 4;  // 因為會轉成立體聲，所以輸入是 1/4
    uint8_t inputBuffer[inputSize];
    
    size_t bytesRead = currentAudioFile.read(inputBuffer, inputSize);
    if (bytesRead == 0) {
        return 0;
    }
    
    // 先重採樣然後轉為立體聲
    uint8_t resampleBuffer[bytesRead * 2];
    resample22to44(inputBuffer, resampleBuffer, bytesRead);
    convertMonoToStereo(resampleBuffer, data, bytesRead * 2);
    
    audioPosition += bytesRead;
    
    return bytesRead * 4; // 回傳實際寫入的位元組數
}

void playAudio(const char* filename) {
    // 關閉之前的檔案
    if(currentAudioFile) {
        currentAudioFile.close();
    }
    
    currentAudioFile = SD.open(filename, FILE_READ);
    if(!currentAudioFile) {
        Serial.printf("無法開啟檔案: %s\n", filename);
        return;
    }

    print_wav_header(currentAudioFile);
    currentAudioFile.seek(44);
    
    audioFileSize = currentAudioFile.size() - 44;
    audioPosition = 0;
    
    // 設定資料回調函數
    a2dp_source.set_data_callback(get_sound_data);
    
    Serial.printf("開始播放音檔: %s\n", filename);
}

void loop() {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        delay(50);
        return;
    }
    
    byte readBuffer[18];
    byte size = sizeof(readBuffer);
    MFRC522::StatusCode status = rfid.MIFARE_Read(4, readBuffer, &size);
    
    if (status == MFRC522::STATUS_OK) {
        char filename[64];
        String tileType = getMahjongType(readBuffer[0]);
        int tileNumber = getMahjongNumber(readBuffer[0]);
        
        // 修改檔案路徑，從 SD 卡根目錄讀取
        snprintf(filename, sizeof(filename), "/audio/MAHJONG_%s_%d.wav", 
                tileType.c_str(), tileNumber);
                
        Serial.printf("播放音檔: %s\n", filename);
        playAudio(filename);
    }
    
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    delay(100);
}
