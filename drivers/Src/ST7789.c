#include <ST7789.h>


/*
 *lcd_driver.c
 *
 *  Created on: Jul 29, 2026
 *      Author: Ashwin
 */

void delay_ms(uint32_t ms){
    while (ms--) {
        for (uint32_t i = 0; i < 16000; i++);
    }
}

void delay_us(uint32_t us){
    while (us--) {
        for (uint32_t i = 0; i < 42; i++);
    }
}


void LCD_Select(LCD_Handle_t *pLCD)
{
    GPIO_WriteToOutputPin(pLCD->CSPort, pLCD->CSPin, DISABLE);   // CS low
}

void LCD_Unselect(LCD_Handle_t *pLCD)
{
    GPIO_WriteToOutputPin(pLCD->CSPort, pLCD->CSPin, ENABLE);    // CS high
}

void LCD_Reset(LCD_Handle_t *pLCD)
{
    GPIO_WriteToOutputPin(pLCD->ResetPort, pLCD->ResetPin, DISABLE);
    delay_ms(20);
    GPIO_WriteToOutputPin(pLCD->ResetPort, pLCD->ResetPin, ENABLE);
    delay_ms(120);
}

void LCD_WriteCommand(LCD_Handle_t *pLCD, uint8_t cmd)
{
    GPIO_WriteToOutputPin(pLCD->DCPort, pLCD->DCPin, DISABLE);   // DC=0 for command
    delay_us(1);                     // Setup time
    LCD_Select(pLCD);
    SPI_SendData(pLCD->pSPIHandle->pSPI, &cmd, 1);
    LCD_Unselect(pLCD);
}

void LCD_WriteData(LCD_Handle_t *pLCD, uint8_t data)
{
    GPIO_WriteToOutputPin(pLCD->DCPort, pLCD->DCPin, ENABLE);    // DC=1 for data
    delay_us(1);
    LCD_Select(pLCD);
    SPI_SendData(pLCD->pSPIHandle->pSPI, &data, 1);
    LCD_Unselect(pLCD);
}

void LCD_WriteDataBytes(LCD_Handle_t *pLCD, uint8_t *data, uint16_t len)
{
    GPIO_WriteToOutputPin(pLCD->DCPort, pLCD->DCPin, ENABLE);
    delay_us(1);
    LCD_Select(pLCD);
    SPI_SendData(pLCD->pSPIHandle->pSPI, data, len);
    LCD_Unselect(pLCD);
}



void LCD_SetAddressWindow(LCD_Handle_t *pLCD, uint16_t x0, uint16_t y0,
                           uint16_t x1, uint16_t y1)
{
    uint16_t xs = x0 + LCD_X_SHIFT, xe = x1 + LCD_X_SHIFT;
    uint16_t ys = y0 + LCD_Y_SHIFT, ye = y1 + LCD_Y_SHIFT;
    uint8_t buf[4];

    LCD_WriteCommand(pLCD, 0x2A);   // CASET
    buf[0] = xs >> 8; buf[1] = xs & 0xFF;
    buf[2] = xe >> 8; buf[3] = xe & 0xFF;
    LCD_WriteDataBytes(pLCD, buf, 4);

    LCD_WriteCommand(pLCD, 0x2B);   // RASET
    buf[0] = ys >> 8; buf[1] = ys & 0xFF;
    buf[2] = ye >> 8; buf[3] = ye & 0xFF;
    LCD_WriteDataBytes(pLCD, buf, 4);

    LCD_WriteCommand(pLCD, 0x2C);   // RAMWR
}


void LCD_DrawPixel(LCD_Handle_t *pLCD, uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT) return;
    LCD_SetAddressWindow(pLCD, x, y, x, y);
    uint8_t buf[2] = { color >> 8, color & 0xFF };
    LCD_WriteDataBytes(pLCD, buf, 2);
}

