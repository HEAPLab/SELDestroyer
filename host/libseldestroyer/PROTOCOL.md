Protocol description
====================

All the messages (in both directions) begin with the character `$`. If any
error occurs, each side must wait for the following `$` character.
All the commands are terminated with `\n` character.

Device -> Host
--------------
- `$P`: Pong
- `$V,SXXXXX`: voltage reading, XXXXX in mV
- `$I,SXXXXX`: current reading, XXXXX in mA
- `$N,+XXX`: number of SELs detected
- `$S`: SEL detected (XXXXX = max current in mA)
- `$CL,XXXXX`: current limiter set value, XXXXX in mA
- `$CH,XXXXX`: current limiter hold time value, XXXXX in 100-us (i.e., XXXXXX=1
  means 100us, XXXXX=20000 means 2 seconds. Max allowed value 4 seconds)
- `$CM,XXXXX`: average mode
- `$CO,X`: current output status: 0 - always OFF, 1 - always ON, A - auto
- `$E`: protocol error / overflow
- `$*`: done (any request)

Host -> Device
--------------
- `$P`: Ping
- `$A`: Ask for V/I instant readings
- `$N`: Ask for number of SELs
- `$R`: Reset number of SELs
- `$?`: Ask for current configs
- `$CL,XXXXX`: Configure limiter set value,  XXXXX in mA
- `$CH,XXXXX`: Configure limiter hold time value in 100-us (i.e., XXXXXX=1
  means 100us, XXXXXX=100000 means 10 seconds)
- `$CM,X`: Configure average mode of the INA233
- `$CO,X`:  Set output mode: 0 - always OFF, 1 - always ON, A - auto