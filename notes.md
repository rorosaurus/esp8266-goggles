Note: currently, the master is the nodeID with the lowest value

# Psuedo-code

````
every 30 seconds, scan // scan takes ~60ms, or 7 frames @ 120fps
if there is a new master OR we haven't updated in 10 minutes
	connect to master (network with lowest nodeID) // 1 second pause in LED animations
	transmit a dummy message
	accept response with updated sync info (millis to next change, currentPattern, hue)
	disconnect
animate LEDs
````

# Known NodeIDs

* Rory's Goggles: 4c306c
* Jenny's Backpack (Yellow Tape): 4c3e67
* Rory's Backpack: 6e283e
* Jenny's Goggles: 91c367


Slaves seem to get ~1 second ahead on hue after ~60 seconds
Maybe use a static pattern specifically for syncing each pattern cycle?
manually updating the hue value seems to sync much much better...

disable the pattern switch button during connected mode
add a pattern which rotates through all patterns?