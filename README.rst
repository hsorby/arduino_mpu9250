OpenGL application for MPU-9250
===============================

Adapted from www.lucidarme.me OpenGL application for mpu-9250.
Detailed information can be found at http://www.lucidarme.me/?p=5057

This repository now contains just the GUI OpenGL application for visualising the movement measurements from an Arduino MPU9250.

The code has been modified to require the serial device to be specified via an environment variable.
The device name is configured by setting the environment variable **MPU9250_DEVICE_NAME**.
On macOS from a bash prompt this environment variable can be set like so::

  export MPU9250_DEVICE_NAME=/dev/cu.usbmodem14401

The default value for the device name is **/dev/null**.

The baud rate can also be set in a similar manner.
To set the baud rate for the serial communication use the environment variable **MPU9250_BAUD_RATE**.
The default value for this variable is **115200**.

The application expects the MPU9250 to output data in a single line in the following format::

  epoch accel_x accel_y accel_z gyro_x gyro_y gyro_z mag_x mag_y mag_z temperature

This code has not been tried or tested on anything other than macOS.
