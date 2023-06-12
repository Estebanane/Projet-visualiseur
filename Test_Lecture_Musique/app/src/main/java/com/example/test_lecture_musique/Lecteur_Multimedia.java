package com.example.test_lecture_musique;

import android.media.MediaPlayer;

public class Lecteur_Multimedia {
    static MediaPlayer instance;

    public static MediaPlayer getInstance() {
        if(instance==null)
        {
            instance= new MediaPlayer();
        }
        return instance;
    }

    public static int currentIndex =-1;
}
