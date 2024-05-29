#ifndef I2CLCD_H
#define I2CLCD_H

#include "esp_log.h"
#include "driver/i2c.h"
#include "unistd.h"

void lcd_init (void);   

void lcd_send_cmd (char cmd);  

void lcd_send_data (char data);  

void lcd_send_string (char *str);  

void lcd_put_cur(int row, int col);  

void lcd_clear (void);

#endif