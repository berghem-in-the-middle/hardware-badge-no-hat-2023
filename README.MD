# [No Hat 2023](https://www.nohat.it) - Custom Hardware Badge

![top_layer][1]

## There we go! Did you like the hardware badge?

This year, at [No Hat 23](https://www.nohat.it), we proposed our first and fully custom hardware badge. A cool project made by Marcello, Nicola, and me, Jacopo: we designed and implemented the features, the hardware, the PCB, the firmware, the production, the assembling, the flashing, and the backend, in one word: everything. Again, fully custom, just for No Hat.

## A bit about the story.

When we started to think about this hardware badge, we wanted to do something cool but also useful, something that people could use after the conf and not only during the conf, and not just a gadget (even though it is a gadget). So, we started to imagine the possible features able to create the wow-effect to the people.\
Of course, it had to be cheap, very cheap. Usually, for HW stuff, the price goes down for lots with at least thousands of pieces. Not our case. It’s still a limited edition, by design, so the BOM (bill of materials) had to be short and cheap.\
Networking is the keyword here. It’s a fundamental idea of No Hat, in the buckets of No Hat values. That’s why we decided to design this kind of challenge where participants were pushed to network with others, as well as with sponsors and speakers.

## Where we did what

All the hardware design, the schematics, the PCB, and the firmware have been made by us. The PCBway company provided us with the PCBs and the CEEI company assembled everything (almost) that we previously purchased. Almost, as I said, because the battery holder is not SMT (surface-mount technology), so we had to manually solder it, put a drop of glue on the back, insert three batteries, and forward to the next.\
We produced a total of 250 pieces and only a few are left in the bucket. And we are extremely happy about that.\
As you could see we flashed them in real-time, in front of you, customizing it with your name that went in the My Data page.\
We decided to make it open, not only the software but also the hardware. We made it for the community.

## HARDWARE

All the components are on the TOP layer, except for the battery holder, of course. We tried to include as many IOs as possible to make it very flexible:

- 1 display
- 2 buttons
- 1 buzzer
- 1 SWDIO connector (flash connector)
- 5 pins for USART and more
- 20 LEDs
 
![badge_architecture][2]

Schematics and PCB design are inside the Hardware folder.

## FIRMWARE

We expect you already know the main features of the badge, in case you don’t, below is a summary with some tech details:

- Badge-badge connection using the 10 pins connector (5+5), using the USART and a smart hardware way to protect the pins from unwanted electric charges and annoying bouncing during data transferring (see above). The connection protocol uses a simple but effective try-wait-retry-until-you-die algorithm.
- Display management using SPI, but it is possible also to use I2C, not both of course. The library is `SSD1306`, it's quite easy to find resources online. The library allows you to print text and other basic shapes.
- A page management to scroll between pages using the NEXT button; it's the base ground for the UI.
- QR code library to generate a QR code based on the id and the value the badge has. Be careful about the size of the QR code output: the display is small, and the resolution is low, this struggled us a bit to calibrate the QR code version, which also determines the maximum data possible to be encoded.
- A virtual EEPROM to save data even if you turn off the badge. A cool (but not super healthy) way to save data with a volatile memory.
- A buzzer for a monotone sound is used during data transferring and in the party mode.
- Party mode: the ultimate way to be cool at the club, we hope you use it during your Saturday nights. Tuning on LEDs and buzzer to the rhythm of some techno music. Be careful, you cannot turn on all the LEDs together, there is a power limitation.

On the [BITM Github page](https://github.com/berghem-in-the-middle) you can find the source code of the firmware, ready to be flashed.

We developed the entire firmware from scratch. Using the DST tool and CubeIDE, we configured the microcontroller's pins to manage buttons, output pins for the buzzer and LEDs, and UART serial port for inter-badge communication. For display management, we used SPI but could easily switch to I2C without any differences. Additionally, we employed an internal hardware timer to time execution, manage input/output pins, and handle flash memory operations.\
We leaned on the ST HAL, which stands for Hardware Abstraction Layer, which simplifies the controls of the hardware, especially with the GPIO. We didn't use interrupts at all, we perform cyclic checks based on the internal clock.\
Data is stored in an emulated EEPROM since the micro does not have one. We used a native ST lib that saved us some time, even if there are some bugs here and there. For each coupled badge, we save the id and the value to avoid multiple connections with the same badge. The badge IDs can be within the range `[1 - 512]`, with `0` used to store the badge's own value.\
We implemented software debounce to manage hardware debounce when buttons are pressed. There's a mechanism performing checks approximately every 10ms to manage input and output pins.\
The badge-to-badge connection operates as a node-to-node system rather than a master/slave setup. Using a try-listen-retry algorithm with random delays, the badge with the lower delay initiates the connection, then they swap roles after data transfer. Upon completion, each badge saves the received id and value in the EEPROM, retaining data even when the badge is powered off.\
The display generates virtual pages navigated through the NEXT button. Upon reaching the QR code page, the badge acquires the paired id-value couple, encrypts the string using an `XOR` function, encodes it in base64, appends the encoded string to the backend URL as a `GET` parameter, and converts it into a QR code for display.\
The code is organized in modules, so it's quite easy to modify as you like.

## BACKEND

A very short description of how we designed the backend to store data and create the scoreboard. Whenever the participant wanted, she/he could upload the collected points online just by scanning the QR code generated by the badge. That QRcode encodes a URL with a GET parameter that is the (very weak) encryption of the couple `badgeID:badgeSCORE`. The URL points to an endpoint on Google Cloud Functions to insert data on a Firebase Realtime Database. A second endpoint has been designed to retrieve all the data of the DB and build the scoreboard.\
We expected some hacking activity here and there, of course. The password and the encryption methods were very weak (also for HW limitations, see the QRcode generation) but the result was fun to see.
![backend_db][3]
Someone tried to push data, eeeehhh?\
If you want to have more fun, give a look inside the DB, it's inside the backend folder, and check how many uploads some IDs have.\
Kudos to the guy who dumped the FW found the password, and uploaded the big value. Everything during the conference. Later, he also changed the URL of the QR code, LOL. Again, Kudos to you.

### Getting started

The first thing you can do is to flash the FW by yourself. To do so, you only need the source code (see above), the IDE (the micro is made by ST so we suggest using [STm CUBE](https://www.st.com/en/development-tools/stm32cubeide.html), and an ST-LINK (around 15 euros online).\
It should be ready to be compiled for the debug and the release configurations.\
Connect the ST-LINK to the SWDIO connector, check above the pin configuration.\
Check the main.h (Core/Inc) and scroll to the bottom, there you find the first variables to change and to start to play on, like `BADGE_ID` and `BADGE_TYPE`.
Something cooler to do, not in order of difficulty:

- Add another page and print something;
- Use the I2C to communicate with the display;
- Re-assign the party button to another party mode made by you;
- Module the PWM used to generate different buzzer tones. Some Mozart composition?
- Communicate with an external board using the USART;
- Change the display with another peripheral and control it using SPI or I2C;

We are really interested to know your experiments and tests, so share it on [X](https://twitter.com/nohatcon) or [Linkedin](https://www.linkedin.com/company/berghem-in-the-middle/mycompany/) and tag us.

Furthermore, we authors are very happy to answer your questions. You can reach out to Nicola on [LinkedIn](https://www.linkedin.com/in/nicola-vaccaro/) and [Telegram](https://t.me/HighSeconds), Marcello on [LinkedIn](https://www.linkedin.com/in/marcello-federici-441b28b/) and [Telegram](https://t.me/Marcio28_99), and me, Jacopo on [LinkedIn](https://www.linkedin.com/in/jacopofederici/) and [Telegram](https://t.me/j_jey).

You know what? We are already working on next year’s badge.

[1]: /Hardware/banner.png
[2]: /Hardware/badgeNOHAT23_TOP_CONNECTIONS.jpg
[3]: /Backend/hw_scoreboard.png
