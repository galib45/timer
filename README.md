# Timer
A simple timer application built with GTK3. The timer allows you to manage sessions with customizable durations.

## Features
- Start/Pause Timer: Press the `Space` key to start or pause the timer.
- Reset Timer: Press the `r` key to reset the timer.
- Adjust Session Duration:
  - `Up` -  Increase the session duration by 5 minutes.
  - `Down` - Decrease the session duration by 5 minutes.
  - `Right` - Increase the session duration by 1 minute.
  - `Left` - Decrease the session duration by 1 minute.
- Quit the App: Press the `q` key to quit the app. The last updated session duration is automatically stored in the "timer.config" file.

## Build Instructions
### Prerequisites
Make sure you have the following dependencies installed:
```
sudo apt update
sudo apt install libgtk-3-0
```
### Build Steps
```
git clone https://github.com/galib45/timer.git
cd timer
```
### Run the application:
```
./timer
```
## Configuration
The last updated session duration is stored in the "timer.config" file when quitting the app.
