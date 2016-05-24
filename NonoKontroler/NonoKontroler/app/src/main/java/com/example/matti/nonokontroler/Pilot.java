package com.example.matti.nonokontroler;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;

import java.io.DataOutputStream;
import java.io.IOException;
import java.util.Set;
import java.util.UUID;

public class Pilot extends AppCompatActivity implements SensorEventListener {

    /* NIEZBĘDNE DLA TRANSMISJI BLUETOOTH */

    private final static int REQUEST_ENABLE_BT = 1;
    private BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");
    private BluetoothDevice Nono = null;
    private BluetoothSocket btsocket = null;
    private DataOutputStream outStream = null;

    private boolean wlaczBluetooth(){

        boolean stan = true;

        if (mBluetoothAdapter != null) {
            // Jeżeli mamy możliwość obsługi bluetooth

            if (!mBluetoothAdapter.isEnabled()) {
                // Jezeli modul bluetooth bl wylaczony wlaczamy go

                try{
                    // Usiłujemy włączyć bluetooth
                    Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);

                }catch (Exception ex){
                    stan = false;
                }
            }

        } else stan = false;

        return stan;
    }
    private BluetoothDevice znajdzNono(){
        Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();
        if (pairedDevices.size() > 0) {
            // Znaleziono sparowane urzadzenia

            for (BluetoothDevice device : pairedDevices) {

                if(device.getAddress().equals("20:15:12:23:88:98")){
                    // Wsrod znalezionych sparowanych urzadzen jest nasz modul bluetooth HC-05

                    return mBluetoothAdapter.getRemoteDevice(device.getAddress());
                    // Zwrocono uchwyt do znalezionego Nono w celu pozniejszego polaczenia z nim
                }
            }
        }
        return null;
        // W przypadku braku dostepnosci Nono
    }
    public boolean polaczZNono(){

        boolean stan = false;

        if(btsocket != null && btsocket.isConnected()) stan = true;
        else{
            try{

                btsocket = Nono.createInsecureRfcommSocketToServiceRecord(MY_UUID);
                outStream = new DataOutputStream(btsocket.getOutputStream());
                btsocket.connect();

                if(btsocket.isConnected()){
                    // Udało się nawiązać połączenie z Nono
                    stan = true;

                }}catch(IOException e){
                // Nie udało nam się nawiązać połączenia z Nono
            }
        }
        return stan;
    }

    public void backBtn(View v)
    {
        Button button =(Button) v;
        startActivity(new Intent(getApplicationContext(), MainActivity.class));
    }

    public void startBtn(View vi)
    {
        /* OBSŁUGA AKCELEROMETRU */

        SensorManager SM = (SensorManager) getSystemService(SENSOR_SERVICE);
        Sensor mySensor = SM.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        SM.registerListener(this, mySensor, SensorManager.SENSOR_DELAY_NORMAL);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.pilot_nono);

        /* OBSŁUGA BLUETOOTH */

        if (mBluetoothAdapter != null) {
            // Urządzenie wspiera Bluetooth

            TextView t = (TextView)findViewById(R.id.log2);
            if (t != null) {
                // Istnieje na formie kontrolka wyswietlajaca log

                if(wlaczBluetooth()){
                    // Udało się włączyć moduł bluetooth

                    Nono = znajdzNono();
                    if(Nono != null && polaczZNono())
                    {
                        // Znaleziono Nono na liscie sparowanych urzadzen i udala sie proba nawiazania z nim polaczenia
                        t.setText("Połączono się z Nono");
                    }
                }
            }
        }
    }

    /* EVENT WYKONUJE SIĘ PRZY KAŻDEJ NAWET NAJMNIEJSZEJ ZMIANIE POŁOŻENIA TELEFONU */
    @Override
    public void onSensorChanged(SensorEvent event) {

        /* WYSYLANIE DANYCH */
        try {

            float OY = event.values[1], OX = event.values[0];

            // JEŚLI SMARTFON ZOSTANIE PRZECHYLONY W BOK O WIĘCEJ NIŻ 4 JEDNOSTKI - POJAZD ZACZNIE SKRĘCAĆ
            // W PRZECIWNYM WYPADKU JEŚLI SMARTFON ZOSTANIE PRZECHYLONY W OSI OY O 2 JEDNOSTKI - ROZPOCZNIE SIE JEGO JAZDA
            char RUCH = (OX < -4 ? 'd' : (OX > 4 ? 'a' : (OY < -2 ? 'w' : (OY > 2 ? 's' : 'r' ) ) ) );

            outStream.write(RUCH);
            outStream.flush();

        }catch (Exception ex){
            // ...
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        // nie używane ..
    }
}
