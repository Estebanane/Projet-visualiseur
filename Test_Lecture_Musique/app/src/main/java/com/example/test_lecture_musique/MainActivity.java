package com.example.test_lecture_musique;

import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBarDrawerToggle;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.drawerlayout.widget.DrawerLayout;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.ContentResolver;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.example.test_lecture_musique.databinding.ActivityMainBinding;
import com.google.android.material.bottomnavigation.BottomNavigationView;
import com.google.android.material.navigation.NavigationBarView;
import com.google.android.material.navigation.NavigationView;

import java.io.File;
import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    BottomNavigationView bottomNavigationView;

    public HomeFragment homeFragment = new HomeFragment();
    public Bluetooth bluetoothFragment = new Bluetooth();
    Couleurs_LED couleurs_ledFragment = new Couleurs_LED();

    private Bluetooth FragBluetooth;
    TextView _BT;

    public static  MainActivity MAINACTIVITY;
/*
    private void lanceFragment(Fragment fragment) {
        FragmentManager fm =getSupportFragmentManager();
        FragmentTransaction ft= fm.beginTransaction();
        ft.replace(R.id.frame_layout,fragment);
        ft.addToBackStack(null);
        ft.commit();
    }
    */


    @SuppressLint("SuspiciousIndentation")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        MAINACTIVITY = this;


/*
=============
NavigationVue
=============
*/
bottomNavigationView = findViewById(R.id.bottom_navigation);
getSupportFragmentManager().beginTransaction().replace(R.id.container,homeFragment).commit();

bottomNavigationView.setOnItemSelectedListener(new NavigationBarView.OnItemSelectedListener() {
    @Override
    public boolean onNavigationItemSelected(@NonNull MenuItem item) {

        switch (item.getItemId()){
            case R.id.superHome:
                getSupportFragmentManager().beginTransaction().replace(R.id.container,homeFragment).commit();
                return true;
            case R.id.superBluetooth:
                getSupportFragmentManager().beginTransaction().replace(R.id.container,bluetoothFragment).commit();
                return true;
            case R.id.superLed:
                getSupportFragmentManager().beginTransaction().replace(R.id.container,couleurs_ledFragment).commit();
                return true;
        }

        return false;
    }
});



    }

//Navigation View

public Cursor cursor  (){
        int a = 30;
    String[] projection ={
            MediaStore.Audio.Media.TITLE,
            MediaStore.Audio.Media.DATA,
            MediaStore.Audio.Media.DURATION,
    };

    String selection = MediaStore.Audio.Media.IS_MUSIC +"!=0";

    ContentResolver resolver = getContentResolver();

    return resolver.query(MediaStore.Audio.Media.EXTERNAL_CONTENT_URI,projection,selection,null,null);
}

}