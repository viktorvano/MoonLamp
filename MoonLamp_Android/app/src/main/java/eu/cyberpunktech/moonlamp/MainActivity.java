package eu.cyberpunktech.moonlamp;

import androidx.appcompat.app.AppCompatActivity;

import java.io.DataOutputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketException;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {
    private Switch aSwitchLight;
    private Switch aSwitchRotation;

    private final int timeout = 1000;
    private final String localIP = "192.168.1.37";
    private final int localPORT = 80;
    public static int exception = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        aSwitchLight = findViewById(R.id.switchLight);
        aSwitchRotation = findViewById(R.id.switchRotation);

        Activity activity = this;

        final Handler handler = new Handler();
        Thread feedThread = new Thread()
        {
            @Override
            public void run() {
                super.run();
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if(exception == 1)
                        {
                            exception = 0;
                            Toast.makeText(activity, "Connection problem.", Toast.LENGTH_SHORT).show();
                        }
                    }
                });
                handler.postDelayed(this, 100);
            }
        };
        handler.postDelayed(feedThread, 100);

        aSwitchLight.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(!aSwitchLight.isChecked() && !aSwitchRotation.isChecked())
                {
                    DataSender dataSender = new DataSender("LAMP:OFF,MOTOR:OFF\n");
                    dataSender.start();
                }
                else if(!aSwitchLight.isChecked() && aSwitchRotation.isChecked())
                {
                    DataSender dataSender = new DataSender("LAMP:OFF,MOTOR:ON\n");
                    dataSender.start();
                }
                else if(aSwitchLight.isChecked() && !aSwitchRotation.isChecked())
                {
                    DataSender dataSender = new DataSender("LAMP:ON,MOTOR:OFF\n");
                    dataSender.start();
                }
                else
                {
                    DataSender dataSender = new DataSender("LAMP:ON,MOTOR:ON\n");
                    dataSender.start();
                }
            }
        });

        aSwitchRotation.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(!aSwitchLight.isChecked() && !aSwitchRotation.isChecked())
                {
                    DataSender dataSender = new DataSender("LAMP:OFF,MOTOR:OFF\n");
                    dataSender.start();
                }
                else if(!aSwitchLight.isChecked() && aSwitchRotation.isChecked())
                {
                    DataSender dataSender = new DataSender("LAMP:OFF,MOTOR:ON\n");
                    dataSender.start();
                }
                else if(aSwitchLight.isChecked() && !aSwitchRotation.isChecked())
                {
                    DataSender dataSender = new DataSender("LAMP:ON,MOTOR:OFF\n");
                    dataSender.start();
                }
                else
                {
                    DataSender dataSender = new DataSender("LAMP:ON,MOTOR:ON\n");
                    dataSender.start();
                }
            }
        });
    }

    public class DataSender extends Thread
    {
        private String message;
        public DataSender(String message)
        {
            this.message = message;
        }

        @Override
        public void run() {
            super.run();
            sendDataToServer(message);
        }

        private void sendDataToServer(String message)
        {
            try
            {
                // need host and port, we want to connect to the ServerSocket at port 7777
                Socket socket = new Socket();
                socket.setSoTimeout(timeout);
                socket.connect(new InetSocketAddress(localIP, localPORT), timeout);
                System.out.println("Connected!");

                // get the output stream from the socket.
                OutputStream outputStream = socket.getOutputStream();
                // create a data output stream from the output stream so we can send data through it
                DataOutputStream dataOutputStream = new DataOutputStream(outputStream);

                System.out.println("Sending string to the ServerSocket");

                // write the message we want to send
                dataOutputStream.writeUTF(message);
                dataOutputStream.flush(); // send the message
                dataOutputStream.close(); // close the output stream when we're done.

                System.out.println("Closing socket.");
                socket.close();
            }
            catch (Exception e)
            {
                e.printStackTrace();
                exception = 1;
            }
        }
    }
}