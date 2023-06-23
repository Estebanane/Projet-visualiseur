package com.example.test_lecture_musique;

import android.os.Bundle;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.SeekBar;

import java.util.ArrayList;
import java.util.List;

import yuku.ambilwarna.AmbilWarnaDialog;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link Couleurs_LED#newInstance} factory method to
 * create an instance of this fragment.
 */
public class Couleurs_LED extends Fragment {

    private View vue;

    Button button;
    ConstraintLayout constraintLayout;
    int defaultColor;

    int red;
    int green;
    int blue;

    Button led_buttonPile1;
    Button led_buttonPile2;
    Button led_buttonPile3;
    Button led_buttonPile4;
    Button led_buttonPile5;
    Button led_buttonDemo0;
    Button led_buttonDemo1;

    SeekBar led_seekbarIntensite;

    SeekBar led_seekbarVitesse;

    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    public Couleurs_LED() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment Couleurs_LED.
     */
    // TODO: Rename and change types and number of parameters
    public static Couleurs_LED newInstance(String param1, String param2) {
        Couleurs_LED fragment = new Couleurs_LED();
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

        vue = inflater.inflate(R.layout.fragment_couleurs__l_e_d, container, false);

        button=vue.findViewById(R.id.btn1);
        constraintLayout=vue.findViewById(R.id.layout);

        led_buttonPile1 = vue.findViewById(R.id.pile1);
        led_buttonPile2 = vue.findViewById(R.id.pile2);
        led_buttonPile3 = vue.findViewById(R.id.pile3);
        led_buttonPile4 = vue.findViewById(R.id.pile4);
        led_buttonPile5 = vue.findViewById(R.id.pile5);
        led_buttonDemo0 = vue.findViewById(R.id.Demo0);
        led_buttonDemo1 = vue.findViewById(R.id.Demo1);

        led_seekbarIntensite = vue.findViewById(R.id.seekbarINTENSITE);
        led_seekbarVitesse = vue.findViewById(R.id.seekbarVitesse);


        defaultColor= ContextCompat.getColor(MainActivity.MAINACTIVITY, com.google.android.material.R.color.design_default_color_primary);

//Bouton couleur
        button.setOnClickListener(v -> {
            openColorPicker();
        });

//PILE 1
        led_buttonPile1.setOnClickListener(v -> {
            MainActivity.MAINACTIVITY.bluetoothFragment.write(GetPileTrame((byte) 0));

        });

//PILE 2
        led_buttonPile2.setOnClickListener(v -> {
            MainActivity.MAINACTIVITY.bluetoothFragment.write(GetPileTrame((byte) 1));
        });

//PILE 3

        led_buttonPile3.setOnClickListener(v -> {
            MainActivity.MAINACTIVITY.bluetoothFragment.write(GetPileTrame((byte) 2));

        });

//PILE 4
        led_buttonPile4.setOnClickListener(v -> {
            MainActivity.MAINACTIVITY.bluetoothFragment.write(GetPileTrame((byte) 3));

        });

//PILE 5

        led_buttonPile5.setOnClickListener(v -> {
            MainActivity.MAINACTIVITY.bluetoothFragment.write(GetPileTrame((byte) 4));

        });


//DEMO 0

        led_buttonDemo0.setOnClickListener(v -> {
            MainActivity.MAINACTIVITY.bluetoothFragment.write(GetDemoTrame((byte) 0));
        });

//DEMO1
        led_buttonDemo1.setOnClickListener(v -> {
            MainActivity.MAINACTIVITY.bluetoothFragment.write(GetDemoTrame((byte) 1));
        });


        return vue;
    }

    public byte[] GetDemoTrame (byte numDemo){
        List<Byte> TableauTrame = new ArrayList<>();
        TableauTrame.add((byte) 'D');
        TableauTrame.add((byte) 'E');
        TableauTrame.add((byte) 'M');
        TableauTrame.add((byte) 'O');
        AddString(TableauTrame, numDemo);
        TableauTrame.add((byte) ' ');
        AddString(TableauTrame, (byte) led_seekbarVitesse.getProgress()); //Vitesse du bandeau de lED
        TableauTrame.add((byte) '\r');
        TableauTrame.add((byte) '\n');


        byte[] tab = new byte[TableauTrame.size()];
        for(int i = 0; i < TableauTrame.size(); ++i)
            tab[i] = TableauTrame.get(i);

        return tab;
    }

    public byte[] GetPileTrame (byte numPile){
        List<Byte> TableauTrame = new ArrayList<>();

        TableauTrame.add((byte) 'P');
        TableauTrame.add((byte) 'I');
        TableauTrame.add((byte) 'L');
        TableauTrame.add((byte) 'E');
        TableauTrame.add((byte) ' ');

        AddString(TableauTrame, (byte) red);   //RED
        TableauTrame.add((byte) ' ');
        AddString(TableauTrame, (byte) green);   //Green
        TableauTrame.add((byte) ' ');
        AddString(TableauTrame, (byte) blue);   //Blue
        TableauTrame.add((byte) ' ');
        AddString(TableauTrame, (byte) led_seekbarIntensite.getProgress()); //Intensité
        TableauTrame.add((byte) ' ');
        AddString(TableauTrame, numPile); // Choix de la pile
        TableauTrame.add((byte) '\r');
        TableauTrame.add((byte) '\n');


        byte[] tab = new byte[TableauTrame.size()];
        for(int i = 0; i < TableauTrame.size(); ++i)
            tab[i] = TableauTrame.get(i);

        return tab;
    }


    public void AddString (List<Byte> TAB , byte Valeur){
        String ValeurString = String.valueOf(Valeur);
        for(int i = 0; i < ValeurString.length(); ++i)
            TAB.add((byte) ValeurString.charAt(i));
    }


    //choix des couleurs
    public void openColorPicker(){

        AmbilWarnaDialog ambilWarnaDialog = new AmbilWarnaDialog(MainActivity.MAINACTIVITY, defaultColor, new AmbilWarnaDialog.OnAmbilWarnaListener() {
            @Override
            public void onCancel(AmbilWarnaDialog dialog) {

            }

            @Override
            public void onOk(AmbilWarnaDialog dialog, int color) {
                defaultColor = color;
                constraintLayout.setBackgroundColor(defaultColor); //affiche la couleur choisi sur le background
                //Pour obtenir les couleurs sous formes R G B
                 red = (color >> 16) & 0xFF;
                 green = (color >> 8) & 0xFF;
                 blue = color & 0xFF;

                // Afficher les composantes RGB
                String rgb = "RGB: " + red + ", " + green + ", " + blue;
                Log.i("RGB", rgb);

            }
        });
        ambilWarnaDialog.show();
    }

}