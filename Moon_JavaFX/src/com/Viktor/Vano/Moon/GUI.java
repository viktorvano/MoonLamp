package com.Viktor.Vano.Moon;

import javafx.animation.Animation;
import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.image.Image;
import javafx.scene.layout.Pane;
import javafx.scene.text.Font;
import javafx.stage.Stage;
import javafx.util.Duration;

import java.util.Objects;

import static com.Viktor.Vano.Moon.FileManager.*;

public class GUI extends Application {
    private final String version = "20220129";
    private String moonIP = "";
    private int moonPort = 80, appPort = 7750;
    private final int width = 550;
    private final int height = 400;
    private Label labelIP, labelPort, labelMoonLamp, labelStatus, labelAppPort;
    private TextField textFieldIP, textFieldPort;
    private Button buttonLight, buttonRotation;
    private Pane pane;
    private boolean light = false, rotation = false;
    private Timeline timelineCheckServer;
    private MyServer myServer;

    public static void main(String[] args)
    {
        launch(args);
    }

    @Override
    public void start(Stage stage){
        pane = new Pane();

        Scene scene = new Scene(pane, width, height);

        stage.setTitle("Moon " + version);
        stage.setScene(scene);
        stage.show();
        stage.setMinWidth(stage.getWidth());
        stage.setMinHeight(stage.getHeight());
        stage.setMaxWidth(stage.getWidth());
        stage.setMaxHeight(stage.getHeight());
        stage.setResizable(false);

        createDirectoryIfNotExist("res");
        try{
            moonPort = Integer.parseInt(Objects.requireNonNull(readOrCreateFile("moonPort.dat")));
            appPort = Integer.parseInt(Objects.requireNonNull(readOrCreateFile("appPort.dat")));
        }catch (Exception e)
        {
            e.printStackTrace();
        }
        moonIP = readOrCreateFile("moonIP.dat");

        labelAppPort = new Label("App Server Port: " + appPort);
        labelAppPort.setFont(Font.font("Arial", 24));
        labelAppPort.setLayoutX(150);
        labelAppPort.setLayoutY(50);
        pane.getChildren().add(labelAppPort);

        labelStatus = new Label("Light: " + light + "\nRotation: " + rotation);
        labelStatus.setFont(Font.font("Arial", 24));
        labelStatus.setLayoutX(50);
        labelStatus.setLayoutY(150);
        pane.getChildren().add(labelStatus);

        labelMoonLamp = new Label("Moon Lamp");
        labelMoonLamp.setFont(Font.font("Arial", 24));
        labelMoonLamp.setLayoutX(50);
        labelMoonLamp.setLayoutY(250);
        pane.getChildren().add(labelMoonLamp);

        labelIP = new Label("IP");
        labelIP.setFont(Font.font("Arial", 24));
        labelIP.setLayoutX(50);
        labelIP.setLayoutY(300);
        pane.getChildren().add(labelIP);

        textFieldIP = new TextField();
        textFieldIP.setLayoutX(100);
        textFieldIP.setLayoutY(300);
        textFieldIP.setPrefWidth(120);
        textFieldIP.setText(moonIP);
        pane.getChildren().add(textFieldIP);
        textFieldIP.textProperty().addListener(observable -> {
            if(textFieldIP.getText().length() > 0)
                try{
                    String value = textFieldIP.getText();
                    writeToFile("moonIP.dat", value);
                    moonIP = value;
                }catch (Exception e)
                {
                    textFieldIP.setText("");
                }
        });

        labelPort = new Label("Port");
        labelPort.setFont(Font.font("Arial", 24));
        labelPort.setLayoutX(50);
        labelPort.setLayoutY(350);
        pane.getChildren().add(labelPort);
        labelPort.textProperty().addListener(observable -> {
            if(labelPort.getText().length() > 0)
                try{
                    int value = Integer.parseInt(labelPort.getText());
                    if(value >= 0)
                    {
                        writeToFile("moonPort.dat", String.valueOf(value));
                        moonPort = value;
                    }else
                        labelPort.setText("");
                }catch (Exception e)
                {
                    labelPort.setText("");
                }
        });

        textFieldPort = new TextField();
        textFieldPort.setLayoutX(100);
        textFieldPort.setLayoutY(350);
        textFieldPort.setPrefWidth(80);
        textFieldPort.setText(String.valueOf(moonPort));
        pane.getChildren().add(textFieldPort);

        try
        {
            Image icon = new Image(getClass().getResourceAsStream("moon.png"));
            stage.getIcons().add(icon);
            System.out.println("Icon loaded on the first attempt...");
        }catch(Exception e)
        {
            try
            {
                Image icon = new Image("moon.png");
                stage.getIcons().add(icon);
                System.out.println("Icon loaded on the second attempt...");
            }catch(Exception e1)
            {
                System.out.println("Icon failed to load...");
            }
        }

        buttonLight = new Button("Light");
        buttonLight.setLayoutX(300);
        buttonLight.setLayoutY(300);
        buttonLight.setPrefSize(80, 80);
        buttonLight.setOnAction(event -> {
            light = !light;
            labelStatus.setText("Light: " + light + "\nRotation: " + rotation);
            sendCommand();
        });
        pane.getChildren().add(buttonLight);

        buttonRotation = new Button("Rotation");
        buttonRotation.setLayoutX(400);
        buttonRotation.setLayoutY(300);
        buttonRotation.setPrefSize(80, 80);
        buttonRotation.setOnAction(event -> {
            rotation = !rotation;
            labelStatus.setText("Light: " + light + "\nRotation: " + rotation);
            sendCommand();
        });
        pane.getChildren().add(buttonRotation);

        myServer = new MyServer(appPort);
        myServer.start();

        timelineCheckServer = new Timeline(new KeyFrame(Duration.millis(100), event -> {
            if(myServer.isMessageAvailable())
            {
                timelineCheckServer.pause();
                String message = myServer.getMessage();
                if(message.contains("rotation") && message.contains("moon") && message.contains("on"))
                    rotation = true;
                if(message.contains("rotation") && message.contains("moon") && message.contains("off"))
                    rotation = false;
                if(message.contains("light") && message.contains("moon") && message.contains("on"))
                    light = true;
                if(message.contains("light") && message.contains("moon") && message.contains("off"))
                    light = false;

                labelStatus.setText("Light: " + light + "\nRotation: " + rotation);
                sendCommand();
                timelineCheckServer.play();
            }
        }));
        timelineCheckServer.setCycleCount(Animation.INDEFINITE);
        timelineCheckServer.play();
    }

    @Override
    public void stop() throws Exception {
        super.stop();
        timelineCheckServer.stop();
        myServer.stopServer();
    }

    private void sendCommand()
    {
        if(!light && !rotation)
            new Client(moonIP, moonPort, "LAMP:OFF,MOTOR:OFF\n");
        else if(!light && rotation)
            new Client(moonIP, moonPort, "LAMP:OFF,MOTOR:ON\n");
        else if(light && !rotation)
            new Client(moonIP, moonPort, "LAMP:ON,MOTOR:OFF\n");
        else
            new Client(moonIP, moonPort, "LAMP:ON,MOTOR:ON\n");
    }
}
