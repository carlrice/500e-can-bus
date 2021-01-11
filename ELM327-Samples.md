# Set up
##### Requirements:
1. An ELM327 device
1. Some way to send commands over serial to it (like minicom)
1. Possibly a yellow adapter if you want to use CAN-B

##### About ELM327:
You access it over a serial connection (terminal or maybe streams on a mobile app.) AT instructions tell the adapter what to do, typically configuring the device or upcoming commands. You literally type `ATZ` then press `ENTER` key in the terminal to reset the device and see it's version. In some examples below, there is no `AT` at the start of the line - here you are just sending raw HEX to the bus. In my experience with 500e you need to always set a header BEFORE sending HEX data using something like `ATSHwwxxyyzz`.

# Examples
### Configure for CAN-B
```
ATZ
ATPB400A
ATSPB
```
Explanation:
1. Resets adapter to default
1. Configures the devices “User Protocol B” for 500e CAN-B
1. Switches to protocol we just configure

### Unlock (configure for CAN-B, yellow adapter)
```
ATSH0C41401F
74
```
Explanation:
1. Sets the header for remote lock/unlock
1. Unlock command
