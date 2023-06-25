package com.example.test_lecture_musique;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.pm.PackageManager;
import android.os.Bundle;

import androidx.core.app.ActivityCompat;
import androidx.fragment.app.Fragment;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;

import java.io.IOException;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link Bluetooth#newInstance} factory method to
 * create an instance of this fragment.
 */
public class Bluetooth extends Fragment {

    private View vue;


    private boolean Connecte =false;

    //Bouton
    Button bt_buttonBTConnect;

    //EditText
    TextView bt_textviewVMA302;

   //Bluetooth
    Set<BluetoothDevice> BTPairedDevices = null;
    BluetoothDevice BTDevice = null;
    BluetoothSocket BTSocket = null;
    private OutputStream outputStream=null;
    static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    BluetoothAdapter BTAdaptor = null;



    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    public Bluetooth() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment Bluetooth.
     */
    // TODO: Rename and change types and number of parameters
    public static Bluetooth newInstance(String param1, String param2) {
        Bluetooth fragment = new Bluetooth();
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


        vue = inflater.inflate(R.layout.fragment_bluetooth, container, false);

        bt_buttonBTConnect = vue.findViewById(R.id.ButtonConnect);
        bt_textviewVMA302 = vue.findViewById(R.id.TextVMA302);

//Boutton Connect
        bt_buttonBTConnect.setOnClickListener(v -> {
            if (Connecte ==false){
                BTAdaptor = BluetoothAdapter.getDefaultAdapter();
                if (ActivityCompat.checkSelfPermission(getContext(), android.Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {

                }
                BTPairedDevices = BTAdaptor.getBondedDevices();
                for (BluetoothDevice BTDev : BTPairedDevices){
                    if(BTDev.getName().equals("VMA302-01")){
                        BTDevice = BTDev;

                        try{
                            BTSocket = BTDevice.createRfcommSocketToServiceRecord(MY_UUID);
                            BTSocket.connect();
                            outputStream=BTSocket.getOutputStream();

                            bt_textviewVMA302.setText("VMA302-01 Connecté");
                            bt_buttonBTConnect.setText("Se déconnecter");
                            Connecte=true;
                            Log.i ("BT", "connecté");

                        }
                        catch (Exception exp)

                        {
                            bt_textviewVMA302.setText("VMA302-01 Non Connecté");
                        }
                    }
                }
            }
            else
            {
                try {
                    BTSocket.close();
                    bt_textviewVMA302.setText("VMA302-01  Non Connecté");
                    bt_buttonBTConnect.setText("Se connecter");
                    Connecte=false;
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });

        // Inflate the layout for this fragment
        return vue;
    }

//Fonction pour envoyer les données pas BT
    public void write(byte[] bytes)
    {
        try {
            outputStream.write(bytes);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }




}