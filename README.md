# Fiat 500e CAN bus notes

### Message decoding
You can find my living document [here](https://docs.google.com/spreadsheets/d/12NPhM8Nom-K4TzoWUFvXqUXVt-HyTaEw7aM3jxspBx4/) which includes documentation many passive and queried data points. Some are probably wrong, please verify if you are able. Included in the document are messages for remote lock, unlock, and preconditioning which seem to be of most interest to people.

### How to
There are dozens of ways to use or discover data like this and which you choose will be based on your end goals. You'll need both hardware and custom software to make most of this work.

##### Hardware:
- Raspberry Pi is an all round winner excelling in deciphering, heavy processing, and built in connectivity
- Arduino based boards are perfect for simple end product hardware
- Pupose built devices from PCAN or CSS Electronics do one thing really well - log
- OBDII dongles lack throughput of all other devices but excell with mobile devices if/when someone makes software for 500e

##### Software:
- (Library) can-utils is an excellent place to start on Linux or Raspberry Pi hosts, but needs to be recompiled from source to work on 500e
- (Library) python-can on Raspberry Pi works great with 500e and every device I have access to
- (Library) FlexCAN for Teensy
- (GUI) SavvyCAN is crossplatform and crashes a lot
- (CLI) minicom for Linux or Mac to work with ELM327 devices
- (Mobile) AlfaOBD is great for extracting data on Android using and ELM327
- (Mobile) Multiecuscan on iOS is better yet and provides a clean and easy to understand UI
- (Mobile) Other off the shelf ELM terminals and OBD apps are not worth your time as they do not work or cause trouble

### Off the shelf solutions
Not a ton of options here. I will caution plugging anything into the OBDII port. In my experience it has caused temporary errors, but at least one report of costly damage exists.
- my500e is a simple purpose built device for those who just want to buy a solution. It's limited in scope, but successfully replaces UConnect.
- AutoPi.io is not a great option for 500e (or most other vehicles IMO.) At a hardware level they only support one of our CAN busses, with most interesting modules out of reach. They also do not support extended frames at a software level making this a complete non-starter.
