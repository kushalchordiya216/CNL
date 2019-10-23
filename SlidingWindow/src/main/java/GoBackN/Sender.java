package GoBackN;

import java.net.*;
import java.io.*;
import java.util.ArrayList;
import java.util.concurrent.*;

class DataPacket implements Serializable {

    private static final long serialVersionUID = -2606496740371463169L;
    int frameno;
    double data;

    DataPacket(int i) {
        frameno = i;
        data = Math.random();
    }
}

public class Sender implements Runnable {
    private ArrayList<DataPacket> datapackets = new ArrayList<DataPacket>();
    private int index = 0, receivedAckIndex = 0, recievedNackIndex = 0, windowStart = 0, windowLength, windowEnd;
    private Socket socket;

    private Sender(int windowLength) {
        this.windowLength = windowLength;
        this.windowEnd = windowStart + windowLength;
    }

    private void dataInit() {
        if (!datapackets.isEmpty()) {
            datapackets.removeAll(datapackets);
        }
        for (int i = 0; i < 10; i++) {
            DataPacket dataPacket = new DataPacket(i);
            this.datapackets.add(dataPacket);
        }
    }

    private void send(Socket Client) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    ObjectOutputStream oos = new ObjectOutputStream(Client.getOutputStream());
                    while (true) {
                        while (windowEnd < index || index == datapackets.size()) {
                            try {
                                TimeUnit.SECONDS.sleep(5);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                        oos.writeObject(datapackets.get(index));
                        index++;
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    void displayFrame() {
        for (DataPacket d : this.datapackets) {
            System.out.println(d.frameno + "|" + d.data);
        }
    }

    private void processAck(Socket Client) {
        new Thread(new Runnable() {
            public void run() {
                System.out.println("process Ack started");
                ObjectInputStream ois;
                try {
                    ois = new ObjectInputStream(Client.getInputStream());
                    while (true) {
                        Ack ack;
                        ack = (Ack) ois.readObject();
                        assert ack != null;
                        if (!ack.status) {
                            recievedNackIndex = ack.frameNo;
                            index = recievedNackIndex;
                            System.out.println("Nack received. Going back to frame" + receivedAckIndex);
                            slideWindow(recievedNackIndex);
                        } else {
                            receivedAckIndex = ack.frameNo;
                            System.out.println("Ack received for frame" + receivedAckIndex);
                            slideWindow(receivedAckIndex);
                            if (receivedAckIndex == datapackets.size() - 1) {
                                System.out.println("One frame sent:");
                                index = 0;
                                slideWindow(index);
                                displayFrame();
                                dataInit();
                            }
                        }

                    }
                } catch (IOException e) {
                    System.out.println("I/O Error in receiving Ack!");
                } catch (ClassNotFoundException e) {
                    System.out.println("I/O Error in receiving Ack");
                } finally {
                    System.out.println("leaving process ack");
                }
            }
        }).start();
    }

    private void slideWindow(int start) {
        windowStart = start;
        if (windowEnd < datapackets.size()) {
            windowEnd = windowStart + windowLength;
        }
        System.out.println("Window frame is [" + windowStart + "," + windowEnd + "]");
    }

    public static void main(String[] args) {
        try {
            // if (args.length == 0) {
            // System.out.println("Provide port number for socket");
            // return;
            // }
            ServerSocket Server = new ServerSocket(8080);
            System.out.println("Listening for clients on " + Server.getInetAddress() + Server.getLocalPort());
            Socket Client = Server.accept();
            Sender sender = new Sender(4);
            sender.dataInit();
            sender.send(Client);
            sender.processAck(Client);
            Server.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run() {

    }

    public Socket getSocket() {
        return socket;
    }

    public void setSocket(Socket socket) {
        this.socket = socket;
    }
}
