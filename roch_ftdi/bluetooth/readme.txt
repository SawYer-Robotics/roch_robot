
Instructions for setting up a bluetooth serial connection on ubuntu.

> sudo apt-get install bluez bluez-tools
# Copy a rule which will make sure it comes up as roch
> sudo cp ../57-roch.rules /etc/udev/rules.d/
> sudo service bluetooth restart
> sudo service udev restart
# Plug in your bluetooth modules

# One shot
> sudo rfcomm bind /dev/rfcomm0 00:00:00:41:48:22
> sudo rfcomm release /dev/rfcomm0

# Permanent on startup
> sudo cp roch.conf /etc/bluetooth/roch.conf
> sudo service bluetooth restart
