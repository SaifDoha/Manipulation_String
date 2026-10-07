#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;
using namespace chrono;

bool chercherSousSequence(const string& mot2, const string& mot1){
    int j = 0;
    for (int i = 0; i < mot1.length(); i++){
        if (mot1[i] == mot2[j]){
            j++;
            if (j == mot2.length())
                return true;
        }
    }

    return false;
}

int compterMot(const string& mot, const vector<string>& mots){
    int compteur = 0;
    for (int i = 0; i < mots.size(); i++){
        if (mots[i] == mot){
            compteur++;
        }
    }
    return compteur;
}

bool find_mot(const string& mot, const vector<string>& mots){
    for (int i = 0; i < mots.size(); i++){
        if (mots[i] == mot){
            return true;
        }
    }
    return false;
}

bool find_mot2(const string& mot, const vector<string>& mots){
    return find(mots.begin(), mots.end(), mot) != mots.end();
}

// Chercher une sous-chaîne dans les mots

int compterSousChaine(const string& sousChaine, const vector<string>& mots){
    int compteur = 0;
    cout << "\nMots contenant \"" << sousChaine << "\" :" << endl;
    for (int i = 0; i < mots.size(); i++){
        if (mots[i].find(sousChaine) != string::npos){
            cout << "- " << mots[i] << endl;
            compteur++;
        }
    }
    return compteur;
}

template <typename Fonction> long long mesurerTemps(Fonction fonction){
    auto debut = high_resolution_clock::now();
    fonction();
    auto fin = high_resolution_clock::now();
    return duration_cast<nanoseconds>(fin - debut).count();
}

void afficherTemps(long long temps){
    cout << "Temps : "
         << temps << " ns"
         << " | "
         << fixed << setprecision(6)
         << temps / 1000000.0 << " ms"
         << " | "
         << temps / 1000000000.0 << " s"
         << endl;
}

int remplacerSousChaine(const string& sousChaine,const string& nouvelleSousChaine,vector<string>& mots){
    int compteur = 0;
    for (int i = 0; i < mots.size(); i++){
        if (mots[i].find(sousChaine) != string::npos){
            cout << "Avant : " << mots[i] <<" ";
           size_t position = mots[i].find(sousChaine);
            while (position != string::npos){
                mots[i].replace(
                    position,
                    sousChaine.length(),
                    nouvelleSousChaine
                );
                position = mots[i].find(
                    sousChaine,
                    position + nouvelleSousChaine.length()
                );
            }
            cout << "Apres : " << mots[i] << endl;
            compteur++;
        }
    }
    return compteur;
}

int main(){
    ifstream fichier("mots.txt");
    if (!fichier){
        cout << "Erreur : impossible d'ouvrir mots.txt" << endl;
        return 1;
    }

    vector<string> mots;
    string mot;
    while (fichier >> mot){
        mots.push_back(mot);
    }
    fichier.close();

    string mot1, mot2, sousChaine;

    cout << "Entrer le mot 1 : ";
    cin >> mot1;

    cout << "Entrer le mot 2 : ";
    cin >> mot2;

    cout << "Entrer la sous-chaine a chercher : ";
    cin >> sousChaine;

    cout << "\n========== EXERCICE 1 ==========" << endl;

    bool resultat1;
    long long temps1 = mesurerTemps([&](){
        resultat1 = chercherSousSequence(mot2, mot1);
    });

    if (resultat1)
        cout << mot2 << " se trouve dans " << mot1 << endl;
    else
        cout << mot2 << " est introuvable dans " << mot1 << endl;

    afficherTemps(temps1);

    cout << "\n========== EXERCICE 2 ==========" << endl;
    int resultat2;
    long long temps2 = mesurerTemps([&](){
        resultat2 = compterMot(mot1, mots);
    });
    if (resultat2 > 0){
        cout << "Mot trouve." << endl;
        cout << "Il existe " << resultat2 << " fois." << endl;
    }
    else{
        cout << "Mot introuvable." << endl;
    }
    afficherTemps(temps2);


    cout << "\n========== FIND_MOT ==========" << endl;
    bool resultat3;
    long long temps3 = mesurerTemps([&](){
        resultat3 = find_mot(mot1, mots);
    });
    if (resultat3)
        cout << "Mot trouve." << endl;
    else
        cout << "Mot introuvable." << endl;
    afficherTemps(temps3);

    cout << "\n========== FIND_MOT2 (std::find) ==========" << endl;
    bool resultat4;
    long long temps4 = mesurerTemps([&](){
        resultat4 = find_mot2(mot1, mots);
    });
    if (resultat4)
        cout << "Mot trouve." << endl;
    else
        cout << "Mot introuvable." << endl;

    afficherTemps(temps4);

    // ==================================================
    // EXERCICE 5 : recherche d'une sous-chaîne
    // ==================================================

    cout << "\n========== SOUS-CHAINE ==========" << endl;

    int resultat5;
    string nouvelleSousChaine;
    cout << "Entrer la nouvelle sous-chaine : ";
    cin >> nouvelleSousChaine;

    long long temps5 = mesurerTemps([&](){
        resultat5 = remplacerSousChaine(sousChaine,nouvelleSousChaine,mots);
    });

    cout << "Nombre de mots modifies : " << resultat5 << endl;

    afficherTemps(temps5);

    // Réécrire le fichier avec les nouvelles valeurs
   ofstream fichierSortie("mots.txt", ios::trunc);
    if (!fichierSortie){
        cout << "Erreur : impossible d'ouvrir mots.txt pour l'ecriture." << endl;
        return 1;
    }

    for (const string& mot : mots){
        fichierSortie << mot << '\n';
    }
    fichierSortie.close();
    cout << "Fichier mots.txt mis a jour !" << endl;


        return 0;
}
