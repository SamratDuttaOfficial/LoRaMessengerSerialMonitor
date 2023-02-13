# LoRa_Messenger_with_Serial_Monitor
Long Range Messenger using LoRa and ESP32, used with Serial Monitor (or Serial USB Terminal on smartphones).

Connect ESP 32 with LoRa:

| LoRa pin 	| ESP32 (ESP-WROOM-32) pin  |
|:---------:|:-------------------------:|
|ANA		|       (Antenna)			|
|GND        | 		GND 				|
|DIO3 		|       don’t connect 		|
|DIO4       | 		don’t connect 		|
|3.3V       | 		3.3V 				|
|DIO0       | 		GPIO 2 				|
|DIO1       | 		don’t connect 		|
|DIO2       | 		don’t connect 		|
|GND        | 		don’t connect 		|
|DIO5       | 		don’t connect 		|
|RESET      | 		GPIO 14  			|
|NSS        | 		GPIO 5  			|
|SCK        | 		GPIO 18  			|
|MOSI       | 		GPIO 23  			|
|MISO       | 		GPIO 19  			|
|GND        | 		don’t connect  		|

Serial Monitor on 115200. 
Baud rate: 115200. 

LoRa Frequency: 433MHz. (India). 
433MHz for Asia. 
866MHz for Europe. 
915MHz for North America. 

Change 'localAddress' according to your choice. Make sure to change the serial monitor messages that appear in the startup according to your chosen address. 
It ranges from 0-0xFF. 0x means the address is represented in Hexadecimal. 0xFF is 255 in byte format. In hex, FF is  15*(16^1)+ 15*(16^0) = 255. 
Capitalization doesn't matter here. 

Sync Word: 0xF3. Change it according to your choice. It also ranges from 0 to 0xFF. 
