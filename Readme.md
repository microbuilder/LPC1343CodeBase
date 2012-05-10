# LPC1343 Code Base

## Overview

This software library is based on the freely available LPC1343 Reference
Design from http://www.microBuilder.eu, and is based around the **ARM Cortex M3
LPC1343** from NXP.  

The LPC1343 is a modern, entry level 32-bit ARM Cortex M3 processor with
**32KB flash, 8KB SRAM**, and can be run at up to **72MHz**.  It also includes
full-speed USB 2.0, along with all the main peripherals you'd expect to find 
(I2C, SPI, PWM, ADC, UART, etc.).

The LPC1000 family from NXP contains a wide variety of ARM Cortex M0 and
Cortex M3 cores, but the LPC1343 is particularly attractive in situations
where price is an issue, but a decent amount of performance is needed.  It has
an efficient instruction set (most commands execute in a single clock cycle,
including single-cycle multiply, etc.), and excellent code-density thanks to
the Thumb-2 instruction set, meaning 32KB stretches a lot further than it 
did with the older ARM7 or ARM9 cores even in Thumb mode.

The ARM Cortex M0 based LPC1114 is cheaper (ridiculously cheap!), and represents
one of the best values out there when both price and power consumption are an issue,
but the Cortex M0 has a minor performance bottleneck compared to the Cortex M3 that 
may or may not be important to you:  The smaller, lighter-weight M0 LPC1114 is based
on a Von Nuemann architecture with a single pipeline for both instructions and
data. This means that it usually takes two clock cycles to do something on the M0 that can
be done in one clock cycle on the ARM Cortex M3 LPC1343.  The M3 LPC1343 implements
a Harvard architecture with a three-stage pipeline, meaning seperate pipelines for
instructions, data and peripherals that can all be filled in the same clock cycle.

This means that in certain situations 72MHz on the M3 LPC1343 can 
actually be more than twice as fast as the 50MHz LPC1114, though this comes at the
expense of a larger and more complicated core that consumes more power and requires a
slightly larger die due to the increased gate count.

> **M0 Sidenote:** The single-pipeline bottleneck on the M0 is addressed in the recently announced
ARM Cortex M0+, which will contain a dual pipeline, while still maintaining a very
light-weight, efficient and inexpensive core.  This is probably the most important
change in the M0+ since it offers much better performance per clock cycle, meaning
you can switch back to a low-power sleep mode that much quicker for better battery life.

If you need to get the best possible performance for your money and are doing
computationally intensive tasks or maybe a lot of rapid memory access and pin state
reads/writes, etc., the LPC1343 will be a better fit.  If you need to get the lowest
possible cost and power consumption, the LPC1114 is probably the better choice, though
the LPC1343 has reasonably good power numbers as well.

One of the biggest advantages of the LPC1343 and LPC1114, though, is that they are
99% register compatible with each other if you want or need to switch to one
or the other, and learning one makes using the other trivial.  While the LPC1343 Code
Base tends to be slightly more actively maintained, there is a parallel LPC1114 Code
Base that mirrors the LPC1343 library here, and it should be painless to switch between
the two.  For more information see: https://github.com/microbuilder/LPC1114CodeBase

## License

Unless otherwise stated, all files are generally licensed
under a modified **BSD license** (see license.txt for details).

## Downloads

The latest development version of the software is always available on github at:
>https://github.com/microbuilder/LPC1343CodeBase

## Development Status

While there are almost certainly bugs and chunks of decidedly
mediocre code in the library, after two years or so of fairly
steady development effort, the core code should be fairly
reliable.  Some drivers in the /drivers folder are incomplete,
and may have been abandoned once the basic features were working
(setting pixels on a display, etc.), but they should serve as
a good starting point for a wide variety of peripherals.

The code base is definately imperfect, and some growing pains and faulty early
assumptions are evident throughout, but it hopefully at least
partially fulfills the goal of providing a genuinely open source
code base for the LPC1343 and GCC with no dependencies on any one
commercial toolchain or IDE.

## Supported Toolchains/IDEs

The entire LPC1343 Code Base is based around **GCC**, and can be built
from the command-line using only free tools like the **Yagarto**
distribution for Windows or the Mac, or using GCC on Linux, etc.
> http://www.yagarto.de/

Nonetheless, it's sometimes more productive to develop in an IDE with a
full set of debugging tools at your disposal, and the code base also
includes project files for Rowley Associate's **Crossworks for ARM**, as
well as project files for the free and open source **CodeLite**, which can
be used in combination with a Segger J-Link for ARM and GDB Server.
Project files for these two IDEs are stored in the /build folder.

Project files for the free (but commercial) **LPCXpresso** IDE are stored in the
root folder, and this probably represents the cheapest means to do real step-through
HW debugging.  For more information on using LPCXpresso with the LPC1343 Code Base see:
>http://www.microbuilder.eu/Projects/LPC1343ReferenceDesign/DebuggingWithLPCXpresso.aspx

## Documentation

Online documentation is available at the following URLs, though the
current documentation reflects and earlier version of the library, and some
inconsistencies are present since parts of the library have been 
reorganized working towards a 1.0.0 release.  The graphics library in
particular has been reorganized quite a bit.

**General API Documentation:**
>http://www.microbuilder.eu/Projects/LPC1343ReferenceDesign/CodeBaseDocumentation.aspx

**TFT/OLED Graphics Sub-System:**
>http://www.microbuilder.eu/Projects/LPC1343ReferenceDesign/TFTLCDAPI_v1_1_0.aspx

**Other tutorials and links:**
>http://www.microbuilder.eu/Projects/LPC1343ReferenceDesign.aspx

## How Can Someone Support This Project?

A lot of the recent development in this library was done
thanks to the financial support of **Adafruit Industries** (paying
my salary to keep working on this stuff!).  If you find this library
useful, please consider ordering something from the people that
make this financially possible:  http://www.adafruit.com

Any additions, suggestions or bug fixes are always welcome, and this
is in fact one of the main incentives behind making this library
open source.  If you have anything to contribute, using the feedback
mechanisms on Github is probably the best and most orderly way to
do this.
