package com.example.test_lecture_musique;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.constraintlayout.helper.widget.Layer;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;

public class MusiqueListeAdapteur extends RecyclerView.Adapter<MusiqueListeAdapteur.Vue> {

    ArrayList<ModelAudio> Liste_Musiques;
    Context context;

    public MusiqueListeAdapteur(ArrayList<ModelAudio> liste_Musiques, Context context) {
        Liste_Musiques = liste_Musiques;
        this.context = context;
    }

    @Override
    public Vue onCreateViewHolder( ViewGroup parent, int viewType) {
        View vue = LayoutInflater.from(context).inflate(R.layout.recycler_item,parent,false);
        return new MusiqueListeAdapteur.Vue(vue);
    }


    @Override
    public void onBindViewHolder(MusiqueListeAdapteur.Vue holder, int position) {
        ModelAudio songData = Liste_Musiques.get(position);
        holder.titreTextView.setText(songData.getTitre());

        if(Lecteur_Multimedia.currentIndex==position){
            holder.titreTextView.setTextColor(Color.parseColor("#FF0000"));
        }
        else
        {
            holder.titreTextView.setTextColor(Color.parseColor("#000000"));
        }


        holder.itemView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //Pour naviguer à une autre activité

             Lecteur_Multimedia.getInstance().reset();
             Lecteur_Multimedia.currentIndex = position;
             Intent intent = new Intent(context, MusicPlayerActivity.class);
             intent.putExtra("LIST",Liste_Musiques);
             intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
             context.startActivity(intent);
            }
        });
    }

    @Override
    public int getItemCount() {
        return Liste_Musiques.size();
    }

    public class Vue extends RecyclerView.ViewHolder {

        TextView titreTextView;
        ImageView iconImageView;

        public Vue(View itemView) {
            super(itemView);

            titreTextView = itemView.findViewById(R.id.titre_de_la_musique);
            iconImageView = itemView.findViewById(R.id.icon_view);

        }
    }
}
