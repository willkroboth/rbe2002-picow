# Pico W code for RBE 2002

The [Romi](https://www.pololu.com/docs/0J69/all) robot used in RBE 2002 is usually controlled by a ATmega32U4. This is usually sufficient, but the ATmega32U4 is limited by a small amount of RAM (2.5 KB of SRAM) and lacks wireless functionality. 

The Raspberry Pi [Pico W](https://docs.sparkfun.com/SparkFun_XRP_Controller/assets/component_documentation/pico-w-datasheet.pdf) improves upon this with 264 kB of SRAM and a CYW43439 chip which can act as a wireless access point. Students who took RBE 2020 should have a Pico W or Pico 2 W from their lab kit.

This repository contains the code for getting the Pico W to talk to the Romi over I2C and with a laptop wirelessly. ChatGPT helped with a few things in [this conversation](https://chatgpt.com/share/691a2d0b-a9e4-8004-9970-d018b923fe1d), but there are some details it did not know about which I figured out through many Internet searches and trial and error.

## Building the code

This code uses the [pico-sdk](https://github.com/raspberrypi/pico-sdk) for interfacing with the Pico W hardware using C. You can install this locally, but I've also set up a [Docker environment](.devcontainer/Dockerfile) which should help getting everything configured consistently. If you want to use that, install [Docker](https://www.docker.com/), [VSCode](https://code.visualstudio.com/), and the [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) VSCode extension. Open this project in VSCode, and run the `Dev Containers: Reopen in Container` command. It may take a while to build the container the first time, but subsequent launches will use the pre-existing container.

Each project can be built running the [`build.sh`](build.sh) script while inside the corresponding directory. For example: `cd wireless-test` `../build.sh`. If successful, a `.u2f` file will be created inside a new `build` directory which can be uploaded to the Pico W.

## Projects

### wireless-test

I used this project to figure out how to get wireless communication between my laptop and the Pico W. Heavily based on the [access point](https://github.com/raspberrypi/pico-examples/blob/master/pico_w/wifi/access_point/picow_access_point.c) example project.

This project enables Access Point mode on the Pico W, which allows other devices to connect to it like any other WiFi network. The Pico W could also be a client and connect to the same network as your computer. However, I assumed it would be hard to get the Pico W connected to school WiFi and figure out its IP address.

The Pico W will run a DHCP server (copied from the access point example) and a TCP server (taken from ChatGPT). The DHCP server assigns ip addresses to clients, which allows them to actually communicate on the network. The TCP server listens for incoming connections on port `1234` and echos back any messages sent to it.

To communicate with the Pico W, connect to its WiFi network (currently ssid KROBOTH_PicoW_AP, password raspberry). The Pico's ip address is `192.168.4.1`, and the TCP server is accessible on port `1234`. On Linux, I connect to the TCP server using the command `nc 192.168.4.1 1234`. You can type a message, press enter, and the Pico W will echo the message back to you.

### i2c-test

I used this project to figure out how to get the Pico W to talk to the Romi over I2C.

This uses the wireless project as a serial terminal to print out some messages. This is partly because I wanted it to work like that in the final project, and partly because I didn't realize that I could open a serial terminal to both the Pico W and Romi at the same time. Regardless, the Pico W only starts working as an I2C receiver once a wireless connection is established, which it uses to send debug messages about what I2C messages it is sending and receiving. The onboard LED will light up when once the access point is enabled, and will flash once a connection has been established.

The Pico W will listen at I2C address 10 on GPIO pins 17 (SCL) and 16 (SDA). Bytes written to the Pico W will be interpreted as strings and printed on the wireless console. When reading from the PicoW, it will send the bytes for `"Hello #%d from Pico"`, where `%d` increments each time.

### garbage-report

This is the actual assignment that I was working towards. The Romi needs to weigh a container of garbage and send out a bill wirelessly. Of course, this is implemented via the Pico W.

The Romi should send the following data over I2C about where on the grid the garbage came from and how much it weighed:

```c
typedef struct {
    int8_t i : 8;
    int8_t j : 8;
    int padding : 16; // Make sure following float is 32-bit aligned
    float weight;
} GarbageData;
```

The Pico W will send an appropriate message over the TCP connection reporting this information.

## Future work

The code for handling the TCP connection doesn't manage when the client disconnects. You just kinda have to restart it by un-powering and re-powering the Pico W. The access point example project also had a better user interface than just a netcat terminal. It automatically forwards you to a webpage where you can click a button to toggle the LED. You could also try running the Pico W as a client so that your laptop can still be connected to the Internet while communicating with the Pico W.

We're currently using an IR remote to control the Romi. This is annoying because it has a limited amount of buttons, and there is no feedback when the button presses don't get registered unless you have the USB plugged in and the serial monitor open. You could create a system to control the Romi wirelessly and log messages.

I wanted to implement a more sophisticated pathfinding algorithm on my Romi, but the code I came up with used too much RAM. The Pico W has more RAM than the Romi, so you could offload this path planning work to the Pico.
