#include <reg51.h>

// ---------- PIN DEFINITIONS ----------
sbit SDA  = P2^0;   // I2C data
sbit SCL  = P2^1;   // I2C clock

sbit DT   = P3^2;   // HX711 data
sbit SCK  = P3^3;   // HX711 clock

sbit TARE = P1^1;   // Tare / calibration button

sbit B0   = P1^3;   // UART button -> 'A'
sbit B1   = P1^4;   // UART button -> 'B'
sbit B2   = P1^5;   // UART button -> 'C'
sbit B3   = P1^6;   // UART button -> 'D'
sbit B4   = P1^0;   // UART button -> 'E'

// ---------- LCD CONFIG ----------
#define LCD_ADDR  0x4E
#define EN        0x04
#define RS        0x01
#define BL        0x08

// ---------- GLOBALS ----------
long x0 = 0;            // HX711 baseline (empty pan)
long x1 = 0;            // HX711 reading with known weight
long span = 1;          // x1 - x0
long reading, weight;

// CAL_WEIGHT in TENTHS of a gram. (193.5g = 1935)
long CAL_WEIGHT = 1935;  

// =============================================================
// DELAYS
// =============================================================

void delay_long() {
    unsigned int i, j;
    for (i = 0; i < 200; i++)
        for (j = 0; j < 1275; j++);
}

void delay_short() {
    unsigned int i, j;
    for (i = 0; i < 5; i++)
        for (j = 0; j < 1275; j++);
}

// =============================================================
// UART
// =============================================================

void UART_Init(void) {
    TMOD = 0x20;    // Timer 1, mode 2
    TH1  = 0xFD;    // 9600 baud @ 11.0592 MHz
    SCON = 0x50;    // UART mode 1
    TR1  = 1;       // Start Timer 1
}

void UART_Send(char ch) {
    SBUF = ch;
    while (TI == 0);
    TI = 0;
}

// =============================================================
// I2C (bit-bang)
// =============================================================

void I2C_Start(void) { SDA = 1; SCL = 1; SDA = 0; SCL = 0; }
void I2C_Stop(void)  { SDA = 0; SCL = 1; SDA = 1; }

void I2C_Write(unsigned char dat) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        SDA = (dat & 0x80) ? 1 : 0;
        SCL = 1; SCL = 0;
        dat <<= 1;
    }
    SDA = 1; SCL = 1; SCL = 0;
}

// =============================================================
// LCD
// =============================================================

void LCD_Send(unsigned char dat, unsigned char mode) {
    unsigned char high = dat & 0xF0;
    unsigned char low  = (dat << 4) & 0xF0;
    I2C_Start(); I2C_Write(LCD_ADDR);
    I2C_Write(high | mode | BL | EN); I2C_Write(high | mode | BL); I2C_Stop();
    I2C_Start(); I2C_Write(LCD_ADDR);
    I2C_Write(low | mode | BL | EN); I2C_Write(low | mode | BL); I2C_Stop();
    delay_short();
}

void LCD_Command(unsigned char cmd) { LCD_Send(cmd, 0); }
void LCD_Char(unsigned char dat)    { LCD_Send(dat, RS); }

void LCD_String(char *str) {
    while (*str) LCD_Char(*str++);
}

void LCD_Number(long num) {
    char buf[10];
    unsigned char i = 0;
    if (num < 0) { LCD_Char('-'); num = -num; }
    if (num == 0) { LCD_Char('0'); return; }
    while (num > 0) { buf[i++] = (num % 10) + '0'; num /= 10; }
    while (i > 0) { LCD_Char(buf[--i]); }
}

void LCD_Init() {
    delay_long();
    LCD_Command(0x02); LCD_Command(0x28); LCD_Command(0x0C);
    LCD_Command(0x06); LCD_Command(0x01);
    delay_long();
}

// =============================================================
// HX711
// =============================================================

long HX711_Read() {
    long count = 0;
    unsigned char i;
    unsigned int timeout = 0;
    while (DT && timeout < 50000) timeout++;
    for (i = 0; i < 24; i++) {
        SCK = 1; count <<= 1; SCK = 0;
        if (DT) count++;
    }
    SCK = 1; SCK = 0;
    if (count & 0x800000) count |= 0xFF000000;
    return count;
}

long HX711_Avg_Slow() {
    unsigned char i; long sum = 0;
    for (i = 0; i < 10; i++) sum += HX711_Read();
    return sum / 10;
}

long HX711_Avg_Fast() {
    unsigned char i; long sum = 0;
    for (i = 0; i < 3; i++) sum += HX711_Read();
    return sum / 3;
}

void LCD_DisplayWeight(long weight_tenths) {
    long whole, decimal;
    if (weight_tenths < 0) weight_tenths = 0;
    whole   = weight_tenths / 10;
    decimal = weight_tenths % 10;
    LCD_Number(whole);
    LCD_Char('.');
    LCD_Char(decimal + '0');
    LCD_String(" g");
}

// =============================================================
// MAIN
// =============================================================

void main() {
    UART_Init();
    LCD_Init();

    // PHASE 1: CALIBRATION
    LCD_Command(0x01); LCD_String("Empty pan");
    LCD_Command(0xC0); LCD_String("Press TARE");
    while (TARE == 1); delay_long(); while (TARE == 0);
    x0 = HX711_Avg_Slow();

    LCD_Command(0x01); LCD_String("Add 193.5g");
    LCD_Command(0xC0); LCD_String("Press TARE");
    while (TARE == 1); delay_long(); while (TARE == 0);
    x1 = HX711_Avg_Slow();
    span = x1 - x0;
    if (span == 0) span = 1;

    LCD_Command(0x01); LCD_String("Cal done!");
    delay_long();

    // PHASE 2 & 3: MAIN LOOP
    while (1) {
        // --- LIVE TARE FEATURE ---
        if (TARE == 0) {
            delay_short();
            if (TARE == 0) {
                x0 = HX711_Avg_Slow(); // Reset zero baseline
                while (TARE == 0);     // Wait for release
            }
        }

        // --- UART BUTTONS ---
        if (B0 == 0) { delay_short(); if (B0 == 0) { UART_Send('A'); while (B0 == 0); } }
        if (B1 == 0) { delay_short(); if (B1 == 0) { UART_Send('B'); while (B1 == 0); } }
        if (B2 == 0) { delay_short(); if (B2 == 0) { UART_Send('C'); while (B2 == 0); } }
        if (B3 == 0) { delay_short(); if (B3 == 0) { UART_Send('D'); while (B3 == 0); } }
        if (B4 == 0) { delay_short(); if (B4 == 0) { UART_Send('E'); while (B4 == 0); } }

        // --- WEIGHT CALCULATION ---
        reading = HX711_Avg_Fast();
        {
            long delta = (reading - x0) / 100;
            long s = span / 100;
            if (s == 0) s = 1;
            weight = (delta * CAL_WEIGHT) / s;
        }

        LCD_Command(0x01); LCD_String("Weight:");
        LCD_Command(0xC0); LCD_DisplayWeight(weight);
        delay_short();
    }
}
