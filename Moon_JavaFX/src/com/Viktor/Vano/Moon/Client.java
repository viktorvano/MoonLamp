package com.Viktor.Vano.Moon;

import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

public class Client {
    // initialize socket and input output streams
    private Socket socket            = null;
    private DataOutputStream out     = null;

    // constructor to put ip address and port
    public Client(String address, int port, String message)
    {
        // establish a connection
        try
        {
            socket = new Socket(address, port);
            System.out.println("Connected");

            // sends output to the socket
            out    = new DataOutputStream(socket.getOutputStream());
        }
        catch(UnknownHostException u)
        {
            System.out.println(u);
        }
        catch(IOException i)
        {
            System.out.println(i);
        }

        try
        {
            out.writeUTF(message);
        }
        catch(IOException i)
        {
            System.out.println(i);
        }


        // close the connection
        try
        {
            out.close();
            socket.close();
            System.out.println("Disconnected");
        }
        catch(IOException i)
        {
            System.out.println(i);
        }
    }
}
