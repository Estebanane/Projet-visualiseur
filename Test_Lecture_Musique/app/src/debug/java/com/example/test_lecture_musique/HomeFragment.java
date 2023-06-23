package com.example.test_lecture_musique;

import android.Manifest;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.os.Bundle;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.provider.MediaStore;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.util.ArrayList;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link HomeFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class HomeFragment extends Fragment {

    private View vue;

    private Bluetooth FragBluetooth;
    TextView _BT;

    RecyclerView _recyclerView;
    TextView _Pas_de_Musiques;

    ArrayList<ModelAudio> ListeMusiques = new ArrayList<>();

    TextView _Musiques;





    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    public HomeFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment HomeFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static HomeFragment newInstance(String param1, String param2) {
        HomeFragment fragment = new HomeFragment();
        Bundle args = new Bundle();
        args.putString(ARG_PARAM1, param1);
        args.putString(ARG_PARAM2, param2);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            mParam1 = getArguments().getString(ARG_PARAM1);
            mParam2 = getArguments().getString(ARG_PARAM2);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        vue =inflater.inflate(R.layout.fragment_home, container, false);


        /*
========
Playlist
========
*/
        _recyclerView = vue.findViewById(R.id.recycler_view);
        _Pas_de_Musiques = vue.findViewById(R.id.Pas_de_Musiques_text);

        if(checkPermission()==false)
        {
            requestPermission();

        }




    Cursor cursor = MainActivity.MAINACTIVITY.cursor();
        while (cursor.moveToNext()){
            ModelAudio Données_des_Musiques = new ModelAudio(cursor.getString(1),cursor.getString(0),cursor.getString(2));

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
            _recyclerView.setLayoutManager(new LinearLayoutManager(getContext()));
            _recyclerView.setAdapter(new MusiqueListeAdapteur(ListeMusiques, MainActivity.MAINACTIVITY.getApplicationContext()));
        }
        return vue;
    }

    //PLaylist
    boolean checkPermission(){
        int result = ContextCompat.checkSelfPermission(getContext(), android.Manifest.permission.READ_EXTERNAL_STORAGE);
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
        if(ActivityCompat.shouldShowRequestPermissionRationale(MainActivity.MAINACTIVITY, android.Manifest.permission.READ_EXTERNAL_STORAGE))
        {
            Toast.makeText(getContext(), "Lire, demande de permission, accepter dans les paramètres", Toast.LENGTH_SHORT).show();
        }
        else
        {
            ActivityCompat.requestPermissions(MainActivity.MAINACTIVITY, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, 123);
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        if(_recyclerView!=null){
            _recyclerView.setAdapter(new MusiqueListeAdapteur(ListeMusiques,MainActivity.MAINACTIVITY.getApplicationContext()));
        }
    }

}