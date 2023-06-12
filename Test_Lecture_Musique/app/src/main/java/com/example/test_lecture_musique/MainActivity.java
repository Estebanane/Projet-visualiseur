package com.example.test_lecture_musique;

import androidx.appcompat.app.ActionBarDrawerToggle;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.drawerlayout.widget.DrawerLayout;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.os.Bundle;
import android.provider.MediaStore;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.material.navigation.NavigationView;

import java.io.File;
import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {
    //Menu

    private DrawerLayout tiroir;
    private NavigationView vueNav;
    private ActionBarDrawerToggle toggle;

    RecyclerView _recyclerView;
    TextView _Pas_de_Musiques;

    ArrayList<ModelAudio> ListeMusiques = new ArrayList<>();

    TextView _Musiques;


    @SuppressLint("SuspiciousIndentation")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
/*
====
Menu
====
*/
        //Association objet et vue du menu
        vueNav = (NavigationView) findViewById(R.id.navigationvue);
        tiroir = (DrawerLayout) findViewById(R.id.drawerLayout);
        toggle = new ActionBarDrawerToggle(this, tiroir, R.string.open,  R.string.close);

        //Pour action sur le menu
        tiroir.addDrawerListener(toggle);
        toggle.syncState();

        vueNav.setNavigationItemSelectedListener(new NavigationView.OnNavigationItemSelectedListener() {


            // balayage des rubriques du menu
            @Override
            public boolean onNavigationItemSelected(MenuItem item) {  switch (item.getItemId()) {
                case R.id.menuItem:
                    Toast toast = Toast.makeText(getApplicationContext(),  "Home", Toast.LENGTH_SHORT);
                    toast.show();
                    break;
            }
                return true;
            }
        });


/*
====
Playlist
====
*/
        _recyclerView =findViewById(R.id.recycler_view);
        _Pas_de_Musiques =findViewById(R.id.Pas_de_Musiques_text);

        if(checkPermission()==false)
        {
            requestPermission();
            return;
        }

        String[] projection ={
            MediaStore.Audio.Media.TITLE,
            MediaStore.Audio.Media.DATA,
            MediaStore.Audio.Media.DURATION,
        };

        String selection = MediaStore.Audio.Media.IS_MUSIC +"!=0";

        Cursor curseur = getContentResolver().query(MediaStore.Audio.Media.EXTERNAL_CONTENT_URI,projection,selection,null,null);

        while (curseur.moveToNext()){
            ModelAudio Données_des_Musiques = new ModelAudio(curseur.getString(1),curseur.getString(0),curseur.getString(2));

            if(new File(Données_des_Musiques.getChemin()).exists())
            ListeMusiques.add(Données_des_Musiques);
        }

        if (ListeMusiques.size()==0)
        {
            _Pas_de_Musiques.setVisibility(View.VISIBLE);
        }
        else
        {
            //recyclerview
            _recyclerView.setLayoutManager(new LinearLayoutManager(this));
            _recyclerView.setAdapter(new MusiqueListeAdapteur(ListeMusiques, getApplicationContext()));
        }

    }

    boolean checkPermission(){
        int result = ContextCompat.checkSelfPermission(MainActivity.this, Manifest.permission.READ_EXTERNAL_STORAGE);
        if(result== PackageManager.PERMISSION_GRANTED)
        {
            return true;
        }
        else
        {
            return false;
        }

    }

    void requestPermission(){
        if(ActivityCompat.shouldShowRequestPermissionRationale(MainActivity.this, Manifest.permission.READ_EXTERNAL_STORAGE))
        {
            Toast.makeText(MainActivity.this, "Lire, demande de permission, accepter dans les paramètres", Toast.LENGTH_SHORT).show();
        }
        else
        {
            ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, 123);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if(_recyclerView!=null){
            _recyclerView.setAdapter(new MusiqueListeAdapteur(ListeMusiques,getApplicationContext()));
        }
    }
}