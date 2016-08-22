# mywface

Basic Watchface for Pebble Time. Displays
Time, date with weekday, Bluetooth status,
steps (needs Pebble Health) and battery
status.

This code can be the basis for designing
more complex watchfaces.

## About battery compsumption

Each indicator of the watchface is suscribed
to an appropiate event to update only when
necessary. Time updates once a minute. Date
updates once a day. Step updates once a minute.
Battery and Bluetooth status only updates on 
change.
