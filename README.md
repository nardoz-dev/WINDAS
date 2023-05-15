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

* **board : `Nucleo f401RE`**

  The [Nucleo F401RE](https://www.st.com/en/evaluation-tools/nucleo-f401re.html) is a development board based on the STM32F401RE microcontroller. The STM32F401RE is a high-performance microcontroller from STMicroelectronics that is part of the STM32 family of 32-bit Arm Cortex-M4 microcontrollers. The board also provides access to the STM32 microcontroller's rich range of hardware peripherals, making it a suitable platform for developing a wide range of applications, such as those in the fields of Internet of Things (IoT), robotics, and other embedded systems.

  The board includes an ARM Cortex-M4 core microcontroller running at up to 84MHz with 512KB of flash memory and 96KB of RAM. It also includes a variety of peripherals, including multiple timers, communication interfaces (UART, SPI, I2C, CAN), and analog-to-digital converters.

  The Nucleo F401RE board has a built-in debugger and programmer, allowing developers to easily program and debug their code. It also features an Arduino-compatible pinout, allowing it to be easily interfaced with Arduino shields and other compatible hardware.

* **Humidity Sensor : `DHT11`**

  The [DHT-11](https://www.adrirobot.it/sensore_dht11/) is a low-cost digital temperature and humidity sensor. It uses a capacitive humidity sensing element and a thermistor to measure the ambient air temperature and humidity. The sensor uses a single-wire digital interface to communicate with the microcontroller, making it easy to integrate into a variety of projects.

  The DHT-11 sensor is used in this projects to measure the surrounding air's relative temperature. 
  The DHT-11 sensor is commonly used in projects related to home automation, weather stations, and other applications where temperature and humidity readings are needed. It is known for its accuracy, reliability, and low cost, making it a popular choice among hobbyists and makers.

  It can measure temperatures between 0 and 50 degrees Celsius, with an accuracy of ±2 degrees Celsius.
  In terms of electrical specifications, the DHT-11 sensor operates at a supply voltage of 3.3V to 5V and has a maximum current consumption of 2.5 mA.

* **7-Segment display : `5611BH`**

  A 7-segment display [5611BH](https://datasheetspdf.com/pdf/1404694/XILTX/5611BH/1), is a type of electronic display device used for displaying decimal numerals. It consists of seven individual segments that can be illuminated to form the numerals 0-9. 

  The display has a forward voltage drop of 2.1V and a maximum forward current of 20mA for each segment. It has a common anode pin, which is used to supply power to the display, and seven cathode pins, one for each segment.

  Each segment of the display can be controlled independently by supplying a voltage to its corresponding cathode pin. The segments are labeled A through G, and each segment can be turned on or off to display different numbers or letters.

  The purpose of the display is only to visualize the status of the global system. It display 0 if it is off, otherwise 1.

* **Motor : `Mabuchi FC-130RA/SA`**

  The [FC-130RA](https://www.neuhold-elektronik.at/datenblatt/fc_130rasa.pdf) is a small DC motor. It is a brushed motor, meaning it uses a system of brushes to transfer electrical energy to the motor's rotor, which then converts this energy into mechanical rotation.

  The FC-130RA/SA motor has a nominal voltage of 3V and can operate in a range of 1.5V to 4.5V. It has a no-load speed of 12,500 RPM and can produce a maximum torque of 7.4 g-cm. The motor has a maximum efficiency of 70% and a maximum power output of 1.2 W.

  It is wiring with the transistor 2N3904 in order to be activated only from a signal that come from the board. So his working is to rotate in order to refresh the environment. It can help to cool the environment because we have attached in his mettalic bar, a plastic fan in order to be able to generate air.

* **Transistor : `2N3904`**

  The [2N3904](https://www.onsemi.com/pdf/datasheet/2n3903-d.pdf) is a small signal NPN bipolar junction transistor which has three leads with a specific function. Named rispectively : collector,base,emitter.

  The concept behind the work is that : when a small current flows into the base, the transistor turns on and allows a larger current to flow between the collector and emitter. The amount of current that flows is proportional to the current flowing into the base. This property makes the 2N3904 useful as an switching device, in order to turn off or on the motor. 

  The trigger of the flowing of the current is given by the board when the dht sensor catch a value of the temperature that is greater than the threshold settings value(25°).

  The transistor has a maximum collector current of 200 mA, a maximum collector-emitter voltage of 40 V, and a maximum power dissipation of 625 mW. 


## Software
### RIOT OS
RIOT powers the Internet of Things like Linux powers the Internet. RIOT is an open-source microkernel-based operating system, designed to match the requirements of Internet of Things (IoT) devices and other embedded devices. These requirements include a very low memory footprint (on the order of a few kilobytes), high energy efficiency, real-time capabilities, support most low-power IoT devices, microcontroller architectures (32-bit, 16-bit, 8-bit), and external devices. RIOT aims to implement all relevant open standards supporting an Internet of Things that is connected, secure, durable & privacy-friendly.

A good high-level overview can be found in the article [RIOT: An Open Source Operating System for Low-End Embedded Devices in the IoT](https://www.riot-os.org/assets/pdfs/riot-ieeeiotjournal-2018.pdf) (IEEE Internet of Things Journal, December 2018).

## Network 

The board communicates via MQTT to AWS IoT core( AWS cloud service ). MQTT is a lightweight messaging protocol designed for IoT (Internet of Things) applications, where bandwidth and power consumption are critical factors.
In our application, the packets are sent via ethos.h which is a library of RIOT OS which provide a set of functions for working with network interfaces and protocols like MQTT. Then through a transparent MQTT-SN/MQTT bridge, component that enables communication betwenn MQTT-SN devices and MQTT brokers using the standard MQTT protocol, and finally over the internet to the AWS endpoint. On the user side, a Python server connects to AWS to retrieve the device data and interact with it as needed.

![image](/docs/sharedpictures/NetworkArchitecture.png)

Type of packets sent by the device over MQTT:

Temperature Data Repors
Sent whenever the device temperature changes, either by the user via the remote, or because the motor was triggered. They contain a playload of the form {"temperature_data": 0, "triggered": 0}

Additionally, the device listens for shadow updates coming from the web interface, so that the motor can be remotely controlled.

## Data Processing
### Edge Computing

The device aggregates the information on temperature level and periodically sends comulative reports. This has the effect of reducing network load and forwarding only necessary information to the rest of the system via MQTT.
The trigger logic for starting the motor is running fully on the device itself, so we gain less influence by network latency. 

### Cloud Computing

The only part that works on cloud , so in the specific on AWS. It recieves the temperature data from the devices and then are stored on DynamoDB. 
Here the steps :  
## How to run
## Demo video

Homemade demo presentation of project, link here. 
