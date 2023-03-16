# kSDR
Simple Fast SDR receiver app with OpenGL support

Demo video: <br>

https://youtu.be/b15e3qSgFJQ

![screen](https://user-images.githubusercontent.com/5113949/221380701-bf05f874-b837-4bfe-bb12-778277fea342.jpg)

I would like to present to your attention the kSDR application for working with popular SDR devices.

The application uses OpenGL to render the user interface. This allows you to get a pleasant experience of interacting with the controls of the program, as well as a smooth output of the spectrum and waterfall.

Unfortunately, this fact imposes some limitations. In particular, the computer must contain a video card capable of running OpenGL.

The program is in early beta. At the moment, kSDR allows you to work with devices:
1) rsp1/rsp1a supported api 2 and 3
2) hackrf
3) rtlsdr

The application is currently configured to receive signals in the range from 1 to 30 MHz. Supported modulations: AM, USB, LSB.

Work on the application is in full swing. The source codes are completely open.

<b>Important</b><br>
------

Before using the application, you need to open settings and set the type of your device.

If you have an sdr device rsp1/rsp1a or their clones, then in order to run our application you need to setup a correct 
api version.

Credits
---
I would like to receive user reviews or any feedback. This will push the progress of the application.

Please, if you are interested in this project or you have difficulty in launching the application for any reason, please contact us by mail:

dkcocto@gmail.com

Thank you!
