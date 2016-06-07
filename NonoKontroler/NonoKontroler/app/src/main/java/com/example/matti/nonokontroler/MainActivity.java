package com.example.matti.nonokontroler;

import android.app.Fragment;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
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

public class MainActivity extends AppCompatActivity {

    /* NIEZBĘDNE DLA TRANSMISJI BLUETOOTH */

    public final static int REQUEST_ENABLE_BT = 1;
    public BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");
    public BluetoothDevice Nono = null;
    public BluetoothSocket btsocket = null;
    public DataOutputStream outStream = null;

    public boolean wlaczBluetooth(){

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
    public BluetoothDevice znajdzNono(){
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

    public void connectBtn(View v)
    {

                if (mBluetoothAdapter != null) {
                    // Urządzenie wspiera Bluetooth

                    TextView t = (TextView)findViewById(R.id.log);
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


    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

    }

    public void openPilot(View view) {
        //setContentView(R.layout.pilot_nono);
        Intent pilot = new Intent (this, Pilot.class);
        startActivity(pilot);
    }

    public void onSensorChanged(SensorEvent event){}
    public void startBtn(View vi)
    {}

    public static class PlaceholderFragment extends Fragment {

        public PlaceholderFragment() {
        }

        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                                 Bundle savedInstanceState) {
            View rootView = inflater.inflate(R.layout.activity_main, container, false);
            return rootView;
        }


    }

}