void LCD_FillColor(LCD_Handle_t *pLCD, uint16_t color)
{
    LCD_SetAddressWindow(pLCD, 0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    uint8_t hi = color >> 8, lo = color & 0xFF;
    uint32_t count = (uint32_t)LCD_WIDTH * LCD_HEIGHT;

    GPIO_WriteToOutputPin(pLCD->DCPort, pLCD->DCPin, ENABLE);
    delay_us(1);
    LCD_Select(pLCD);

    for (uint32_t i = 0; i < count; i++) {
        SPI_SendData(pLCD->pSPIHandle->pSPI, &hi, 1);
        SPI_SendData(pLCD->pSPIHandle->pSPI, &lo, 1);
    }
    LCD_Unselect(pLCD);
}

void LCD_Fill(LCD_Handle_t *pLCD, uint16_t xSta, uint16_t ySta,
              uint16_t xEnd, uint16_t yEnd, uint16_t color)
{
    if (xEnd >= LCD_WIDTH || yEnd >= LCD_HEIGHT) return;
    LCD_SetAddressWindow(pLCD, xSta, ySta, xEnd, yEnd);

    uint8_t hi = color >> 8, lo = color & 0xFF;
    uint32_t count = (uint32_t)(xEnd - xSta + 1) * (yEnd - ySta + 1);

    GPIO_WriteToOutputPin(pLCD->DCPort, pLCD->DCPin, ENABLE);
    delay_us(1);
    LCD_Select(pLCD);

    for (uint32_t i = 0; i < count; i++) {
        SPI_SendData(pLCD->pSPIHandle->pSPI, &hi, 1);
        SPI_SendData(pLCD->pSPIHandle->pSPI, &lo, 1);
    }
    LCD_Unselect(pLCD);
}


void LCD_Init(LCD_Handle_t *pLCD)
{
    // Turn on backlight if a BL pin is defined
    if (pLCD->BLPort != NULL) {
        GPIO_WriteToOutputPin(pLCD->BLPort, pLCD->BLPin, ENABLE);
    }

    // Hardware reset
    LCD_Reset(pLCD);   // uses your GPIO functions: 20ms low, 120ms high

    // ---- Sleep out ----
    LCD_WriteCommand(pLCD, 0x11);
    delay_ms(120);

    // ---- Normal display mode on ----
    LCD_WriteCommand(pLCD, 0x13);

    // ---- Display and colour format ----
    LCD_WriteCommand(pLCD, 0x36);   // MADCTL
    LCD_WriteData(pLCD, 0x00);      // RGB order, normal orientation

    LCD_WriteCommand(pLCD, 0xB6);   // JLX240 display specific
    LCD_WriteData(pLCD, 0x0A);
    LCD_WriteData(pLCD, 0x82);

    LCD_WriteCommand(pLCD, 0xB0);   // RAMCTRL
    LCD_WriteData(pLCD, 0x00);
    LCD_WriteData(pLCD, 0xE0);      // 5‑to‑6‑bit conversion

    LCD_WriteCommand(pLCD, 0x3A);   // COLMOD
    LCD_WriteData(pLCD, 0x55);      // 16‑bit RGB565
    delay_ms(10);

    // ---- Frame rate setting ----
    LCD_WriteCommand(pLCD, 0xB2);   // PORCTRL
    LCD_WriteData(pLCD, 0x0C);
    LCD_WriteData(pLCD, 0x0C);
    LCD_WriteData(pLCD, 0x00);
    LCD_WriteData(pLCD, 0x33);
    LCD_WriteData(pLCD, 0x33);

    LCD_WriteCommand(pLCD, 0xB7);   // GCTRL (VGH/VGL)
    LCD_WriteData(pLCD, 0x35);

    // ---- Power settings ----
    LCD_WriteCommand(pLCD, 0xBB);   // VCOMS
    LCD_WriteData(pLCD, 0x28);

    LCD_WriteCommand(pLCD, 0xC0);   // LCMCTRL
    LCD_WriteData(pLCD, 0x0C);

    LCD_WriteCommand(pLCD, 0xC2);   // VDVVRHEN
    LCD_WriteData(pLCD, 0x01);
    LCD_WriteData(pLCD, 0xFF);

    LCD_WriteCommand(pLCD, 0xC3);   // VRHS
    LCD_WriteData(pLCD, 0x10);

    LCD_WriteCommand(pLCD, 0xC4);   // VDVSET
    LCD_WriteData(pLCD, 0x20);

    LCD_WriteCommand(pLCD, 0xC6);   // FRCTR2
    LCD_WriteData(pLCD, 0x0F);

    LCD_WriteCommand(pLCD, 0xD0);   // PWCTRL1
    LCD_WriteData(pLCD, 0xA4);
    LCD_WriteData(pLCD, 0xA1);

    // ---- Gamma settings ----
    LCD_WriteCommand(pLCD, 0xE0);   // Positive gamma
    uint8_t gamma_p[] = {0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x32,
                         0x44, 0x42, 0x06, 0x0E, 0x12, 0x14, 0x17};
    LCD_WriteDataBytes(pLCD, gamma_p, sizeof(gamma_p));

    LCD_WriteCommand(pLCD, 0xE1);   // Negative gamma
    uint8_t gamma_n[] = {0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x31,
                         0x54, 0x47, 0x0E, 0x1C, 0x17, 0x1B, 0x1E};
    LCD_WriteDataBytes(pLCD, gamma_n, sizeof(gamma_n));

   // LCD_WriteCommand(pLCD, 0x21);   // Display inversion on
   // ---- Set column & row address (240x320) ----
    LCD_WriteCommand(pLCD, 0x2A);   // CASET
    uint8_t caset[] = {0x00, 0x00, 0x00, 0xEF};  // 0…239
    LCD_WriteDataBytes(pLCD, caset, 4);

    LCD_WriteCommand(pLCD, 0x2B);   // RASET
    uint8_t raset[] = {0x00, 0x00, 0x01, 0x3F};  // 0…319
    LCD_WriteDataBytes(pLCD, raset, 4);

    delay_ms(120);

    // ---- Display on ----
    LCD_WriteCommand(pLCD, 0x29);
    delay_ms(120);
}
// Control byte = S A2 A1 A0 MODE SER/DFR PD1 PD0
// PD1PD0 = 11 -> keep ADC/ref powered between back-to-back reads (less noise)
// PD1PD0 = 00 -> power down after the last read in the sequence

#define TOUCH_CMD_Z2  0xC0   // 1100 0000 -> A2A1A0=100, power down (last read)

static uint16_t Touch_SPI_ReadCmd(LCD_Handle_t *pLCD, uint8_t cmd)
{
    uint8_t rx_buf[2];
    uint8_t dummy = 0x00;
    uint8_t ignore;

    SPI_SendData(pLCD->pSPIHandle->pSPI, &cmd, 1);
    SPI_ReceiveData(pLCD->pSPIHandle->pSPI, &ignore, 1);

    SPI_SendData(pLCD->pSPIHandle->pSPI, &dummy, 1);
    SPI_ReceiveData(pLCD->pSPIHandle->pSPI, &rx_buf[0], 1);

    SPI_SendData(pLCD->pSPIHandle->pSPI, &dummy, 1);
    SPI_ReceiveData(pLCD->pSPIHandle->pSPI, &rx_buf[1], 1);

    return ((rx_buf[0] << 8) | rx_buf[1]) >> 4;   // 12-bit result
}

void Touch_ReadXYZ(LCD_Handle_t *pLCD, uint16_t xyz[4])
{
    GPIO_WriteToOutputPin(pLCD->Touch_CSPORT, pLCD->Touch_CSPIN, DISABLE);
    delay_us(1);

    xyz[0] = Touch_SPI_ReadCmd(pLCD, TOUCH_CMD_X);   // X-position (drivers XP,XN)
    xyz[1] = Touch_SPI_ReadCmd(pLCD, TOUCH_CMD_Y);   // Y-position (drivers YP,YN)
    xyz[2] = Touch_SPI_ReadCmd(pLCD, TOUCH_CMD_Z1);  // Z1 (drivers YP,XN)
    xyz[3] = Touch_SPI_ReadCmd(pLCD, TOUCH_CMD_Z2);  // Z2 (drivers YP,XN), powers down after

    GPIO_WriteToOutputPin(pLCD->Touch_CSPORT, pLCD->Touch_CSPIN, ENABLE);
}
void LCD_SetRotation(LCD_Handle_t *pLCD, uint8_t m)
{
    LCD_WriteCommand(pLCD, 0x36);
    switch (m) {
        case 0: LCD_WriteData(pLCD, 0x00); break;
        case 1: LCD_WriteData(pLCD, 0x60); break;
        case 2: LCD_WriteData(pLCD, 0xC0); break;
        case 3: LCD_WriteData(pLCD, 0xA0); break;
        default: break;
    }
}

void LCD_InvertColors(LCD_Handle_t *pLCD, uint8_t invert)
{
    LCD_WriteCommand(pLCD, invert ? 0x21 : 0x20);
}
void LCD_DrawLine(LCD_Handle_t *pLCD, uint16_t x0, uint16_t y0,
                  uint16_t x1, uint16_t y1, uint16_t color)
{
    int16_t dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int16_t dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;

    while (1) {
        LCD_DrawPixel(pLCD, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int16_t e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx)  { err += dx; y0 += sy; }
    }
}
void LCD_DrawImageFast(LCD_Handle_t *pLCD, const uint16_t image[])
{
    // LCD_HEIGHT   *  LCD_WIDTH ;
    for (uint16_t y = 0; y < LCD_WIDTH; y++) {
        for (uint16_t x = 0; x < LCD_HEIGHT; x++) {
            uint16_t color = image[y * LCD_HEIGHT + x];
            LCD_DrawPixel(pLCD, (LCD_WIDTH -1 ) - y,(LCD_HEIGHT  -1 ) - x, color);
        }
    }
}
void LCD_DrawSine(LCD_Handle_t*pLCD  , uint16_t start_x ,uint16_t start_y , uint16_t period ,
		uint16_t amplitude){
	  //  convert this to the angle ;

	float angle= (2.0f *(3.14f))/((float)period)  ;
	int16_t prev_x  = -1 ; int16_t prev_y = -1 ;
	for(uint16_t i = start_x  ; i<LCD_HEIGHT   ; i++){
	      int16_t diff_x  = ((int16_t)i  -(int16_t)start_x ) ;
	      int16_t diff_y  = (int16_t)(amplitude *(sinf(angle *diff_x))) ;
	      int16_t y_value = (int16_t)start_y+diff_y  ;
	      if(y_value>=LCD_WIDTH )y_value = LCD_WIDTH ;
	      if(y_value <=0) y_value  = 0 ;
	      LCD_DrawPixel(pLCD , y_value ,  i , LCD_COLOR_BLUE) ;
          if(prev_x  ==  -1  || prev_y == - 1){
        	  prev_x = i ;
        	  prev_y =  y_value ;
        	  continue ;
          }
          else{
        	  LCD_DrawLine(pLCD ,  prev_y  , prev_x , y_value  , i ,  LCD_COLOR_BLUE )  ;
        	  prev_x = i ;
        	  prev_y =  y_value ;
          }
	      delay_ms(2) ;
   }
}
void LCD_WriteText(LCD_Handle_t *pLCD, uint16_t x, uint16_t y,
                   const char *str, uint16_t color, uint16_t bg_color,
                   uint8_t scale)
{
    if (scale == 0) scale = 1;

    uint16_t cursor_x = x;
    uint16_t cursor_y = y;

    uint8_t char_width  = 5 * scale;
    uint8_t char_height = 7 * scale;
    uint8_t spacing     = scale;

    while (*str) {
        if (*str == '\n') {
            cursor_y  -= char_height + spacing;
            cursor_x = x;
            str++;
            continue;
        }
        if (*str == '\r') {
            cursor_x = x;
            str++;
            continue;
        }
        LCD_WriteChar(pLCD, cursor_x, cursor_y, *str, color, bg_color, scale);
        cursor_x += char_width + spacing;
        if ((cursor_x  + 4 )*scale  +scale> LCD_HEIGHT) {
            cursor_x = x;
            if((int16_t)cursor_y  -(int16_t)char_height   -(int16_t)spacing<=2*scale){
            	LCD_FillColor(pLCD ,  LCD_COLOR_BLACK) ;
            	cursor_y = 240  ;
            }
            else cursor_y -= char_height + spacing;
        }
        str++;
    }
}
void LCD_WriteChar(LCD_Handle_t *pLCD, uint16_t x, uint16_t y,
                             char c, uint16_t color, uint16_t bg_color,
                             uint8_t scale)
{
    if (scale == 0) scale = 1;
    if (c < 32 || c > 127) c = 32;
    uint8_t index = c - 32;

    for (uint8_t col = 0; col < 5; col++) {
        uint8_t col_data = font5x7[index][col];
        for (uint8_t row = 0; row <= 6; row++) {

            uint16_t px = x + (col);
            uint16_t py = y + (6 - row);
            for (uint8_t dy = 0; dy < scale; dy++) {
                for (uint8_t dx = 0; dx < scale; dx++) {
                    uint16_t draw_x = py * scale + dx;
                    uint16_t draw_y = px * scale + dy;

                    if (draw_x >= LCD_WIDTH || draw_y >= LCD_HEIGHT) continue;
                    if (col_data & (1 << row)) {
                        LCD_DrawPixel(pLCD, draw_x, draw_y, color);
                    } else {
                        LCD_DrawPixel(pLCD, draw_x, draw_y, bg_color);
                    }

                }
            }

        }
    }
}
