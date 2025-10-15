#ifndef PIN_H
#define PIN_H

// RC522 pins (VSPI)
#define RST_PIN     15    
#define SS_PIN      21    
#define MISO_PIN    22    
#define MOSI_PIN    23    
#define SCK_PIN     18    

// SD Card pins (HSPI)
#define SD_MISO_PIN    26     // 改用 GPIO12（HSPI 預設 MISO）
#define SD_MOSI_PIN    27     // GPIO13（HSPI 預設 MOSI）
#define SD_SCK_PIN     25     // GPIO14（HSPI 預設 SCK）
#define SD_CS_PIN      32     // GPIO32（CS 可以用任何數位輸出腳位）


// SPI configurations
extern SPIClass spi;
extern MFRC522 rfid;

#endif // PIN_H
