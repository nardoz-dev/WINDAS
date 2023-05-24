# WINDFORME ![image](/docs/sharedpictures/logo.png)

`WINDFORME` - IoT University Project for the IoT 2022 Course @ Sapienza University of Rome.

## Member

| **Name and Surname** | **Linkedin** | **GitHub** |
| :---: | :---: | :---: |
| `Andrea Nardocci ` | [![name](https://github.com/nardoz-dev/projectName/blob/main/docs/sharedpictures/LogoIn.png)](https://www.linkedin.com/in/andrea-nardocci) | [![name](https://github.com/nardoz-dev/projectName/blob/main/docs/sharedpictures/GitHubLogo.png)](https://github.com/nardoz-dev) |

## Introduction

`WINDFORME` Project.

The Internet of Things (IoT) has revolutionized the way we interact with technology in our daily lives, and this project is no exception. Introducing the WINDFORME a fully automated fan system solution designed to bring comfort and convenience to your home or office. This system leverages the power of IoT to create a smart, self-regulating environment that adjusts to your changing needs. The fan system is equipped with a range of sensors and controls that work in tandem to monitor the temperature. It then uses this information to automatically adjust the speed of the fan, ensuring that you always have a comfortable and optimal indoor climate. Whether you're at home, at work, or away, this system takes care of your comfort, providing you with peace of mind and a more enjoyable indoor environment.

## Architecture Design
![image](/docs/sharedpictures/HWArchitecture.png)

## Hardware

* **Board : `Nucleo f401RE`**

  The [Nucleo F401RE](https://www.st.com/en/evaluation-tools/nucleo-f401re.html) is a development board based on the STM32F401RE microcontroller. The STM32F401RE is a high-performance microcontroller from STMicroelectronics that is part of the STM32 family of 32-bit Arm Cortex-M4 microcontrollers. The board also provides access to the STM32 microcontroller's rich range of hardware peripherals, making it a suitable platform for developing a wide range of applications, such as those in the fields of Internet of Things (IoT), robotics, and other embedded systems.
  The board includes an ARM Cortex-M4 core microcontroller running at up to 84MHz with 512KB of flash memory and 96KB of RAM. It also includes a variety of peripherals, including multiple timers, communication interfaces (UART, SPI, I2C, CAN), and analog-to-digital converters.

* **Humidity Sensor : `DHT11`**

  The [DHT-11](https://www.adrirobot.it/sensore_dht11/) is a low-cost digital temperature and humidity sensor. It uses a capacitive humidity sensing element and a thermistor to measure the ambient air temperature and humidity.
  It can measure temperatures between 0 and 50 degrees Celsius, with an accuracy of ±2 degrees Celsius.
  In terms of electrical specifications, the DHT-11 sensor operates at a supply voltage of 3.3V to 5V and has a maximum current consumption of 2.5 mA.

* **7-Segment display : `5611BH`**

  A 7-segment display [5611BH](https://datasheetspdf.com/pdf/1404694/XILTX/5611BH/1), is a type of electronic display device used for displaying decimal numerals. It consists of seven individual segments, labeled A through G, that can be illuminated to form the numerals 0-9. The display has a forward voltage drop of 2.1V and a maximum forward current of 20mA for each segment.

* **Motor : `Mabuchi FC-130RA/SA`**

  The [FC-130RA](https://www.neuhold-elektronik.at/datenblatt/fc_130rasa.pdf) is a small DC motor. It is a brushed motor, meaning it uses a system of brushes to transfer electrical energy to the motor's rotor, which then converts this energy into mechanical rotation.
  The FC-130RA/SA motor has a nominal voltage of 3V and can operate in a range of 1.5V to 4.5V. It has a no-load speed of 12,500 RPM and can produce a maximum torque of 7.4 g-cm. The motor has a maximum efficiency of 70% and a maximum power output of 1.2 W.


* **Transistor : `2N3904`**

  The [2N3904](https://www.onsemi.com/pdf/datasheet/2n3903-d.pdf) is a small signal NPN bipolar junction transistor that can be used to control a flow of electric current in which a small amount of current in the base conductor controls a larger current between collector and emitter. They can be used to amplify a weak signal, like a switch. In normal state, 2N3904 acts as reverse biased and there's no conduction between Collector & Emitter. When small voltage applies at its Base Terminal(normally 5V), 2N3904 converts its state from reverse to forward biased and conventional current starts flowing from Collector to Emitter.
  The transistor has a maximum collector current of 200 mA, a maximum collector-emitter voltage of 40 V, and a maximum power dissipation of 625 mW. 


## Software

* ### RIOT OS

  RIOT powers the Internet of Things like Linux powers the Internet. RIOT is an open-source microkernel-based operating system, designed to match the requirements of Internet of Things (IoT) devices and other embedded devices. These requirements include a very low memory footprint (on the order of a few kilobytes), high energy efficiency, real-time capabilities, support most low-power IoT devices, microcontroller architectures (32-bit, 16-bit, 8-bit), and external devices. RIOT aims to implement all relevant open standards supporting an Internet of Things that is connected, secure, durable & privacy-friendly.

  A good high-level overview can be found in the article [RIOT: An Open Source Operating System for Low-End Embedded Devices in the IoT](https://www.riot-os.org/assets/pdfs/riot-ieeeiotjournal-2018.pdf) (IEEE Internet of Things Journal, December 2018).

## Network 

The following image represents a description of the entire network system that we can find within the project.

![image](/docs/sharedpictures/NetworkArchitecture.png)


* **Board**  
  RIOT provides the ethos_uhcpd tool to provide network connectivity to an MCU via the USB connection. In the case of the STM32 Nucleo-64 F401RE this is very useful as the development board does not provide a wireless network interface.
  The ethos_uhcpd tool builds on top of the serial interface, ethos (Ethernet Over Serial) and UHCP (micro Host Configuration Protocol). Ethos multiplexes serial data to separate ethernet packets from shell commands. The ethos_uhcpd tool will provide network connectivity through the TAP interface. After all connection , the board send messages through the MQTT protocol.
  
* **Protocol**  
  MQTT is a lightweight and flexible network protocol that uses a central message broker to coordinate communication among the peers of the network. Peers can interact with the broker by sending and receiving messages within specific topis. By organizing message exchanges into topics, devices can exchange data or commands with each other or in a group. This structure can allow the application developer to set up a hierarchy of devices with complex controlling schemes.

* **Mosquitto.rsmb**  
  Then the board is connected to mosquitto.rsmb : the Really Small Message Broker is a server implementation of the MQTT and MQTT-SN protocols. Any client that implements this protocol properly can use this server for sending and receiving messages.

* **Transparent Bridge**  
  A transparent bridge written in python that works as a bridge exchanging messages using MQTT between broker (In our case the broker : are the "board" and the AwS IoT Core). It reads messages from the local broker with "topic_data" and publishes them to AWS IoTCore on the same topic. It also reads messages from AWS IoT Core with "topic_board" and publishes them on the local broker with the same topic.

* **AwS Services**  
  This is the cloud part. Once the data arrives to AwS IoT Core, it will be processed and store on AWS Services : DynamoDB. Then are available some Lambda function that are invoked from the WebApp.
    
  This process varies from user to user, in our case we set up our rules, our iam, all this in order to grant specific permissions for different AwS services.

  AwS Amplify then is used to host our application. The User Interface is fully understandable, and it display and monitor the temperature and humidity data. It can also display some measurements that we make in order to be able to have an overview of the trend of the data collected by the sensor, such as average, maximum, minimum and variation. Futhermore from our web application we can send special commands to the board.

## Working Principle

This IoT system plays the role of monitoring temperatures and humidity ***(with the DHT11 sensor )*** and then triggers a cooling system if the temperature exceeds the threshold temperature. In addition, there is a 7-segment display ***(5611BH)*** which indicates the status of the cooling system, which consists of a motor ***(mabuchi FC-130 RA/SA)*** and a fan. The status is indicated with 0 if the fan is off, 1 if the fan is on, and 2 if the fan has received an external command that has changed system modes.

The system has three system modes, 0 : AUTO , 1 : ON , 2 : OFF , where they indicate respectively : system in automatic mode, system always on, system always off. They refer to the operation of the cooling engine, also , can be set by the user remotely through the user interface provided in the web application.
The board during the first startup, is initialized with the system in automatic mode. The automatic mode consists of performing computational calculations on the data read from the sensors, i.e. we check whether the temperature read from the sensor is greater than the threshold temperature, if it is the case the cooling motor is triggered. This is achieve by the activation of the GPIO pin linked to the base of transistor, in this way the transistor ***(2N3904)*** allows current flow into the motor. Timers and flags have been added to ensure that the engine start procedure, like the shutdown procedure do not cause interference with the system and are not repeated in case the same condition is regenerated repeatedly. 
The ON mode, always keeps the engine running, vice versa for the OFF mode, however, both ensure the sensor data is read and sent to allow the user to monitor the data from the graphs in the web app.

The polling and 'sending of data from the board into the cloud was decided to make it periodic with an interval of about 10 seconds. This avoids an aggregation of the data being sent, preventing any traffic congestion. In addition, much of the computation is done at the edge level, avoiding high latency when sending data to the cloud, all because the board can support computation. In this way, the only thing we send to the cloud is a message containing the temperature and humidity. 

## Network

La gestione e l'invio dei dati all'interno di questo progetto viene effettuata usando il protocollo MQTT. Abbiamo quindi la board collegata al server mosquitto.rsmb e il nostro servizio AwS IoTCore che sono collegati attraverso un transparent bridge che abbiamo scritto in python. Abbiamo quindi il nostro broker mqtt che abbiamo sottoscritto al topic "topic_data" e il nostro broker AwS IoTCore che invece è sottoscritto al topic "topic_board".

La board costruisce l'informazione da mandare e la pubblica sul topic : *topic_data*, il transparent bridge cattura questo messaggio e lo spedisce invocando una lambda function su un topic al quale abbiamo fatto iscrivere l'AwSIoTCore per poi salvarla su una tabella in DynamoDB.
Un'importante aspetto di questo funzionamento è la Policy che associamo al nostro oggetto in AwSIoTCore, ecco un [esempio di policy]() .
Il messaggio inviato è piu piccolo di 15 bytesm 


E' stato implementata la comunicazione bidirezionale, quindi dalla nostra webapplication possiamo comunicare con la board. 
Per il momento l'unica funzionalità che è stata implementata è l'invio di un messaggio che cambia la modalità di sistema dell'intera applicazione. Il messaggio generato dalla web application viene pubblicato attraverso una lambda function sul topic : *topic_board* . Il transparent bridge si attiva per trasportare il messaggio pubblicato sul broker della board, in modo tale da triggerare la funzione di callback del protocollo mqtt. Viene così processato il messaggio in modo da estrapolare il comando per poterlo eseguire.

Un'altra aspetto importante è l'assegnazione delle IAM Policy per le lambda function, le quale in base al tipo di servizio che offrono hanno bisogno delle autorizzazioni. 

Inoltre è possibile azionare dalla WebApplication la syncronizzazione del retrieve delle informazioni ogni qualvolta la board invia un messaggio al database. Questo clamping è stato gestito tenendo in considerazione le latenze che subiscono il messaggio in fase di invio e ricezione.


## Data Processing

Data Processed on the by the Board
In general the amount of the data received from the sensors depends on how well the threads that handle the measurement are synchronized. If they are perfectly in sync, then the system would receive a total of 24 bits (8 bits for temperature, 8 bits for relative humidity and 8 bits for soil moisture).

This data is then formatted in a message that is then sent to the broker. In particular to reduce the number of bytes sent by the device in the network the message has a payload size of 9 bytes when sending data about temperature and humidity, and size of 5 bytes when sending data about soil moisture. Obviously in addition to the payload the message has a header but in this case is small thanks to the use of mqtt-sn protocol.


### Edge Computing

The device aggregates the information on temperature level and periodically sends comulative reports. This has the effect of reducing network load and forwarding only necessary information to the rest of the system via MQTT.
The trigger logic for starting the motor is running fully on the device itself, so we gain less influence by network latency. 

/* Ampliamo l'edge computing scrivendo tutte le fasi in cui è stato effettuato edge computing all'interno del mio dispositivo */ 

### Cloud Computing

The only part that works on cloud , so in the specific on AWS. It recieves the temperature data from the devices and then are stored on DynamoDB. 

## Setup & Run 

> link to mosquitto rsmb

## Demo video
Homemade demo presentation of project, link here. 











 
