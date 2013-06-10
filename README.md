mqtt-remote
===========

Remote control of any application using MQTT protocol.


Prerequisites
-------------

 * [mosquitto.h](https://bitbucket.org/oojah/mosquitto/src/tip/lib/mosquitto.h?at=v1.1.2) in the same version as installed library
 * Installed MQTT server [Mosquitto](http://mosquitto.org/download/)


Instalation
-----------

On Raspberry Pi just run:

	make

On other linux distro (tested on Arch Linux):

	make -f Makefile.archlinux


Setup
-----


Currently supported applications
-------

 * mocp


Remotes
-------

 * gasia corp ps gamepad adaptor (_ID 054c:0268 Sony Corp. Batoh Device / PlayStation 3 Controller_)
 * keyboard
