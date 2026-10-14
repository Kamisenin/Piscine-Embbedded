F_CPU?=16000000UL

CC:= avr-gcc

CFLAGS:= -Wall -Werror -Wextra -mmcu=atmega328p -DF_CPU=$(F_CPU)

SRC:=main.c

OBJ = $(SRC:%.c=%.o)

NAME=light

HEX = $(NAME).hex

BIN = $(NAME).bin

all: $(BIN) hex flash

$(BIN): $(OBJ)
	$(CC) $(OBJ) -mmcu=atmega328p -o $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

hex: $(HEX)

$(HEX):
	avr-objcopy -O ihex $(BIN) $@

flash:
	avrdude -p ATMega328p -U flash:w:$(HEX):i -c arduino -P ch340

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(BIN) $(HEX)

re : fclean all

.PHONY: hex flash clean fclean re all
