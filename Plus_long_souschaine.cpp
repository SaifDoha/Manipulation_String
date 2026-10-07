#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace chrono;

void afficherTailleSousChaines(
    const unordered_map<string, unordered_set<size_t>>& sousChaines)
{
    cout << "Nombre de sous-chaines stockees : "
         << sousChaines.size()
         << endl;
}

vector<string> chargerMots(const string& nomFichier){
    vector<string> mots;
    ifstream fichier(nomFichier);
    if (!fichier){
        cerr << "Erreur : impossible d'ouvrir le fichier "
             << nomFichier << endl;
        return mots;
    }
    string mot;

    while (fichier >> mot){
        mots.push_back(mot);
    }

    fichier.close();
    return mots;
}

// Fonction : trouver la plus grande sous-chaîne commune

string trouverPlusGrandeCommune(
    const vector<string>& mots,
    vector<string>& motsConcernes,
    size_t& nombreSousChaines)
{
    unordered_map<string, unordered_set<size_t>> sousChaines;

    // Étape 1 : générer les sous-chaînes de chaque mot
    for (size_t i = 0; i < mots.size(); i++){
        const string& mot = mots[i];

        unordered_set<string> sousChainesDuMot;

        for (size_t debut = 0; debut < mot.length(); debut++)
        {
            for (size_t longueur = 1;
                 debut + longueur <= mot.length();
                 longueur++)
            {
                string sousChaine =
                    mot.substr(debut, longueur);

                sousChainesDuMot.insert(sousChaine);
            }
        }

        // Étape 2 : associer chaque sous-chaîne au mot
        for (const string& sousChaine : sousChainesDuMot)
        {
            sousChaines[sousChaine].insert(i);
        }
    }

    // Nombre de sous-chaînes distinctes stockées
    nombreSousChaines = sousChaines.size();

    // Étape 3 : rechercher la plus grande sous-chaîne
    string plusGrande = "";

    for (const auto& element : sousChaines){
        const string& sousChaine = element.first;
        const unordered_set<size_t>& motsAvecSousChaine =
            element.second;

        if (motsAvecSousChaine.size() >= 2){
            if (sousChaine.length() > plusGrande.length()){
                plusGrande = sousChaine;
            }
        }
    }

    // Étape 4 : trouver les mots concernés
    if (!plusGrande.empty()){
        for (size_t i = 0; i < mots.size(); i++){
            if (mots[i].find(plusGrande) != string::npos){
                motsConcernes.push_back(mots[i]);
            }
        }
    }
    return plusGrande;
}

template <typename Fonction>long long mesurerTemps(Fonction fonction){
    auto debut = high_resolution_clock::now();
    fonction();
    auto fin = high_resolution_clock::now();
    return duration_cast<nanoseconds>(
        fin - debut
    ).count();
}

void afficherTemps(long long tempsNanosecondes){
    double tempsMillisecondes =
        tempsNanosecondes / 1'000'000.0;

    double tempsSecondes =
        tempsNanosecondes / 1'000'000'000.0;

    cout << fixed << setprecision(6);
    cout << "\nTemps d'execution :" << endl;
    cout << "  Nanosecondes : "
         << tempsNanosecondes
         << " ns" << endl;

    cout << "  Millisecondes : "
         << tempsMillisecondes
         << " ms" << endl;

    cout << "  Secondes : "
         << tempsSecondes
         << " s" << endl;
}


int main(){
    string nomFichier = "mots.txt";
    vector<string> mots =chargerMots(nomFichier);
    // Vérifier si le fichier contient des mots
    if (mots.empty()){
        cout << "Aucun mot trouve dans le fichier."
             << endl;
        return 1;
    }
    cout << " RECHERCHE DE LA PLUS GRANDE"
         << endl;

    cout << " SOUS-CHAINE COMMUNE"
         << endl;

    cout << "========================================"
         << endl;

    cout << "\nNombre de mots : "
         << mots.size()
         << endl;

    vector<string> motsConcernes;
    string plusGrande;
    size_t nombreSousChaines = 0;
    long long temps =
        mesurerTemps([&](){
            plusGrande =trouverPlusGrandeCommune(mots,motsConcernes,nombreSousChaines);
        });

    cout << "\n----------------------------------------"
         << endl;

    if (!plusGrande.empty())
    {
        cout << "Plus grande sous-chaine commune : "
             << plusGrande
             << endl;

        cout << "Longueur : "
             << plusGrande.length()
             << endl;


        cout << "\nMots contenant cette sous-chaine :"
             << endl;

        for (const string& mot : motsConcernes)
        {
            cout << "- " << mot << endl;
        }
    }
    else
    {
        cout << "Aucune sous-chaine commune "
             << "dans au moins deux mots."
             << endl;
    }
    cout << "\n----------------------------------------"
         << endl;

    afficherTemps(temps);


    cout << "\n========================================"
         << endl;


    return 0;
}

