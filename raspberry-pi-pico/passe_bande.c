#include "passe_bande.h"

#include "hardware/adc.h"

#include <stdio.h>

#define ADC_VREF        3.3
#define ADC_RANGE       (1 << 12)
#define ADC_CONVERT     (ADC_VREF / (ADC_RANGE - 1))

#define LED_ADC_OFFSET      1.8f
#define LED_ADC_RANGE       1.5f

// Numérateur des filtres à multiplier aux échantillons FiltreX_num * En[x];
float Filtre1_num[5] = {
    0.000261295744083394655687957452983027906f,
    0.0f,
    -0.000522591488166789311375914905966055812f,
    0.0f,
    0.000261295744083394655687957452983027906f
};

// float Filtre2_num[3] = {
//     0.014623132928667393476418467912480991799f,
//     0.0f,
//     -0.014623132928667393476418467912480991799f
// };

float Filtre3_num[5] = {
    0.003218852170340172000329026502640772378f, 
    0.0f,
    -0.006437704340680344000658053005281544756f,
    0.0f,
    0.003218852170340172000329026502640772378f
};
float Filtre4_num[3] = {
    0.083817117889534220598335423346725292504f,
    0.0f,
    -0.083817117889534220598335423346725292504f
};

float Filtre5_num[5] = {
    0.082557510657307014989569893259613309056f,
    0.0f,
    -0.165115021314614029979139786519226618111f,
    0.0f,
    0.082557510657307014989569893259613309056f
};

//Dénominateur des filtre à multiplié avec les itérations précédentes de S(n);
float Filtre1_den[5] = {
    1.0f,
    -3.953495902644310611151468037860468029976f,
    5.861802915777815314868348650634288787842f,
    -3.863112053901564202362806099699810147285f,
    0.954805058190714484034344877727562561631f
};

float Filtre2_num[3]={ 0.061445286504494919799235219670663354918, 0,-0.061445286504494919799235219670663354918};
float Filtre2_den[2] ={-1.862134180264185756570327612280379980803,  0.877109426991010132645953945029759779572};

// float Filtre2_den[3] = {
//     1.0f,
//     -1.969944571806576671946231726906262338161f,
//     0.970753734142665192230481352453352883458f
// };

float Filtre3_den[5] = {
    1.0f,
    -3.823585096117956716454955312656238675117f,
    5.494063470698320905682976444950327277184f,
    -3.516551402123536007593429530970752239227f,
    0.84609627698785105920364912890363484621f
};

float Filtre4_den[3] = {
    1.0f,
    -1.804238045212939622885528478946071118116f,
    0.832365764220931558803329153306549414992f
};

float Filtre5_den[5] = {
    1.0f,
    -2.750778468418865863043265562737360596657f,
    3.012292635300034326917284488445147871971f,
    -1.609207979577763847345295289414934813976f,
    0.369378183537995952168131452708621509373f
};

//Déclaration des echantillons En pour le filtre et En_max pour trouver l'amplitude (hauteur de pile a atteindre)
// Initialise toutes les valeurs d'entrées à 0
float En_1[5] = { 0.0f };
float En_2[3] = { 0.0f };
float En_3[5] = { 0.0f };
float En_4[3] = { 0.0f };
float En_5[5] = { 0.0f };

//Echantillons 
float En_max_1 = 0.0f;
float En_max_2 = 0.0f;
float En_max_3 = 0.0f;
float En_max_4 = 0.0f;
float En_max_5 = 0.0f;
// s-********************************************************************************ties pour chaque filtre

// Initialise toutes les valeurs de sorties à 0
float Sn_1[5] = { 0.0f };
float Sn_2[3] = { 0.0f };
float Sn_3[5] = { 0.0f };
float Sn_4[3] = { 0.0f };
float Sn_5[5] = { 0.0f };

uint32_t g_CompteurFiltre = 0;
float g_SortieFiltre = 0.0f;
float g_SortieMax = 0.0f;

bool filtre1_timer(repeating_timer_t *t)
{
    uint adc_raw;
    float adcInput;

    g_CompteurFiltre++;

    adc_raw = adc_read();
    adcInput = (adc_raw * ADC_CONVERT) - LED_ADC_OFFSET;

    ///////////// FILTRE //////////////
    
    // Lecture du signal
    En_2[0] = adcInput;

    /// Calcul du filtre
    Sn_2[0] =
        (
            Filtre2_num[0] * En_2[0] +
            Filtre2_num[1] * En_2[1] +
            Filtre2_num[2] * En_2[2] //+
            //Filtre2_num[3] * En_1[3] +
            //Filtre2_num[4] * En_1[4]
        )-
          (  Filtre2_den[0] * Sn_2[1] +
            Filtre2_den[1] * Sn_2[2] //+
            //Filtre1_den[3] * Sn_1[3] +
            //Filtre1_den[4] * Sn_1[4]
          );

    // Réattribution des valeurs des échantillons selon le temps
    //Sn_1[4] = Sn_1[3];
    //Sn_1[3] = Sn_1[2];
    Sn_2[2] = Sn_2[1];
    Sn_2[1] = Sn_2[0];

    //En_2[4] = En_2[3];
    //En_2[3] = En_2[2];
    En_2[2] = En_2[1];
    En_2[1] = En_2[0];

    g_SortieFiltre = Sn_2[0];

    // Vérifie la valeur max de l'amplitude
    if((g_SortieFiltre > g_SortieMax)/*&&(g_CompteurFiltre>700)*/)
        g_SortieMax = g_SortieFiltre;

    return true;
}
