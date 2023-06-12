package com.example.visualiseur_musicale;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;

import androidx.core.app.ActivityCompat;
import androidx.fragment.app.Fragment;

import android.os.SystemClock;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Set;
import java.util.UUID;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link Bluetooth#newInstance} factory method to
 * create an instance of this fragment.
 */
public class Bluetooth extends Fragment {

    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    private View vue;
    private Button boutonActivationBT;
    private Button boutonAppareils;
    private Button boutonEfface;
    private Button boutonConnect;
    private Button boutonLire;

    private TextView vide;
    private ListView AppareilsAsso;

    private BluetoothAdapter mBluetoothAdapter; // objets pour manipuler le BT
    private BluetoothManager mBluetoothManager;
    private PackageManager packageManager;
    private Integer interfaceON = 1; // 4 drapeaux pour valider des étapes
    private Integer interfaceOK = 0;
    private Integer appareilConnecte = 0;
    private Integer autorisationConnexion = 1;

    private BluetoothDevice deviceTrouve;
    private BluetoothSocket mmSocket = null;

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
        boutonActivationBT = (Button) vue.findViewById(R.id.BActivationBT);
        boutonAppareils = (Button) vue.findViewById(R.id.BAppareils);
        boutonEfface = (Button) vue.findViewById(R.id.BEfface);
        AppareilsAsso = (ListView) vue.findViewById(R.id.liste);
        vide = (TextView) vue.findViewById(R.id.Vide);

        boutonConnect = (Button) vue.findViewById(R.id.Bconnect);
        boutonLire = (Button) vue.findViewById(R.id.BLireCode);

        ArrayAdapter<String> adapter = new ArrayAdapter<String>(getContext(), android.R.layout.simple_list_item_1, new ArrayList<String>());
        AppareilsAsso.setAdapter(adapter);


//Test de Bluetooth
        BluetoothManager mBluetoothManager = (BluetoothManager) getActivity().getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter == null) {
// Si le téléphone n'a pas de  Bluetooth
            Toast.makeText(getActivity(), "La machine ne possède pas le Bluetooth",
                    Toast.LENGTH_SHORT).show();
            interfaceOK = 0;
        } else {
//Si le téléphone a le Bluetooth
            interfaceOK = 1;
            Toast.makeText(getActivity(), "interface BT existe",
                    Toast.LENGTH_SHORT).show();
        }

//Pour activer le bluetooth sur le téléphone
        boutonActivationBT.setOnClickListener(v -> {

            if (interfaceOK == 1) {
                if (!mBluetoothAdapter.isEnabled()) {
                    Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    Log.i("BTT", "1");

                    if (ActivityCompat.checkSelfPermission(getContext(), android.Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {

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


        boutonAppareils.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (ActivityCompat.checkSelfPermission(getContext(), android.Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {

                }
                Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();

                if (pairedDevices.size() > 0) {
                    for (BluetoothDevice device : pairedDevices) {
                        adapter.add(device.getName() + "\n" + device.getAddress());
                        if (device.getAddress().equals("00:1C:97:1E:C7:B9")) {
// lecteur de code barres
                            deviceTrouve = device;
                            Log.i("BT", device.getAddress());
                        }
                    }
                }

            }
        });
        boutonEfface.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                adapter.clear();
            }
        });

//boutonn connect
        boutonConnect.setOnClickListener((view -> {
            new Thread(this::ConnexionAppareil).start();

        }));

//bouton pour lire le code barre
        boutonLire.setOnClickListener((view -> {
            new Thread(this::Liste).start();
        }));


        return vue;
    }

//Trame envoyé en bluetooth
    void Liste() {
        InputStream tmpIn = null;
        InputStream receiveStream = null;
        byte[] buffer = new byte[100]; // pour la lecture des données
        String resultat="";
        Integer tailleMessage;
        String nomMessage;
        TextView afficheReceptionBT; // pour afficher le code barre dans l'UI
        String finalResultat; // La chaine de caracteres dans laquelle est mémorisée lecode barre lu

// La chaine de caracteres dans laquelle est mémorisée le code barre lu
// code tache LireCode()
        int i;
        int numBytes;

// OutputStream tmpOut = null; // non utilisé ici pour le flux dans l'autre sens
// Get the BluetoothSocket input and output streams
        Integer lectureOK = 0;
        try {
            tmpIn = mmSocket.getInputStream();
// tmpOut = mmSocket.getOutputStream();
            receiveStream = tmpIn;
// sendStream = tmpOut; // pour un flux émis non fait ici
            Log.i("BT", "attente code-barre");
            tailleMessage = 0; // la variable tailleMessage permet de compter les lus. Un code complet envoie 15 caractères

// les deux derniers sont LF et CR
            do {
// Read from the InputStream
                numBytes = receiveStream.read(buffer);
                tailleMessage = tailleMessage + numBytes;
                for (i = 0; i < numBytes; i++) {
                    if ((buffer[i] != 10) && (buffer[i] != 13)) {
                        resultat = resultat + buffer[i];
                        Log.i("BT", "partiel " + resultat);
                        Log.i("BT", "partiel " + Integer.toString(numBytes));
                    }
                    if ((buffer[i] == 10) || (buffer[i] == 13)) {
                        lectureOK = 1;
                    }
                }
            }
            while (lectureOK!=1) ;
            if (lectureOK == 1) {
//lireCode.setText(resultat);
                Log.i("BT", "resultat final " + resultat);
            }
        }
// fin 2ème try
        catch(Exception e){
            Log.e("BT", "fin de message", e);
            return;
        }
    }

    void ConnexionAppareil() {
        BluetoothSocket tmp = null;
        if (interfaceON == 0) {
            Log.i("BT", "Le BT n’est pas prêt");
        } else {
// déclarer mmDevice dans l'activité

            if (deviceTrouve == null) {
                Log.i("BT", "le VMA302-01 n'est pas reconnu");
            } else {

                try {
                    if (ActivityCompat.checkSelfPermission(vue.getContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                    }
                    tmp =
                            deviceTrouve.createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"));
                    Log.i("BT", "socket cree");
                    autorisationConnexion = 1;
                } catch (IOException e) {
                    Log.i("BT", "erreur creation socket");
                    return; // si pb on s'arrete là
                }

                if (autorisationConnexion == 1) {
                    mmSocket = tmp;

                    try {

                        Log.i("BT", "début attente connexion");
                        mmSocket.connect();
                        SystemClock.sleep(12000); // ce sont des ms
                        Log.i("BT", "fin attente connexion - connexion ok ");
                        appareilConnecte=1;
                    } catch (IOException e) {
                        Log.e("BT", e.getMessage());
// ceci permt d’obtenir des infos sur le pb, si pb il y

                        try {
// si la connexion ne se fait pas on ferme le socket
                            mmSocket.close();
                            Log.i("BT", "erreur fermeture socket ");
                        } catch (IOException e2) {
                            Log.i("BT", "impossible de fermer le socket");
                        }
                    }
                }

            }
        }}



    @Override

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode,data);
        Log.i("BT", "onActivityResult, requestCode: " + requestCode + ", resultCode: "
                + resultCode);
        if (resultCode !=0) {
            interfaceON =1;
// Ici on pourrait ajouter de code
        }
        else interfaceON=0;
    }
}