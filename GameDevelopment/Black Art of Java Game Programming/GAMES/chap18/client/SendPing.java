package client;

import common.Packet;

class SendPing extends Thread {
  Client client = null;

  public SendPing (Client cli) {
    client = cli;
  }

  public void run () {
    while (true) {
      client.output(Packet.CPPing());
      try {
	sleep(30000);
      } catch (InterruptedException e) {
      }
    }
  }
}
