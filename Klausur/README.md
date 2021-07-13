# TCPIP-Programmierung SoSe 2021 – Klausuraufgabe - smb – Simple Message Broker

## Task 

This exam project focuses on developing simple message broker architecture. A `message` consists
the tuple `topic` (subject) and `message` (value, content), where
topic and message are to be implemented as simple `strings`. If the
hashtag `#` is used as topic, all messages are forwarded from the `broker` to the
`subscriber`. The broker does not store the messages but forwards a message only if the topic has been subscribed to by a subscriber. Subscriber has subscribed to it. The communication between broker and publisher or broker and subscriber is done via  `UDP`.

## Structure

![structure](structure.png)

### Broker

* mediator of news
* reads message from publisher
* manages message
* sends message to subscriber

### Publisher

* creator / sender of message
* generates data and sends message to Broker
* message consists of topic and data

### Subscriber

* consumer / receiver of message
* connects to Broker and subscribes to Topic
* reads message from broker for further processing

## Prepare the environment

### Compilation smbbroker.c

run this in bash before using:

```bash
gcc smbbroker.c -o smbbroker
sudo cp smbbroker /usr/local/bin
 ```

### Compilation smbpublish.c

run this in bash before using:

```bash
gcc smbpublish.c -o smbpublish
sudo cp smbpublish /usr/local/bin
 ```

### Compilation smbsubscribe.c

run this in bash before using:

```bash
gcc smbsubscribe.c -o smbsubscribe
sudo cp smbsubscribe /usr/local/bin
 ```

## Usage 

Broker should be always up:

Run this:

```bash
smbbroker
 ```

`Subscriber` should have one topic and it should be also up.

Run this in another terminal to `subscribe lamp1`:

```bash
smbsubscribe localhost lamp1
 ```

Note that, you can have multiple subscribers.

Run this in another terminal to `subscribe lamp2`:

```bash
smbsubscribe localhost lamp2
 ```

`Subscriber` can also have `wildcard`. It means it receives all messages from `Broker`.

Run this in new terminal:

```bash
smbsubscribe localhost "#"
 ```

When subscribers are set, it is expected to have 5 terminal as shown below.

![subs](subscribersAreSet.png)

Now `Publisher` can send messages to `Broker`. Let's say we want to turn on lamp1

Run this in another terminal

```bash
smbpublish localhost lamp1 on
 ```

![structure](lamp1.png)

As you can see from the result, message is delivered via `Broker` only to `lamp1 Subscriber` and `wildcard Subscriber`.

Let's now turn off lamp2. Run this in `publisher terminal`:

```bash
smbpublish localhost lamp2 off
 ```

![structure](lamp2.png)

As you can see from the result, message is delivered via `Broker` only to `lamp2 Subscriber` and `wildcard Subscriber`.

Let's try now to turn off lamp3 which does not exist. Run this in `publisher terminal`:

```bash
smbpublish localhost lamp3 off
 ```

![structure](noSubs.png)

As you can see from the result, message is delivered via `Broker` only to `wildcard Subscriber`.

## Broker: smbbroker.c

## Publisher smbpublish.c

## Subscriber: smbsubscribe.c
