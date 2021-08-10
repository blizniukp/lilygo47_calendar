# lilygo47_calendar

*Read this in other language: [English](README.md), [Polski](README.pl.md).*

Application for displaying entries from the Google Calendar on the LILYGO 4,7" device.
![lilygo47_calendar](/img/img1.jpg)
![lilygo47_calendar](/img/img2.jpg)
![lilygo47_calendar](/img/img3.jpg)

# Hardware
The project uses the LILIGO EPD 4.7" device.
[http://www.lilygo.cn/](http://www.lilygo.cn/prod_view.aspx?TypeId=50031&Id=1384&FId=t3:50031:3)

# Description
The device periodically connects to wifi, and then downloads information about scheduled events (using Apps Scripts) from the Google Calendar. After downloading the data, they are displayed and the device itself goes to sleep.

The presented data consists of two main elements:
* Status bar,
* Information about events

The status bar contains information (from the left) about:
-- The number of events that were not displayed (did not fit on the display),
-- Date and time the information was retrieved,
-- Battery charge level and supply voltage.

# Installation
### Google Apps Script
#### Adding a script to Apps Script
In order to add a script that will download and prepare data from the calendar, go to [https://script.google.com](https://script.google.com).
Then select "New Project". In the window that appears, paste the code from the file `calendar_script.gc`.
In the code snippet:
```
  var _calendarName = '___CALENDAR_NAME___',
    _checkInRate = SIXTYDAYS;
```
you must replace the text `___ CALENDAR_NAME ___` with the name of your calendar.

If we have a family founded on Google [https://families.google.com/families](https://families.google.com/families) then we can share the calendar among all people in our family. If you need this functionality then replace `___ CALENDAR NAME ___` with `Family`. The finished code snippet should look like this:
```
  var _calendarName = 'Family',
    _checkInRate = SIXTYDAYS;
```

#### Deploy script
When the script is ready, select `Deploy-> New Deployment` in the upper right corner of the page. In the window that will appear, select the type as `Internet application`. Enter any name in the `Description` (or leave it blank).
In `Execute as`, choose` Me`, and in `Who has access`, set` Anyone with a Google account`.

After the deploy, a window appears from which we copy the `Implementation ID` - we will need it later.

### Preparation of the device

I used Visual Studio Code (version 1.57.1) with PlatformIO IDE plugin (version 2.3.2)

The configuration on the device side is limited to the preparation of the configuration file. A sample configuration file is provided in the `include / config.h_example` file. You only need to rename it to `config.h` and complete the WiFi (ssid and password) section and Google Apps script id (Implementation ID from previous stage).

```
#define WIFI_SSID             "Your WiFi SSID"
#define WIFI_PASSWORD         "Your PASSWORD"

#define SCRIPT_ID             "YOUR_GOOGLE_SCRIPT_ID"
```

### Event priority
Each entry in the calendar can be marked with the appropriate color - I use this as a priority of the event. The higher the priority, the darker the dot is displayed before to the entry.

| Name             | Value from AppScript (int)| Value in EPD (nazwa, hex) |
|------------------|---------------------------|-----------------------------|
| Tomato           | 11                        | LightGrey (0xBB)            |
| Tangerine        | 6                         | LightGrey (0xBB)            |
| Sage             | 2                         | LightGrey (0xBB)            |
| Peacock          | 7                         | Grey (0x88)                 |
| Lavender         | 1                         | Grey (0x88)                 |
| Graphite         | 8                         | Grey (0x88)                 |
| Flamingo         | 4                         | DarkGrey (0x44)             |
| Banana           | 5                         | DarkGrey (0x44)             |
| Basil            | 10                        | DarkGrey (0x44)             |
| Blueberry        | 9                         | Black (0x00)                |
| Grape            | 3                         | Black (0x00)                |
| Default          |                           | Grey (0x88)                 |


# Housing
I used the housing made by [3DMath](https://www.thingiverse.com/3dmath/designs)
Link to the housing: https://www.thingiverse.com/thing:4908721

# License
MIT