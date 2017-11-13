# chibi-ssd1306
Interactive shell for write and draw on the 0.96" 128x64 OLED SSD1306 SPI with STM32F103C8T6 &amp; ChibiOS 16.x

Fonts created with great program
http://www.riuson.com/lcd-image-converter

0.96" OLED 128x64 SSD1306 SPI version connected to SPI1

PA4 - SSD1306 CS

PA5 - SSD1306 SCK

PA6 - SSD1306 DC

PA7 - SSD1306 SDA

Chibios shell active on Serial over USB

Supported commands:

  on - display on,
  
  off - display off,
  
  cls - clear screen,
  
  pixel x y - draw pixel with default color,
  
  xy x y - set position to x,y,
  
  putc char - put character with default font & color,
  
  puts string - put string with default font & color,
  
  color c - set default color c=[0|1],
  
  font f - set default font f=[0-3] (0 - Font8x13, 1 - Font10x17, 2- Font16x24, 3 - Symbol15x15),
  
  line x1 y1 x2 y2 - draw on line from x1,y1 to x2,y2,
  
  rect x y w h - draw on rectangle from x,y with w - width and h - height,
  
  frect x y w h - draw on filled rectangle from x,y with w - width and h - height,
  
  circle x y r - draw on circle with x,y and r - radius,
  
  fcircle x y r - draw on filled circle with x,y and r - radius,
  
  reboot - reboot
