# kSDR

### kSDR is now available on **Google Play** too!  

Try **kSDR** on your Android device — phone, tablet, Chromebook, or even an Android TV box!  
All you need is an SDR receiver and your Android device — and you can enjoy your radio hobby anywhere: at home, outdoors, or on the go.  

📥 [Download on Google Play](https://play.google.com/store/apps/details?id=ru.kslabs.ksdr) <br>
📥 [Download from our website](https://kslabs.ru/ksdr)  

## Why kSDR?  

We analyzed existing SDR apps on Google Play and noticed some common issues:  
- ❌ Clunky and inconvenient interfaces  
- ❌ Poor data processing speed  
- ❌ Lack of gesture support  
- ❌ Sluggish spectrum and waterfall performance  

For a long time, this gave the impression that SDR apps on Android simply couldn’t be smooth or user-friendly.  
With **kSDR**, we set out to prove otherwise!  

## What makes kSDR different?  

✔️ Frequency control and spectrum navigation with **simple gestures** — no need to dig through menus  
✔️ Intuitive zooming and frequency shifting  
✔️ Spectrum, waterfall, and UI elements rendered directly via **OpenGL** for smooth performance  
✔️ Minimal setup — works out of the box  

**Minimum requirement:** Android 7.0 or higher (OpenGL is supported by default on all devices).  

## How to start  

1. Install the app.  
2. Connect your SDR device.  
3. Start receiving signals!  

No complicated setup. No extra software.  

## Supported devices  

- **HackRF**  
- **RTL-SDR**  
- **MSI2500 (RSP1)**  

> ⚠️ When using HackRF, kSDR can both **receive and transmit** radio signals.  
> Always make sure an antenna is connected before transmitting — otherwise, you may damage your HackRF.  

## Roadmap  

We are continuously improving **kSDR**, fixing bugs, and expanding the list of supported SDR hardware.  

👉 Try it today — and experience a new level of receiving radio-signals on Android!  

---
<br><br>
### Simple Fast SDR receiver app with OpenGL support

Try our app for PC with Windows. It's absolutely free. Fast addiction guaranteed!!)

Demo video: <br>

https://youtu.be/b15e3qSgFJQ

![Screen](https://github.com/DkCocto/kSDR/assets/5113949/2a7d563a-fbae-4bba-81cc-0fbef30cfbe4)

I would like to present to your attention the kSDR application for working with popular SDR devices.

The application uses OpenGL to render the user interface. This allows you to get a pleasant experience of interacting with the controls of the program, as well as a smooth output of the spectrum and waterfall.

Unfortunately, this fact imposes some limitations. In particular, the computer must contain a video card capable of running OpenGL.

The program is in beta. At the moment, kSDR allows you to work with devices:

1) rsp1/rsp1a (supported api 3)
2) hackrf
3) rtlsdr

The application is currently configured to receive signals in the range from 1 to 30 MHz. Supported modulations: AM, USB, LSB, nFM.

Work on the application is in full swing. The source codes are completely open.

<b>Important</b><br>
------

When you launch the app for the first time, you will most likely need to go to the settings and select your device from the list. It is also necessary to correctly select the sampling rate and the Fourier transform length (FFT length). This will make the spectrum more informative.

Credits
---
I would like to receive user reviews or any feedback. This will push the progress of the application.

Please, if you are interested in this project or you have difficulty in launching the application for any reason, please contact us by e-mail:

dkcocto@gmail.com

Thank you!

Licensing
---

This program is distributed under The 2-Clause BSD License (https://opensource.org/license/BSD-2-Clause).

Copyright KSLABS<br>
2024<br>

https://www.kslabs.ru

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
<br><br>
