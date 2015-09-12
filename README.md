# Arduino-Serial-over-Ethernet

IMPORTANT: I only included the client code for now, the server code is currently being ported completely to the Arduino

This code is meant to allow you to implement a way to remotely access your machines via serial, like a backdoor would like BASH over TCP using Netcat or Socat..

To use this script you will need an Arduino Mega with an Arduino Ethernet shield and a network connection to the other Arduino Mega with it's own Ethernet shield.

This is the initial release version, so there are bugs and some limitations.
I am working on making the stock firmware/code more friendly to conventional serial communication software.

So far this is compatible with GtkTerm(more clients will be tested later..)
