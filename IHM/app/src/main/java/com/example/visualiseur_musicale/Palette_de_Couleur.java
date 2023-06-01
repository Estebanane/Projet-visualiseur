package com.example.visualiseur_musicale;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;

import androidx.core.app.ActivityCompat;
import androidx.fragment.app.Fragment;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextClock;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Set;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link Palette_de_Couleur#newInstance} factory method to
 * create an instance of this fragment.
 */
public class Palette_de_Couleur extends Fragment {

    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    //Bluetooth
    private View vue;
    private Button _ActiveBT;
    private Button _ConnectBT;
    private BluetoothAdapter mBluetoothAdapter; // objets pour manipuler le BT
    private BluetoothManager mBluetoothManager;
    private Integer interfaceON = 1; // 4 drapeaux pour valider des étapes
    private Integer interfaceOK = 0;

    public Palette_de_Couleur() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment Palette_de_Couleur.
     */
    // TODO: Rename and change types and number of parameters
    public static Palette_de_Couleur newInstance(String param1, String param2) {
        Palette_de_Couleur fragment = new Palette_de_Couleur();
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
        vue = inflater.inflate(R.layout.fragment_palette_de__couleur, container, false);

        _ActiveBT = (Button) vue.findViewById(R.id.BActivationBT);
        _ConnectBT = (Button) vue.findViewById(R.id.Bconnect);


        //Le code ci-dessous permet de tester si l’appareil est doté d’une interface BT
        BluetoothManager mBluetoothManager = (BluetoothManager) getActivity().getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter == null) {
// Device doesn't support Bluetooth
            Toast.makeText(getActivity(), "La machine ne possède pas le Bluetooth",
                    Toast.LENGTH_SHORT).show();
            interfaceOK = 0;
        } else {
            interfaceOK = 1;
            Toast.makeText(getActivity(), "interface BT existe",
                    Toast.LENGTH_SHORT).show();
        }

        _ActiveBT.setOnClickListener(v -> {
            if (interfaceOK == 1) {
                if (!mBluetoothAdapter.isEnabled()) {
                    Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    Log.i("BTT", "1");

                    if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                        // TODO: Consider calling
                        //    ActivityCompat#requestPermissions
                        // here to request the missing permissions, and then overriding
                        //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                        //                                          int[] grantResults)
                        // to handle the case where the user grants the permission. See the documentation
                        // for ActivityCompat#requestPermissions for more details.
                        //return;
                    }
                    getActivity().startActivityForResult(enableBtIntent, 10);

                    Log.i("BTT", "2");
                } else {
                    interfaceON = 1; // le bluetooth est allumé
                    Toast.makeText(getActivity(), "Bluetooth allumé",
                            Toast.LENGTH_SHORT).show();
                }
            }


        });




        return vue;
    }
}