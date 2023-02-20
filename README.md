# kSDR
Simple Fast SDR receiver app with OpenGL support

Demo video: <br>

https://www.youtube.com/watch?v=dhKHVnkqCZA

![2023-02-01_13-18-52](https://user-images.githubusercontent.com/5113949/216016600-60bd6e26-5792-4ebb-9cea-20fed224fe0d.png)

I would like to present to your attention the kSDR application for working with popular SDR devices.

The application uses OpenGL to render the user interface. This allows you to get a pleasant experience of interacting with the controls of the program, as well as a smooth output of the spectrum and waterfall.

Unfortunately, this fact imposes some limitations. In particular, the computer must contain a video card capable of running OpenGL.

The program is in early beta. At the moment, kSDR allows you to work with devices:
1) rsp1/rsp1a supported api 2 and 3
2) hackrf
3) rtlsdr (not yet fully supported)

The application is currently configured to receive signals in the range from 1 to 30 MHz. Supported modulations: AM, USB, LSB.

Work on the application is in full swing. The source codes are completely open.

<b>Important</b><br>
------

Before using the application, you need to open the settings file: "config.xml" and set the type of your device in accordance with the instructions given in this file.

Example: 
```xml
Device codes
0: RSP1/RSP1A
1: hackrf

<type>1</type>
```

If you have an sdr device rsp1/rsp1a or their clones, then in order to run our application you need to configure "config.xml" file and setup a correct 
api version. Default will be api 3.

Credits
---
I would like to receive user reviews or any feedback. This will push the progress of the application.

Please, if you are interested in this project or you have difficulty in launching the application for any reason, please contact us by mail:

dkcocto@gmail.com

Thank you!
