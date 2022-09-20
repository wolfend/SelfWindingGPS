# SelfWindingGPS
Arduino code that works with Adafruit GPS shield and libraries to receive GPS time, display it on a ST7735 128x128 TFT display, and generate a once per hour pulse at the top  of every hour on digital output pin that is used to synchronize a Self Winding Clock Company clock. The digital output switches a reed relay that operates didrectly from the output pin. The relay is used to switch the output of a small 3.3V power supply module. The reed relay I use has an internal snubber diode for its own coil's kickback but the contacts of the reed relay also need to be protected by a snubber diode from the sizable kickback current from the synch coil in the clock.
A digital input pin is used to read a switch's state to set the time zone that is display; UTC or MST.