the master is the nodeID with the lowest value

# Psuedo-code
every 30 seconds, scan // scan takes ~60ms, or 7 frames @ 120fps
if there is a new master OR we haven't updated in 10 minutes
	connect to master (network with lowest nodeID) // 1 second pause in LED animations
	transmit a dummy message
	accept response with updated sync info (millis to next change, currentPattern, hue)
	disconnect
animate LEDs

# Known NodeIDs
Rory's Goggles: 4c306c
Jenny's Backpack (Yellow Tape): 4c3e67
Rory's Backpack: 6e283e