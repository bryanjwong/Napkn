# Napkn #
<img src="https://challengepost-s3-challengepost.netdna-ssl.com/photos/production/software_photos/000/908/858/datas/gallery.jpg" style="width:50px"/>
# IDEA Hacks 2020 - 1st Place Winner! 

### Bryan Wong, Caleb Terrill, Fred Chu, Lucas Wolter, Caitlyn Chau 
## [Slide Deck](https://docs.google.com/presentation/d/1dGl8s7gmYrvF6xw_J4Y-Q9_lGWD_yz9ZMoIB_ys8bzU/edit?usp=sharing) | [Videos](https://drive.google.com/drive/folders/11-iaHL7j0u7RTrozLryqRc23VB2-lLvD?usp=sharing) | [Website](https://bryanjwong.github.io/Napkn/) | [DevPost](https://devpost.com/software/napkn-kavgmw)

## Intro 
Trying to find a table in a room packed full to the brim with people can be extremely stressful. Imagine you're famished after spending the entire day hard at work, only to enter a dining hall and find that there isn't a single table for you to eat at. Or say, perhaps, you finally find a table and set down a few napkins to reserve it (customary for UCLA students), but come back later to find that some clueless freshman has stolen your seat!

 Well, it's time to throw away those napkins, **Napkn is here!**

## About Napkn 

Napkn is a convenient and intuitive tabletop IoT table reservation device. With a simple tap of a RFID-enabled card, users can check in to a table, reserving it for a predefined duration. Once the user is finished using their reserved dining hall table, library table, etc., all it takes is another tap of their RFID card to free the table for use. This check in procedure doubles as a user verification system, ensuring that tables go to UCLA students and authorized guests. A mini OLED display, a pair of red/green LEDs, and a transistor-amplified speaker provide satisfying visual and auditory feedback for users.

## Internet of Things: Google Firebase 
<img src="https://challengepost-s3-challengepost.netdna-ssl.com/photos/production/software_photos/000/910/347/datas/gallery.jpg" style="max-width:75%;max-height:75%"/>

Each Napkn is built with WiFi functionality, allowing it to communicate with our real-time Google Firebase Database and update information about device usage and user reservations. This information is made public through the [Napkn website](https://bryanjwong.github.io/Napkn), allowing users to get extremely accurate metrics on dining/study hall traffic. Additionally, during periods of high traffic, parameters such as the checkin time limit and maximum number of consecutive checkins are modified to help alleviate overcrowding and help ensure everybody can get a table.

# Hardware 
<img src="https://challengepost-s3-challengepost.netdna-ssl.com/photos/production/software_photos/000/908/862/datas/gallery.jpg" style="max-width:75%;max-height:75%"/>
Each Napkn consists of a variety of input and output components built around a WiFi-enabled ESP32 microcontroller. With 2 different communication protocols and a NMOS transistor amplifier circuit, building the circuit proved quite the challenge!

Here are the parts we used for each Napkn:
* [DOIT Esp32 DevKit v1](https://docs.zerynth.com/latest/official/board.zerynth.doit_esp32/docs/index.html)
* [Sunfounder MFRC522 RFID Module](http://wiki.sunfounder.cc/images/c/c6/MFRC522_datasheet.pdf)
* [DIYMall Mini OLED Display](https://drive.google.com/file/d/1qkexPET5MUICAarGOtlnQk4F-JEKkrHZ/view)
* [Magnetic Speaker](https://cdn-shop.adafruit.com/datasheets/P1898.pdf)
* 3D-Printed Chassis
* NMOS Transistor
* Green LED
* Red LED
* Perfboard
* 9V Battery and Connectors
* Assorted Resistors

The RFID module is controlled with SPI and the OLED display is controlled with I2C. The speaker is amplified with a low-side driver circuit using an NMOS transistor. The microcontroller and circuitry was soldered onto a perfboard with jumper wire connections to each component, all of which fits nicely inside the 3D-Printed Chassis.

## Software 
<img src="https://challengepost-s3-challengepost.netdna-ssl.com/photos/production/software_photos/000/910/358/datas/gallery.jpg" style="max-width:75%;max-height:75%"/>

The code that runs on each Napkn is `Napkn.ino`, located in the `Napkn` folder. By using a Firebase library we found online, we were able to connect to our database. Upon scanning an RFID card, the Napkn will communicate with Firebase to determine the correct action to take. For example, if the user has checked out too many tables, the database will notify the Napkn to reject the checkin. These cases that could reject users are dynamically set, so that they can be modified from the database or from an external script based on dining/study hall traffic. If a user is successfully checked in, the Napkn will update the database with its status, the user's RFID number, and the number of tables the user has checked in. Since reading and writing from Firebase can have high latency, we minimized the number of reads and writes by using several local variables stored on the Napkn that are synced with the database upon startup. This way, if our Napkn crashes, it is able to recover its state from the database.

In order for the Napkn properties, we used several libraries, many of which were modified due to compatibility issues. **In order for Napkn to work properly, you must add the libraries inside the `libraries` folder into your `Arduino/libraries` folder!**

Here is a list of the libraries we used:
* [IOXhop_FirebaseESP32](https://github.com/ioxhop/IOXhop_FirebaseESP32)
* [MFRC522](https://github.com/miguelbalboa/rfid)
* [ArduinoJson](https://arduinojson.org/)
* Adafruit SSD1306 (for OLED)
* Adafruit GFX (for OLED)
* pitches (labelled notes for ease of use with speaker)

<img src="https://challengepost-s3-challengepost.netdna-ssl.com/photos/production/software_photos/000/910/346/datas/gallery.jpg" style="max-width:25%;max-height:25%"/>

We also created a [simple website](https://bryanjwong.github.io/Napkn/) using HTML, CSS, and Javascript to display table occupancy to users. `app.js` pulls information from Firebase and `index.html` uses HTML and CSS to display it. The website pulls information from Firebase and displays it in an easy-to-understand format. In the future, we would like to add user-end functionality, such as reserving tables.

## Improvements 
In order to improve the Napkn experience, there are several designs and optimizations we could explore. From a hardware perspective, designing a PCB would allow for a much better form factor. We could also improve the durability and materials used for the exterior. On the software side, implementing a table map on the website could allow users to know exactly which tables are open. Given more Napkn modules, we would like to implement a server-side algorithm that automatically senses dining/study hall traffic and updates our database's dynamic values for checkin duration and number of tables allowed.
