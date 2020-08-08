## 1. Prerequisites

Having at least one can0 interface up and running and physically connected to the same or another Linux machine.

In this example, we'll consider that we have two CANable dongles on ttyACM0 and ttyACM1 connected together (CANH-CANH, CANL-CANL, GND-GND) with their 120 ohms termination enabled (TERM jumper).
See https://canable.io/

Uses can-utils:
```
sudo apt install can-utils
```


## 2. Setup

Setup CAN0:
```
sudo slcand -o -c -s8 /dev/ttyACM0 can0
sudo ifconfig can0 up
sudo ifconfig can0 txqueuelen 10000
```

Setup can1:
```
sudo slcand -o -c -s8 /dev/ttyACM1 can1
sudo ifconfig can1 up
sudo ifconfig can1 txqueuelen 10000
```

## 3. Pre-test

We want to verify that both interfaces work.

In one terminal run can-util candump on can1:
```
candump can1
```

In another:
```
cansend can0 123#DEADBEEF
```

Verify that can1 received the frame via candump:
```
can1  123   [4]  DE AD BE EF
```

And vice-versa:
```
candump can0
cansend can1 123#DEADBEEF
can0  123   [4]  DE AD BE EF
```

We can also check ifconfig can0 and ifconfig can1 for the number of data bytes sent and received, or eventual errors.
If this doesn't work, double check that you have 60 ohms between CANH and CANL when the system is OFF (i.e., two 120 ohms terminations on both sides of the nodes).

## 4. CanTester test

With Ref compiled, launch GDS and Ref in fprime/Ref:
```
fprime-gds -d .
```
#
In another terminal, run candump on can1:
```
candump can1
```

Via GDS, send canTester.SendCanFrame command.

Verify that can1 received through candump:
```
can1  05A5A5A5   [8]  DE AD C0 DE DE AD BE EF
```
#
From can1, send a frame with id 123 (standard frame format):
```
cansend can1 123#DEADBEEF
```

Verify in Ref.log:
```
[CAN] Receiving data
[CANTester] ID = 00000123
[CANTester] size = 4
[CANTester] data = de ad be ef  END
```
#
From can1, send a frame with id 122 (standard frame format):
```
cansend can1 122#DEADBEEF
```

Verify that no frames were received by check Ref.log because the mask and filter are expecting frames with ID 0xXXXXX123 (see Topology.cpp startSocketTask invocation).
#
From can1, send a frame with id 0bcde123 (extended frame format):
```
cansend can1 0BCDE123#DEADBEEF
```

Verify in Ref.log:
```
[CAN] Receiving data
[CANTester] ID = 8bcde123
[CANTester] size = 4
[CANTester] data = de ad be ef  END
```

Note that the received ID is 8bcde123 instead of 0bcde123.
This is due to the EFF (extended frame format) bit at bit 31 (see linux/include/can-utils/can.h) and can be masked in CanTester CanRecv_handler.


