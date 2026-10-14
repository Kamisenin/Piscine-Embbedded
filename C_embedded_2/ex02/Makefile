F_CPU?=16000000UL

MMCU=atmega328p

BAUD?=115200

CC:= avr-gcc

CFLAGS:= -Wall -Werror -Wextra -mmcu=$(MMCU) -DF_CPU=$(F_CPU) -DUART_BAUDRATE=$(BAUD) -Os

SRC:=main.c

OBJ = $(SRC:%.c=%.o)

NAME=firmware

HEX = $(NAME).hex

BIN = $(NAME).bin

all: $(BIN) hex flash

$(BIN): $(OBJ)
	$(CC) -mmcu=$(MMCU) $(OBJ) -o $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

hex:
	avr-objcopy -O ihex $(BIN) $(HEX)

flash:
	avrdude -p $(MMCU) -U flash:w:$(HEX):i -c arduino -P ch340 -b $(BAUD)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(BIN) $(HEX)

re : fclean all

.PHONY: hex flash clean fclean re all
