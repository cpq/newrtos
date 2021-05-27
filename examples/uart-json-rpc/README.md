This example demonstrates how to implement a JSON-RPC server over UART.
JSON-RPC frames are separated by new lines.

This firmware implements two RPC methods:
- `math.sum`. Expected params: array with two numbers. Returns a sum of two numbers
- `led`. Expected params: object with attr `on`. Sets LED on or off

Build and flash the example:

```sh
$ make clean all flash
```

Use any terminal program to send JSON-RPC frames and get responses.
In the example below, we use `miniterm` - but any other program would
too, like `cu`, `minicom`, or `putty` on Windows.

Start `miniterm` with local echo turned on (note that the exact
device name could be different on your machine):

```sh
$ miniterm -q -e /dev/cu.usbmodem14202 115200
```

Now `miniterm` waits for an input.
Copy a `{"id":1, "method":"rpc.list"}` string, which is a JSON-RPC
request, paste into `miniterm`, and press Enter to send a finishing
new line. We should see a response with a list of registered RPC methods:

```sh
{"id":1,"method":"rpc.list"}
{"id":1,"result":["math.sum","led","rpc.list"]}
```

Now let's call our function `sum`:

```sh
{"id":2, "method":"math.sum", "params": [1.23,2.73]}
{"id":2,"result":{"result":3.96}}
```

Turn LED on:

```sh
{"id":3, "method":"led", "params": {"on": true}}
{"id":3,"result":{"ram":9724}}
```

The LED lits up! The response shows free RAM available - on your
board it could be different. Turn LED off:

```sh
{"id":3, "method":"led", "params": {"on": false}}
{"id":3,"result":{"ram":9724}}
```

That's it! Congratulations, now you could register more functions
on your board and call them via UART.

Hint: if you wire an ESP32 to your board in a way described at
https://vcon.io, you could call your JSON-RPC functions remotely
via the https://dash.vcon.io cloud.
