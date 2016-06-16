# ESP8266 example (with json response)
This is a esp8266 json-response with html GET-request example.

## First you need to set

- In `esp8266.ino`, you need to set your SSID, PASSWD.
- In `app.html`, you need to set esp8266's IP.

## How it works?

`app.html`  ---Sending a get request--> `esp8266.ino`
`app.html`  <--Return a json response-- `esp8266.ino`

If you sending a get request by `pin=1`,
and `esp8266.ino` will response a `{ pin:1 }`.

Or sending a get request by `pin=1&status=1`,
will response a `{ pin:1, status:100 }`.

## License

MIT