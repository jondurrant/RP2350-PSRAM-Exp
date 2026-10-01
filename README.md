# RP2350 and PSRAM
I've been experimenting with PSRAM on RP2350 both prior to SDK 2.3.0 (Launched mid 2026) and after that. Testing these out on three boards for my YouTube channel [DrJonEA](https://youtube.com/@drjonea]

## Boards
- [PIMORONI Pico 2 Plus](https://shop.pimoroni.com/products/pimoroni-pico-plus-2)
- [Waveshare RP2350 PIZero](https://www.waveshare.com/rp2350-pizero.htm?&aff_id=DrJonEA) With PSRAM added [APS6404L](https://www.aliexpress.com/item/1005008903070951.htm)
- [Waveshare RP2350 Touch 2.8C](https://www.waveshare.com/rp2350-touch-lcd-2.8c.htm?&aff_id=DrJonEA)


## Firmware

### Pico 2 Plus
Board files exist that include the PSRM configuration for this board which makes it easier to use.  I have three examples:

- psram-raw: In this example I setup the PSRAM but just use it as raw memory space, without telling the  linker about it
- psram-ld: In this example, which is pre SDK 2.3.0 I setup the Linker description manually. This means I can mark variables as to be locatred in PSRAM
- psram-sdk2.3.1: This makes use of the new hardware_psram library to really set the PSRAM up properly. Only in this example is the QSPI bus put into quad mode and driven at the full 133MHz that this chip will support (266MHz system clock divided by 2)

### RP2350-PIZero
This board from waveshare comes with a slot to add PSRAM onto. This means that the board configuration file does not define the PSRAM setup.

- psram-ld: This setups a linker description and tests out PSRAM. Only at SPI speeds
- psram-sdk2-3-1: This uses the new hardware-psram library to initialise and set the speed of the bus to 133MHz (266MHz system clock divided by 2). It does also require a linker description file though due to the PSRAM definitions not being present in the board description file.  It makes build a bit more awkward.
- psram-sdk2.3.1-board: This adds a local board description file for the board with the PSRAM definitions. Therefore this example does not need the linker description file. Much easier to work with.

## Cloning and Building
This is a Pico SDK based project in C++. Where libraries are included they are normally included as submodules. When cloning please recurse submodules. 

I have a blog that explains how to clone repos and build my projects on 
[drjonea.co.uk](https://drjonea.co.uk/2025/12/15/building-my-projects-from-repo/).