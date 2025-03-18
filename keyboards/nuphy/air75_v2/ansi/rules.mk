SRC += user_kb.c
SRC += rf.c
SRC += ws2812_bitbang.c ws2812_side_driver.c
SRC += rgb.c

SRC += mcu_pwr.c sleep.c rf_driver.c rf_queue.c debounce.c redefine.c

UART_DRIVER_REQUIRED = yes
OS_DETECTION_ENABLE = yes

GCC_EXTRA_OPTIONS =
OPT = s $(GCC_EXTRA_OPTIONS)

CUSTOM_MATRIX = lite
SRC += matrix.c

