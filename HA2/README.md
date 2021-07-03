# Hausaufgabe 2 - Very Lightweighted File Transfer Protocol#

[Aufgabe](https://github.com/gogamid/TCPIP/blob/main/HA2/aufgabe.txt)

 ## SERVER: vlftpd.c
 
This is server program. It accepts commands from client 
and sends result of that command back to client.

### Compilation

run this in bash before using:
```bash
gcc vlftpd.c -o vlftpd
sudo cp vlftpd /usr/local/bin
 ```
### Run:

```bash
vlftpd
 ```

## CLIENT:  vlftp.c

This is client program. It sends commands to the server and
waits for response.

## Commands awailable:

pwd - print current working directory

cd directory – change directory

dir [files|directory] – list directory

get remotefile [localfile] - get (copy) file from server to local host

put localfile [remotefile] – put (copy) file from local host to server

### Compilation:

run this in bash before using:
```bash
gcc vlftp.c -o vlftp
sudo cp vlftp /usr/local/bin
 ```
 ### Usage:
 
 ```bash
 vlftp server command [argument1 [argument2]]
 ```
### Examples:

  ```bash
vlftp localhost pwd
vlftp localhost dir /home
vlftp localhost cd ..
vlftp localhost get sicherheit.txt get.txt
vlftp localhost put sicherheit.txt put.txt
 ```
