package com.example.matti.nonokontroler;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;

import java.io.DataOutputStream;
import java.io.IOException;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity implements SensorEventListener {

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

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        /* OBSŁUGA AKCELEROMETRU */

        SensorManager SM = (SensorManager) getSystemService(SENSOR_SERVICE);
        Sensor mySensor = SM.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        SM.registerListener(this, mySensor, SensorManager.SENSOR_DELAY_NORMAL);

        /* OBSŁUGA BLUETOOTH */

        if (mBluetoothAdapter != null) {
            // Urządzenie wspiera Bluetooth

            TextView t = (TextView)findViewById(R.id.log);
            if (t != null) {
                // Istnieje na formie kontrolka wyswietlajaca log

                if(wlaczBluetooth()){
                    // Udało się włączyć moduł bluetooth

                    t.setText("Próbuję odnaleźć Nono");
                    Nono = znajdzNono();

                    if(Nono != null && polaczZNono())
                    {
                        // Znaleziono Nono na liscie sparowanych urzadzen i udala sie proba nawiazania z nim polaczenia
                        t.setText("Połączono się z Nono");
                    }

                } else t.setText("Nie udało się włączyć bluetooth");
            }
        }
    }

    public void przeslijDaneSterowania(float LP, float GD){

        /*
            LP: MIN -10 MAX 10 (LEWO/PRAWO) 0 - SRODEK
            GD: MIN -11 MAX 11 (PRZÓD/TYL) 0 - SRODEK
         */

        /* BUFOR 4 BAJTÓW DO PRZESŁANIA */
        byte[] Tab = new byte[4];

        /* PRZELICZANIE DANYCH */
        if(GD > 0)
        {
            GD = (GD/11)*255;
            Tab[1] = 'T';
            Tab[3] = 'T';
        }
        else if(GD < 0)
        {
            GD = (-GD/11)*255;
            Tab[1] = 'P';
            Tab[3] = 'P';
        }
        if(LP > 0)
        {
            LP = (LP/10)*255;

            Tab[0] = (byte)(LP<GD?(GD-GD*(LP/GD)):(LP-LP*(GD/LP)));
            Tab[2] = LP<GD?(byte)GD:(byte)LP; // <-- PRAWY SILNIK BĘDZIE SZYBSZY
        }
        else if(LP < 0)
        {
            LP = (-LP/10)*255;

            Tab[0] = LP<GD?(byte)GD:(byte)LP; // <-- LEWY SILNIK BĘDZIE SZYBSZY
            Tab[2] = (byte)(LP<GD?(GD-GD*(LP/GD)):(LP-LP*(GD/LP)));
        }

        /* WYSYLANIE DANYCH */
        try {

            outStream.write(Tab);
            outStream.flush();

        }catch (Exception ex){
            // ..
        }
    }

    /* EVENT WYKONUJE SIĘ PRZY KAŻDEJ NAWET NAJMNIEJSZEJ ZMIANIE POŁOŻENIA TELEFONU */
    @Override
    public void onSensorChanged(SensorEvent event) {

        /* WSP TOLERANCJI PRZECHYŁU POWYŻTJ KTÓREGO WARTOŚCI DANE STEROWANIA ZOSTANĄ PRZESŁANE */
        float wspToler = (float)2.0;

        if(Math.abs(event.values[0])>wspToler || Math.abs(event.values[1])>wspToler)
            przeslijDaneSterowania(event.values[0],event.values[1]);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        // nie używane ..
    }
}
