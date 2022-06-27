# NETSGPClient
Arduino Interface for cheap 2.4ghz RF enabled Solar Micro Inverters using the so-called NETSGP protocol for communication.

Here is a YouTube video that shows the general function
https://youtu.be/uA2eMhF7RCY
[![YoutubeVideo](https://img.youtube.com/vi/uA2eMhF7RCY/0.jpg)](https://www.youtube.com/watch?v=uA2eMhF7RCY)

## Examples
The `PollDemo` shows how to request a status from the Micro Inverter synchronously.
Input your inverter id to get status updates.

The `AsyncDemo` shows how to get a status from multiple Micro Inverters asynchronously.
Input one or more inverter identifiers to get status updates inside the callback.


## Supported Devices
You can find an overview of all devices and their datasheets [here](http://newenergytek.com/)

| Model          | Tested             | Compatible         | Notes       | ID starting with |
|:---------------|--------------------|--------------------|-------------|------------------|
| `SG200LS`      | :x:                | :grey_question:    | 200W model  | :grey_question:  |
| `SG200MS`      | :white_check_mark: | :white_check_mark: | 200W model  | 11000001         |
| `SG200HS`      | :x:                | :grey_question:    | 200W model  | :grey_question:  |
| `SG200TS`      | :x:                | :grey_question:    | 200W model  | :grey_question:  |
| `SG250LS`      | :x:                | :grey_question:    | 250W model  | :grey_question:  |
| `SG250MS`      | :x:                | :grey_question:    | 250W model  | :grey_question:  |
| `SG250HS`      | :x:                | :grey_question:    | 250W model  | :grey_question:  |
| `SG250TS`      | :x:                | :grey_question:    | 250W model  | :grey_question:  |
| `SG300LS`      | :x:                | :grey_question:    | 300W model  | :grey_question:  |
| `SG300MS`      | :x:                | :grey_question:    | 300W model  | 19000001         |
| `SG300HS`      | :x:                | :grey_question:    | 300W model  | :grey_question:  |
| `SG300TS`      | :x:                | :grey_question:    | 300W model  | :grey_question:  |
| `SG350LS`      | :x:                | :grey_question:    | 350W model  | :grey_question:  |
| `SG350MS`      | :x:                | :grey_question:    | 350W model  | :grey_question:  |
| `SG350HS`      | :x:                | :grey_question:    | 350W model  | :grey_question:  |
| `SG350TS`      | :x:                | :grey_question:    | 350W model  | :grey_question:  |
| `SG400MS`      | :x:                | :grey_question:    | 400W model  | :grey_question:  |
| `SG400HS`      | :x:                | :grey_question:    | 400W model  | :grey_question:  |
| `SG400MD`      | :x:                | :grey_question:    | 400W model  | :grey_question:  |
| `SG400TD`      | :x:                | :grey_question:    | 400W model  | :grey_question:  |
| `SG450MS`      | :x:                | :grey_question:    | 450W model  | :grey_question:  |
| `SG450HS`      | :x:                | :grey_question:    | 450W model  | :grey_question:  |
| `SG450MD`      | :x:                | :grey_question:    | 450W model  | :grey_question:  |
| `SG450TD`      | :x:                | :grey_question:    | 450W model  | :grey_question:  |
| `SG500MS`      | :x:                | :grey_question:    | 500W model  | :grey_question:  |
| `SG500HS`      | :x:                | :grey_question:    | 500W model  | :grey_question:  |
| `SG500MD`      | :white_check_mark: | :white_check_mark: | 500W model  | 34000001         |
| `SG500TD`      | :x:                | :grey_question:    | 500W model  | :grey_question:  |
| `SG600MD`      | :x:                | :grey_question:    | 600W model  | 38000001         |
| `SG600HD`      | :x:                | :grey_question:    | 600W model  | :grey_question:  |
| `SG600TD`      | :x:                | :grey_question:    | 600W model  | :grey_question:  |
| `SG700MD`      | :x:                | :grey_question:    | 700W model  | 41000001         |
| `SG700HD`      | :x:                | :grey_question:    | 700W model  | :grey_question:  |
| `SG700TD`      | :x:                | :grey_question:    | 700W model  | :grey_question:  |
| `SG800MD`      | :white_check_mark: | :white_check_mark: | 800W model  | 26000001 :grey_question:  |
| `SG1000MD`     | :x:                | :grey_question:    | 1.0kW model | :grey_question:  |
| `SG1000HD`     | :x:                | :grey_question:    | 1.0kW model | :grey_question:  |
| `SG1000MQ`     | :x:                | :grey_question:    | 1.0kW model | 48000001         |
| `SG1000TQ`     | :x:                | :grey_question:    | 1.0kW model | :grey_question:  |
| `SG1200MT`     | :x:                | :grey_question:    | 1.2kW model | :grey_question:  |
| `SG1200HT`     | :x:                | :grey_question:    | 1.2kW model | :grey_question:  |
| `SG1200MQ`     | :x:                | :grey_question:    | 1.2kW model | 52000001         |
| `SG1400MQ`     | :x:                | :grey_question:    | 1.4kW model | :grey_question:  |
| `SG1400HQ`     | :x:                | :grey_question:    | 1.4kW model | :grey_question:  |

### Model number explanation
Model numbers consist of three parts. Prefix, Wattage and Suffix. The suffix itself consists of two capital letters having both different meanings. The first suffix letter specifies the maximum voltage capabilities of the inverter, the second letter the amount of input MC4 connector pairs. The wattage gets split across all inputs.

| Part                 | Value      | Meaning     |
|:---------------------|------------|-------------|
| Prefix               | SG         | Presumably stands for `Solar Grid`
| Wattage              | 200 - 1400 | Maximum power capability
| Suffix first letter  | L          | Low operating voltage 10V-30V, MPPT voltage 14V-24V
|                      | M          | Medium operating voltage 18V-50V, MPPT voltage 24V-40V
|                      | H          | High operating voltage 30V-70V, MPPT voltage 36V-60V
|                      | T          | Top? operating voltage 70V-120V, MPPT voltage 80V-115V
| Suffix second letter | S          | One MC4 connector pair (Single)
|                      | D          | Two MC4 connector pairs (Dual)
|                      | T          | Three MC4 connector pairs (Triple)
|                      | Q          | Four MC4 connector pairs (Quad)