# TW.R: Two-Wire xxx Register (p 622)

## TWBR
 - TWBR (bit rate). Set once at init and it fixes the SCL speed.This is where your 100 kHz comes from.
It sets the speed this board uses when it is master.
Only the master generates the SCL clock
 A bigger TWBR means a slower SCL
 ```bash
 SCL = F_CPU / (16 + 2 × TWBR × prescaler)
 ```

 - TWCR (control). You write it at every step to say what to do next.


## TWCR
### Bit 7: TWINT (TWI Interrupt Flag)
Set to 1 by hardware when the current operation is done (START sent, byte sent/received, ...).
While TWINT = 1, SCL is held low: the bus is paused, waiting for you.
Cleared by writing 1 to it. That write also starts the next operation.
Wait for it: while (!(TWCR & (1 << TWINT)));
Not cleared automatically when the ISR runs: your ISR must clear it.
### Bit 6: TWEA (TWI Enable Acknowledge)
1 = the TWI sends an ACK when:
it receives its own slave address (TWAR),
it receives a data byte (as slave or master receiver).
0 = sends NACK / the TWI is "virtually disconnected" as a slave.
A slave must have TWEA = 1 to answer its address.
### Bit 5: TWSTA (TWI START Condition)
1 = generate a START (or repeated START) as soon as the bus is free.
Not cleared by hardware: you must leave it out of your next TWCR write.
### Bit 4: TWSTO (TWI STOP Condition)
1 (in master mode) = generate a STOP.
Cleared automatically by hardware once the STOP is sent.
No TWINT after a STOP: don't wait for it.
In slave mode: used to recover from a bus error.
### Bit 3: TWWC (TWI Write Collision Flag), read-only
Set when you write TWDR while TWINT = 0 (wrong moment).
Cleared when you write TWDR while TWINT = 1.
### Bit 2: TWEN (TWI Enable)
1 = TWI on. Takes over PC4 (SDA) / PC5 (SCL).
0 = TWI off, any ongoing transfer is aborted.
Off then on = reset of a stuck TWI.
Always include it in every TWCR write.
### Bit 1: reserved
Always reads 0.
### Bit 0: TWIE (TWI Interrupt Enable)
1 = TWI_vect interrupt fires while TWINT = 1 (needs sei()).


## TWAR
### Bits 7..1: TWA6–TWA0 (TWI Slave Address)
The board's own 7-bit address. You choose it and write it in software at init.
The hardware compares every address byte on the bus with these bits. On a match, it ACKs, as long as TWEA = 1 in TWCR, and TWSR gives 0x60 (write) or 0xA8 (read).
The address sits in bits 7..1, hence the shift: TWAR = addr << 1;
### Bit 0: TWGCE (TWI General Call Recognition Enable)
1 = also answer the general call address 0x00 (a "broadcast" to every slave).
0 = ignore it. You can leave it at 0.


## TWSR
### Bits 7..3: TWS7–TWS3 (TWI Status), read-only
Written by hardware each time an operation ends (when TWINT goes to 1).
Tells you what just happened on the bus. You read it to decide the next step.
Always read with the mask: TWSR & 0xF8.

Master (common)
Code	Meaning
0x08	START sent
0x10	Repeated START sent
0x38	Arbitration lost (another master won the bus)
Master Transmitter (master writes to slave)
Code	Meaning
0x18	SLA+W sent, ACK received
0x20	SLA+W sent, NACK received (nobody answered)
0x28	Data sent, ACK received
0x30	Data sent, NACK received
Master Receiver (master reads from slave)
Code	Meaning
0x40	SLA+R sent, ACK received
0x48	SLA+R sent, NACK received
0x50	Data received, ACK returned
0x58	Data received, NACK returned
Slave Receiver (slave gets written to)
Code	Meaning
0x60	Own SLA+W received, ACK returned
0x68	Lost arbitration as master, then own SLA+W received, ACK returned
0x70	General call (0x00) received, ACK returned
0x78	Lost arbitration as master, then general call received, ACK returned
0x80	Data received (own address), ACK returned
0x88	Data received (own address), NACK returned
0x90	Data received (general call), ACK returned
0x98	Data received (general call), NACK returned
0xA0	STOP or repeated START received while addressed
Slave Transmitter (slave gets read from)
Code	Meaning
0xA8	Own SLA+R received, ACK returned
0xB0	Lost arbitration as master, then own SLA+R received, ACK returned
0xB8	Data sent, ACK received
0xC0	Data sent, NACK received
0xC8	Last data byte sent (TWEA = 0), ACK received
Miscellaneous
Code	Meaning
0xF8	No relevant state (TWINT = 0)
0x00	Bus error (illegal START/STOP). Recover: write TWINT + TWSTO + TWEN
### Bit 2: reserved
Always reads 0.
### Bits 1..0: TWPS1–TWPS0 (TWI Prescaler), read/write
Set once at init by the program.
Used in the SCL formula: SCL = F_CPU / (16 + 2 * TWBR * prescaler)

 - TWDR (data). Write the byte to send (address or data) into it, or read the received byte from it.

 ## TWDR
 data register to read / write
 Only access TWDR while TWINT = 1 (the TWI is idle, waiting for you)