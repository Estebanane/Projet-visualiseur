package com.example.visualiseur_musicale;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;

import android.content.Intent;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;

import java.util.Set;

public class MainActivity extends AppCompatActivity {

    public static MediaPlayer MEDIA_PLAYER;

    private void lanceFragment(Fragment fragment) {
        FragmentManager fm = getSupportFragmentManager();
        FragmentTransaction ft = fm.beginTransaction();
        ft.replace(R.id.frame, fragment);
        ft.addToBackStack(null);
        ft.commit();

    }


    Lecture_Playlist frag1;
    Palette_de_Couleur frag2;





    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        frag1 = new Lecture_Playlist();
        frag2 = new Palette_de_Couleur();
        // récupération de l'ID
        Button buttonPage1 = (Button) findViewById(R.id.Page1);
        Button buttonPage2 = (Button) findViewById(R.id.Page2);

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

        MEDIA_PLAYER = MediaPlayer.create(this, R.raw.pokemon);

    }


}









