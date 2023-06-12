package com.example.visualiseur_musicale;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;

import android.Manifest;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.pm.PackageManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;

import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {
    //V globale
    public static MediaPlayer MEDIA_PLAYER;
    public static MainActivity MAIN_ACTIVITY;




    private void lanceFragment(Fragment fragment) {
        FragmentManager fm = getSupportFragmentManager();
        FragmentTransaction ft = fm.beginTransaction();
        ft.replace(R.id.frame, fragment);
        ft.addToBackStack(null);
        ft.commit();

    }


    //Fragments
    Lecture_Playlist frag1;
    Palette_de_Couleur frag2;
    Bluetooth frag3;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);



        frag1 = new Lecture_Playlist();
        frag2 = new Palette_de_Couleur();
        frag3 = new Bluetooth();
        // récupération de l'ID
        Button buttonPage1 = (Button) findViewById(R.id.Page1);
        Button buttonPage2 = (Button) findViewById(R.id.Page2);
        Button buttonPage3 = (Button) findViewById(R.id.Page3);

//Page 1 et 2
        buttonPage1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                lanceFragment(frag1);
            }
        });
        buttonPage2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                lanceFragment(frag2);
            }
        });
        buttonPage3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {lanceFragment(frag3);}
        });

        MEDIA_PLAYER = MediaPlayer.create(this, R.raw.pokemon);



    }
}






