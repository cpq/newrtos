This example demonstrates how to implement a JSON-RPC server over UART.
JSON-RPC frames are separated by new lines.

This firmware implements two RPC methods:
- `math.sum`. Expected params: array with two numbers. Returns sum of two numbers
- `gpio.write`. Expected params: object with `pin` and `val`. Sets GPIO pin

1. Build and flash the example
  ```sh
  $ make clean all flash
  ```
2. Use any terminal program to send JSON-RPC frames and get responses.
  Switch local echo on in order to see the input.
  The example below shows how we send commands to our board to sum two numbers,
  and to turn LED on. NOTE: LED pin in the command is 259, which corresponds
  to `PB3`:

  ```sh
  $ miniterm -e /dev/cu.usbmodem14202 57600

  {"id":1,"method":"rpc.list"}
  {"id":1,"result":["math.sum","gpio.write","rpc.list"]}

  {"id":2, "method":"math.sum", "params": [1.23,2.77]}
  {"id":2,"result":{"result":4}}

  {"id":3, "method":"gpio.write", "params": {"pin": 259, "val": 1}}
  {"id":3,"result":true}
  ```
