package com.example.test_lecture_musique;

import androidx.appcompat.app.AppCompatActivity;

import android.media.MediaPlayer;
import android.media.audiofx.Visualizer;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.TextView;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeUnit;

public class MusicPlayerActivity extends AppCompatActivity {

    SeekBar _Seekbar;
    TextView _Titre;
    TextView _TempsMusique;
    TextView _TempsDeFinMusique;
    ImageView _ProchaineMusique;
    ImageView _PrecedenteMusique;
    ImageView _Pause_Play_Musique;
    ImageView _Disque_Icon;
    int x=0;

    ArrayList<ModelAudio> Liste_Musiques;
    ModelAudio MusiqueActuelle;
    MediaPlayer mediaPlayer = Lecteur_Multimedia.getInstance();
    Visualizer visualizer = new Visualizer(mediaPlayer.getAudioSessionId());
    

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_music_player);

        _Seekbar = findViewById(R.id.seekbar);
        _Titre = findViewById(R.id.titre_musique);
        _TempsMusique = findViewById(R.id.tempsMusique);
        _TempsDeFinMusique = findViewById(R.id.tempsFinMusique);
        _ProchaineMusique = findViewById(R.id.next_Musique);
        _PrecedenteMusique = findViewById(R.id.previous_Musique);
        _Pause_Play_Musique = findViewById(R.id.pause_play);
        _Disque_Icon = findViewById(R.id.disque_icon);

        _Titre.setSelected(true);

        Liste_Musiques = (ArrayList<ModelAudio>) getIntent().getSerializableExtra("LIST");

        RessourcesAveclaMusique();

//Récupération fréquence et amplitude de la musique

        visualizer.setCaptureSize(Visualizer.getCaptureSizeRange()[1]);

        visualizer.setDataCaptureListener(new Visualizer.OnDataCaptureListener() {
            @Override
            public void onWaveFormDataCapture(Visualizer visualizer, byte[] waveform, int samplingRate) {

                List<Integer> amplitudes = new ArrayList<>();

// Test de Récupération d'amplitude (pas utile pour le projet)


                // Parcourez le tableau waveform pour accéder à chaque échantillon d'amplitude
                for (int i = 0; i < waveform.length; i++) {
                    // La valeur d'amplitude est comprise entre -128 et 127
                    //-128 pas de musiques ou pas de son
                    //127 =10
                    int amplitude = waveform[i];

                    amplitudes.add(amplitude);

                    // Affichage de l'amplitude dans la console
                    Log.i("BT", "Amplitude: " + amplitude);
                }

            }

            @Override
            public void onFftDataCapture(Visualizer visualizer, byte[] fft, int samplingRate) {
                // Fréquence
             int fftSize = fft.length / 2;

//Test de récupération de la Fréquence (pas utile pour le projet)


                // Parcourez le tableau fft pour accéder à chaque paire de valeurs (partie réelle, partie imaginaire)
                for (int i = 0; i < fftSize; i++) {
                    // La partie réelle et la partie imaginaire d'une bande de fréquence
                    byte real = fft[i * 2];
                    byte imaginary = fft[i * 2 + 1];

                    // Calculez l'amplitude de la bande de fréquence à partir de la partie réelle et la partie imaginaire
                    double amplitude = Math.sqrt((real * real) + (imaginary * imaginary));

                    // Affichez l'amplitude dans la console
                    Log.i("BTT", "Amplitude FFT: " + amplitude);
                }


                for(int i=0;i<fft.length;i+=2) {
                    byte real = fft[i];
                    byte imag = fft[i+1];
                    long magnitudeSquare = real*real + imag*imag;
                }

            }
        },Visualizer.getMaxCaptureRate()/2,true,false);

        visualizer.setEnabled(true);
        //stop le visualiser
        mediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(MediaPlayer mp) {
                visualizer.setEnabled(false);
            }
        });





        MusicPlayerActivity.this.runOnUiThread(new Runnable(){
            @Override
            public void run(){

                if(mediaPlayer!=null) {
                    _Seekbar.setProgress(mediaPlayer.getCurrentPosition());
                    _TempsMusique.setText(ConvertToMMSS(mediaPlayer.getCurrentPosition()+""));

                    if(mediaPlayer.isPlaying()){
                        _Pause_Play_Musique.setImageResource(R.drawable.ic_baseline_pause_circle_outline_24);

                        _Disque_Icon.setRotation(x++);//rotation du disque


                    }
                    else
                    {
                        _Pause_Play_Musique.setImageResource(R.drawable.ic_baseline_play_circle_outline_24);

                        _Disque_Icon.setRotation(0);//Arret rotation du disque
                    }

                }
                new Handler().postDelayed(this,100);
            }
        });

        _Seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (mediaPlayer!=null && fromUser){
                    mediaPlayer.seekTo(progress);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

    }
    void RessourcesAveclaMusique(){
        MusiqueActuelle = Liste_Musiques.get(Lecteur_Multimedia.currentIndex);

        _Titre.setText(MusiqueActuelle.getTitre());

        _TempsDeFinMusique.setText(ConvertToMMSS(MusiqueActuelle.getDuree()));

        _Pause_Play_Musique.setOnClickListener(v -> pauseMusique());
        _ProchaineMusique.setOnClickListener(v -> playProchaineMusique());
        _PrecedenteMusique.setOnClickListener(v -> playPrecedenteMusique());

        playMusique();
    }

    private void playMusique(){

        mediaPlayer.reset();
        try {
            mediaPlayer.setDataSource(MusiqueActuelle.getChemin());
            mediaPlayer.prepare();
            mediaPlayer.start();
            _Seekbar.setProgress(0);
            _Seekbar.setMax(mediaPlayer.getDuration());
        }
        catch (IOException e){
            e.printStackTrace();
        }
    }

    private void playProchaineMusique(){

        if(Lecteur_Multimedia.currentIndex==Liste_Musiques.size()-1)
                return;

        Lecteur_Multimedia.currentIndex +=1;
        mediaPlayer.reset();
        RessourcesAveclaMusique();

    }
    private void playPrecedenteMusique(){
        if(Lecteur_Multimedia.currentIndex==0)
            return;

        Lecteur_Multimedia.currentIndex -=1;
        mediaPlayer.reset();
        RessourcesAveclaMusique();
    }
    private void pauseMusique(){
        if(mediaPlayer.isPlaying()) {
            mediaPlayer.pause();
        }
        else {
            mediaPlayer.start();
        }
    }

//conversion pour les temps de la musique
    public static String ConvertToMMSS(String duration ){

        Long millis = Long.parseLong(duration);
        return String.format("%02d:%02d",
                TimeUnit.MILLISECONDS.toMinutes(millis) % TimeUnit.HOURS.toMinutes(1),
                TimeUnit.MILLISECONDS.toSeconds(millis) % TimeUnit.MINUTES.toSeconds(1));


    }


}