# AirQuality
AirQuality sensor based on Wemos / BME280 / MQ135 for Jeedom

Lib to be added in the sketch:
https://github.com/GeorgK/MQ135

Calibration:
- Power the system for 48H
- Disconnect and put outside for 30min
- reconnect and launch sketch in monitoring to immediatly record RZERO
- Update RZERO value un MQ135.h , ligne 27

For information "good fresh "air is around 400ppm


