## Flashing ESP-32 from the command line

Checkout my YouTube video on how to flash from the command line:
https://www.youtube.com/watch?v=xEohqTJ2WvU&t=1s

### esp-idf commands

`idf.py create-project main_esp_idf`

`idf.py menuconfig`

input wifi credentials

`idf.py build`

`idf.py -p COM6 flash monitor`
