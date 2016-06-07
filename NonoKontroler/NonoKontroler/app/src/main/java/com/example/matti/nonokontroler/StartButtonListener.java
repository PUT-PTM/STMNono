package com.example.matti.nonokontroler;

import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.view.View;
import android.widget.Button;

/**
 * Created by Daniel H on 2016-05-31.
 */
public class StartButtonListener implements View.OnClickListener {

    private Pilot pilot;

    private Button button;

    public StartButtonListener(Pilot pilot, Button button) {
        this.pilot = pilot;
        this.button = button;
    }

    @Override
    public void onClick(View v) {
        SensorManager SM = (SensorManager) pilot.getSystemService(Pilot.SENSOR_SERVICE);
        Sensor mySensor = SM.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        SM.registerListener(pilot, mySensor, SensorManager.SENSOR_DELAY_NORMAL);
    }
}
