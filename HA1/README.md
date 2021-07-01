
 
 # Hausaufgabe 1 - srsmon - Simple Remote System Monitor
 ## SERVER: srsmond.c
 
This is server program. It accepts commands from client 
and sends result of that command back to client.
### Instruction
run this in bash before using:
```bash
gcc srsmond.c -o srsmond
sudo cp srsmond /usr/local/bin
 ```
### Usage:
```bash
srsmond
 ```

## CLIENT:  srsmon.c

This is client program. It sends commands to the server and
waits for response.
### INSTRUCTION:
run this in bash before using:
```bash
gcc srsmon.c -o srsmon
sudo cp srsmon /usr/local/bin
 ```
### Usage:
  ```bash
srsmon localhost users
 ```
  ```bash
srsmon localhost procs
 ```
  ```bash
srsmon localhost nets
 ```
  ```bash
srsmon localhost disks
 ```
