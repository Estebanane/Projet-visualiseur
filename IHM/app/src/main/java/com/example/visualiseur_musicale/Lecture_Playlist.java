package com.example.visualiseur_musicale;

import android.content.Context;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.os.Handler;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.TextView;

import com.google.android.material.snackbar.Snackbar;

import java.util.ArrayList;
import java.util.List;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link Lecture_Playlist#newInstance} factory method to
 * create an instance of this fragment.
 */
public class Lecture_Playlist extends Fragment {


    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    private View vue;
    private ImageView _boutonReplay;
    private ImageView _boutonPrevious;
    private ImageView _boutonPlay;
    private ImageView _boutonPause;
    private ImageView _boutonNext;
    private SeekBar _seekbarVolume;
    private ProgressBar _progressBarTemps;
    private TextView _tempsDebut;
    private TextView _tempsFin;
    private ListView _listeMusique;


    private Handler mHandler;
    private Runnable mRunnable;
    AudioManager audioManager;

    public Lecture_Playlist() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment Lecture_Playlist.
     */
    // TODO: Rename and change types and number of parameters
    public static Lecture_Playlist newInstance(String param1, String param2) {
        Lecture_Playlist fragment = new Lecture_Playlist();
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

        vue = inflater.inflate(R.layout.fragment_lecture__playlist, container, false);

        _boutonReplay = (ImageView) vue.findViewById(R.id.imageButtonReplay);
        _boutonPrevious = (ImageView) vue.findViewById(R.id.imageButtonPrevious);
        _boutonPlay = (ImageView) vue.findViewById(R.id.imageButtonPlay);
        _boutonPause = (ImageView) vue.findViewById(R.id.imageButtonPause);
        _boutonNext = (ImageView) vue.findViewById(R.id.imageButtonNext);
        _seekbarVolume = (SeekBar) vue.findViewById(R.id.seekbarVolume);
        _progressBarTemps = (ProgressBar) vue.findViewById(R.id.ProgressBarTemps);
        _listeMusique = (ListView) vue.findViewById(R.id.ListeMusique);
        _tempsDebut = (TextView) vue.findViewById(R.id.tempsDebut);
        _tempsFin = (TextView) vue.findViewById(R.id.tempsFin);



        ArrayAdapter<String> adapter = new ArrayAdapter<String>(getContext(), android.R.layout.simple_list_item_1, new ArrayList<String>());
        _listeMusique.setAdapter(adapter);

//Choix du volume pour l'utilisateur
        super.onCreate(savedInstanceState);

        //seebarVolume de la chanson
        _seekbarVolume.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                // La valeur du volume a changé, vous pouvez mettre à jour votre logique en fonction de la nouvelle valeur
                // Par exemple, vous pouvez ajuster le volume du média en utilisant AudioManager, ou mettre à jour l'affichage de la valeur du volume
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                // L'utilisateur a commencé à toucher la SeekBar
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                // L'utilisateur a terminé de toucher la SeekBar
            }
        });

//progressbar du temps de la chanson



        mHandler = new Handler();

        MainActivity.MEDIA_PLAYER.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
            @Override
            public void onPrepared(MediaPlayer mp) {
                // Pour avoir la durée de la musique joué
                int duration = MainActivity.MEDIA_PLAYER.getDuration();
                // Pour avoir la durée total de la musique sur la progressbar
                _progressBarTemps.setMax(duration);

                // Pour pouvoir faire avancer la progressbar en fonction du moment où se trouve la musique
                mRunnable = new Runnable() {
                    @Override
                    public void run() {
                        if (MainActivity.MEDIA_PLAYER != null) {
                            int currentPosition = MainActivity.MEDIA_PLAYER.getCurrentPosition();
                           _progressBarTemps.setProgress(currentPosition);
                            mHandler.postDelayed(this, 1000); // Pour mettre à jour toutes les 1000 ms
                        }
                    }
                };
                mHandler.postDelayed(mRunnable, 1000); //Pour mettre à jour la ProgressBar
            }
        });

//les boutons
        _boutonReplay.setOnClickListener(view -> {
            if (MainActivity.MEDIA_PLAYER.isPlaying()) {
                MainActivity.MEDIA_PLAYER.pause();

            } else if (!MainActivity.MEDIA_PLAYER.isPlaying()) {
                MainActivity.MEDIA_PLAYER.start();

                playCycle();
            }
        });

        _boutonPrevious.setOnClickListener(view -> {

        });

        _boutonPlay.setOnClickListener(view -> {
            MainActivity.MEDIA_PLAYER.start();
        });

        _boutonPause.setOnClickListener(view -> {
            MainActivity.MEDIA_PLAYER.pause();
        });

        _boutonNext.setOnClickListener(view -> {

        });


        // Démarrer la lecture de la musique




//
    /*
    protected void onDestroy() {
        super.onDestroy();
        if (mediaPlayer != null) {
            mediaPlayer.release();
            mediaPlayer = null;
        }
        // Arrêter la mise à jour de la ProgressBar
        mHandler.removeCallbacks(mRunnable);
    }
*/

        return vue;
    }


    private void playCycle(){

        try {
            _seekbarVolume.setProgress(MainActivity.MEDIA_PLAYER.getCurrentPosition());
            //_tempsDebut.setText(getTimeFormatted(mediaPlayer.getCurrentPosition()));
            if (MainActivity.MEDIA_PLAYER.isPlaying()) {
                mRunnable = new Runnable() {
                    @Override
                    public void run() {
                        playCycle();

                    }
                };
                mHandler.postDelayed(mRunnable, 100);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


}
