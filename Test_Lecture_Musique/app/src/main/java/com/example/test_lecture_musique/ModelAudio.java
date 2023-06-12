package com.example.test_lecture_musique;

import java.io.Serializable;

public class ModelAudio implements Serializable {

    String chemin;
    String titre;
    String duree;

    public ModelAudio(String chemin, String titre, String duree) {
        this.chemin = chemin;
        this.titre = titre;
        this.duree = duree;
    }

    public String getChemin() {
        return chemin;
    }

    public void setChemin(String chemin) {
        this.chemin = chemin;
    }

    public String getTitre() {
        return titre;
    }

    public void setTitre(String titre) {
        this.titre = titre;
    }

    public String getDuree() {
        return duree;
    }

    public void setDuree(String duree) {
        this.duree = duree;
    }
}
