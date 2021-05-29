# NETSGPClient
Arduino Interface for cheap 2.4ghz RF enabled Solar Micro Inverters using the so-called NETSGP protocol for communication.

Here is a YouTube video that shows the general function
https://youtu.be/uA2eMhF7RCY
[![YoutubeVideo](https://img.youtube.com/vi/uA2eMhF7RCY/0.jpg)](https://www.youtube.com/watch?v=uA2eMhF7RCY)

## Examples
The `PollDemo` shows how to request a status from the Micro Inverter synchronously.
Input your inverter id to get status updates.

The `AsyncDemo` shoows how to get a status from multiple Micro Inverters asynchronously.
Input one or more inverter identifiers to get status updates inside the callback


## Supported Devices
| Model          | Tested             | Compatible         | Notes       | ID starting with |
|:---------------|--------------------|--------------------|-------------|------------------|
| `SG200MS`      | :white_check_mark: | :white_check_mark: | 200W model  | 11000001         |
| `SG300MS`      | :x:                | :grey_question:    | 300W model  | 19000001         |
| `SG600MD`      | :x:                | :grey_question:    | 600W model  | 38000001         |
| `SG700MD`      | :x:                | :grey_question:    | 700W model  | 41000001         |
| `SG1000MQ`     | :x:                | :grey_question:    | 1.0kW model | 48000001         |
| `SG1200MQ`     | :x:                | :grey_question:    | 1.2kW model | 52000001         |
