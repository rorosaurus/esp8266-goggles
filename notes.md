the master is the nodeID with the highest value

all slaves will:
every 10 seconds, scan and connect to network with highest nodeID
transmit a dummy message, and get response with updated sync info
disconnect