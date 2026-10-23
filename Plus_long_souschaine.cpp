#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>   // sort, unique
#include <chrono>      // mesure du temps
#include <iomanip>     // setprecision

using namespace std;
using namespace chrono;

vector<string> chargerMots(const string& nomFichier) {
    vector<string> mots;
    ifstream fichier(nomFichier);
    if (!fichier) {
        cerr << "Erreur : impossible d'ouvrir le fichier "
             << nomFichier << endl;
        return mots;
    }
    string mot;
    while (fichier >> mot) {
        mots.push_back(mot);
    }
    fichier.close();
    return mots;
}

template <typename Fonction> long long mesurerTemps(Fonction fonction) {
    auto debut = high_resolution_clock::now();
    fonction();
    auto fin = high_resolution_clock::now();
    return duration_cast<nanoseconds>(fin - debut).count();
}

void afficherTemps(long long tempsNanosecondes) {
    cout << fixed << setprecision(6);

    cout << "Nanosecondes  : "
         << tempsNanosecondes << " ns" << endl;

    cout << "Millisecondes : "
         << tempsNanosecondes / 1'000'000.0 << " ms" << endl;

    cout << "Secondes      : "
         << tempsNanosecondes / 1'000'000'000.0 << " s" << endl;
}

struct SousChaine {
    string texte; //la sous-chaine elle-meme
    size_t longueur;//sa longueur
    size_t nombreOccurrences; //le nombre de MOTS DIFFERENTS qui la contiennent
};


// =====================================================================
//                      TRAITEMENT 1 : VECTOR
// =====================================================================
vector<string> trouverPlusGrandeCommuneVector(const vector<string>& mots,vector<string>& motsConcernes,size_t& nombreSousChaines){
    motsConcernes.clear();
    vector<SousChaine> sousChaines; //toutes les sous-chaines distinctes de tous les mots

    for (const string& mot : mots) {

        // Sous-chaines distinctes de CE mot (liste temporaire)
        vector<SousChaine> sousChainesDuMot;

        for (size_t debut = 0; debut < mot.length(); debut++) {
            for (size_t longueur = 1;debut + longueur <= mot.length();longueur++) {
                string s = mot.substr(debut, longueur);

                // Parcours complet de la liste du mot pour detecter
                bool existe = false;
                for (const SousChaine& ancienne : sousChainesDuMot) {
                    if (ancienne.texte == s) {
                        existe = true;
                        break;
                    }
                }
                if (!existe) {
                    sousChainesDuMot.push_back({s, longueur, 0});
                }
            }
        }

        //mettre a jour la liste globale
        for (const SousChaine& s : sousChainesDuMot) {
            // Recherche sequentielle depuis le debut de la liste globale
            size_t position = 0;
            while (position < sousChaines.size() &&
                   sousChaines[position].texte != s.texte) {
                position++;
            }

            if (position == sousChaines.size()) {
                // Pas trouvee : nouvelle sous-chaine, compteur a 1
                sousChaines.push_back({s.texte, s.longueur, 1});
            } else {
                // Deja presente : on incremente seulement le compteur
                sousChaines[position].nombreOccurrences++;
            }
        }
    }

    nombreSousChaines = sousChaines.size();
    // ETAPE 3 : conserver TOUTES les sous-chaines communes de longueur maximale
    vector<string> plusGrandes;
    size_t longueurMax = 0;

    for (const SousChaine& s : sousChaines) {
        if (s.nombreOccurrences >= 2) {
            if (s.longueur > longueurMax) {
                plusGrandes.clear();
                plusGrandes.push_back(s.texte);
                longueurMax = s.longueur;
            } else if (s.longueur == longueurMax) {
                plusGrandes.push_back(s.texte);
            }
        }
    }

    // ETAPE 4 : retrouver les mots qui contiennent au moins un resultat
    motsConcernes.clear();
    for (const string& mot : mots) {
        for (const string& sousChaine : plusGrandes) {
            if (mot.find(sousChaine) != string::npos) {
                if (find(motsConcernes.begin(), motsConcernes.end(), mot)
                    == motsConcernes.end()) {
                    motsConcernes.push_back(mot);
                }
                break;
            }
        }
    }

    sort(plusGrandes.begin(), plusGrandes.end());
    return plusGrandes;
}

// =====================================================================
//                  TRAITEMENT 2 : LISTE ORDONNEE
// =====================================================================

vector<string> trouverPlusGrandeCommuneListeOrdonnee(
    const vector<string>& mots,
    vector<string>& motsConcernes,
    size_t& nombreSousChaines)
{
    // Liste globale triee : une entree par sous-chaine distincte
    list<SousChaine> sousChaines;

    for (const string& mot : mots) {

        // ETAPE 1 : generer toutes les sous-chaines
        // La liste est maintenue dans l'ordre alphabetique
        list<string> sousChainesDuMot;

        for (size_t debut = 0; debut < mot.length(); debut++) {
            for (size_t longueur = 1;
                 debut + longueur <= mot.length();
                 longueur++) {

                string s = mot.substr(debut, longueur);

                // Trouver la position d'insertion
                auto it = sousChainesDuMot.begin();

                while (it != sousChainesDuMot.end()
                       && *it < s) {
                    ++it;
                }

                // Inserer seulement si elle n'existe pas deja
                if (it == sousChainesDuMot.end() || *it != s) {
                    sousChainesDuMot.insert(it, s);
                }
            }
        }

        // ETAPE 2 : fusionner avec la liste globale ordonnee
        for (const string& s : sousChainesDuMot) {

            auto it = sousChaines.begin();

            while (it != sousChaines.end()
                   && it->texte < s) {
                ++it;
            }

            if (it != sousChaines.end()
                && it->texte == s) {

                // Sous-chaine deja presente dans un autre mot
                it->nombreOccurrences++;

            } else {

                // Nouvelle sous-chaine, insertion ordonnee
                sousChaines.insert(
                    it, {s, s.length(), 1});
            }
        }
    }

    nombreSousChaines = sousChaines.size();
    // ETAPE 3 : conserver TOUTES les sous-chaines communes de longueur maximale
    vector<string> plusGrandes;
    size_t longueurMax = 0;

    for (const SousChaine& s : sousChaines) {
        if (s.nombreOccurrences >= 2) {
            if (s.longueur > longueurMax) {
                plusGrandes.clear();
                plusGrandes.push_back(s.texte);
                longueurMax = s.longueur;
            } else if (s.longueur == longueurMax) {
                plusGrandes.push_back(s.texte);
            }
        }
    }

    // ETAPE 4 : retrouver les mots qui contiennent au moins un resultat
    motsConcernes.clear();
    for (const string& mot : mots) {
        for (const string& sousChaine : plusGrandes) {
            if (mot.find(sousChaine) != string::npos) {
                if (find(motsConcernes.begin(), motsConcernes.end(), mot)
                    == motsConcernes.end()) {
                    motsConcernes.push_back(mot);
                }
                break;
            }
        }
    }

    sort(plusGrandes.begin(), plusGrandes.end());
    return plusGrandes;
}


// =====================================================================
//  TRAITEMENT 3 : SET : Maintient les éléments triés et refuse les doublons.
// =====================================================================

struct ComparerSousChaine {
    bool operator()(const SousChaine& a,const SousChaine& b) const {
        return a.texte < b.texte;
    }
};

vector<string> trouverPlusGrandeCommuneSet(
    const vector<string>& mots,
    vector<string>& motsConcernes,
    size_t& nombreSousChaines)
{
    set<SousChaine, ComparerSousChaine> sousChaines;

    for (const string& mot : mots) {

        // 1. Generer les sous-chaines distinctes du mot
        set<string> sousChainesDuMot;

        for (size_t debut = 0; debut < mot.length(); debut++) {
            for (size_t longueur = 1;
                 debut + longueur <= mot.length();
                 longueur++) {

                sousChainesDuMot.insert(
                    mot.substr(debut, longueur));
            }
        }

        // 2. Mettre a jour les occurrences globales
        for (const string& s : sousChainesDuMot) {

            auto it = sousChaines.find({s, s.length(), 0});

            if (it != sousChaines.end()) {

                // Sous-chaine deja presente
                SousChaine maj = *it;
                sousChaines.erase(it);

                maj.nombreOccurrences++;
                sousChaines.insert(maj);

            } else {

                // Nouvelle sous-chaine
                sousChaines.insert({s, s.length(), 1});
            }
        }
    }

    nombreSousChaines = sousChaines.size();
    // ETAPE 3 : conserver TOUTES les sous-chaines communes de longueur maximale
    vector<string> plusGrandes;
    size_t longueurMax = 0;

    for (const SousChaine& s : sousChaines) {
        if (s.nombreOccurrences >= 2) {
            if (s.longueur > longueurMax) {
                plusGrandes.clear();
                plusGrandes.push_back(s.texte);
                longueurMax = s.longueur;
            } else if (s.longueur == longueurMax) {
                plusGrandes.push_back(s.texte);
            }
        }
    }

    // ETAPE 4 : retrouver les mots qui contiennent au moins un resultat
    motsConcernes.clear();
    for (const string& mot : mots) {
        for (const string& sousChaine : plusGrandes) {
            if (mot.find(sousChaine) != string::npos) {
                if (find(motsConcernes.begin(), motsConcernes.end(), mot)
                    == motsConcernes.end()) {
                    motsConcernes.push_back(mot);
                }
                break;
            }
        }
    }

    sort(plusGrandes.begin(), plusGrandes.end());
    return plusGrandes;
}


// =====================================================================
//  TRAITEMENT 4 : unordered_map table de hachage : Associe chaque sous-chaîne à son compteur
// =====================================================================


vector<string> trouverPlusGrandeCommuneUnorderedMap(const vector<string>& mots,vector<string>& motsConcernes,size_t& nombreSousChaines)
{
    // Cle : sous-chaine
    // Valeur : nombre de mots contenant cette sous-chaine
    unordered_map<string, size_t> occurrences;

    for (const string& mot : mots) {

        // 1. Generer les sous-chaines distinctes du mot
        unordered_set<string> sousChainesDuMot;

        for (size_t debut = 0; debut < mot.length(); debut++) {
            for (size_t longueur = 1;
                 debut + longueur <= mot.length();
                 longueur++) {

                sousChainesDuMot.insert(
                    mot.substr(debut, longueur));
            }
        }

        // 2. Mettre a jour les compteurs globaux
        for (const string& s : sousChainesDuMot) {
            occurrences[s]++;
        }
    }

    nombreSousChaines = occurrences.size();
    // 3. Conserver toutes les sous-chaines communes de longueur maximale
    vector<string> plusGrandes;
    size_t longueurMax = 0;

    for (const auto& element : occurrences) {
        const string& sousChaine = element.first;
        size_t nombre = element.second;

        if (nombre >= 2) {
            if (sousChaine.length() > longueurMax) {
                plusGrandes.clear();
                plusGrandes.push_back(sousChaine);
                longueurMax = sousChaine.length();
            } else if (sousChaine.length() == longueurMax) {
                plusGrandes.push_back(sousChaine);
            }
        }
    }

    // 4. Recuperer les mots contenant au moins un resultat
    motsConcernes.clear();
    for (const string& mot : mots) {
        for (const string& sousChaine : plusGrandes) {
            if (mot.find(sousChaine) != string::npos) {
                if (find(motsConcernes.begin(), motsConcernes.end(), mot)
                    == motsConcernes.end()) {
                    motsConcernes.push_back(mot);
                }
                break;
            }
        }
    }

    sort(plusGrandes.begin(), plusGrandes.end());
    return plusGrandes;
}

// =====================================================================
// 7. AFFICHAGE DES RESULTATS D'UN TRAITEMENT
// =====================================================================

void afficherResultats(
    const string& titre,
    const vector<string>& plusGrandes,
    const vector<string>& motsConcernes,
    size_t nombreSousChaines,
    long long temps)
{
    cout << "\n========================================" << endl;
    cout << titre << endl;
    cout << "========================================" << endl;

    cout << "Nombre de sous-chaines distinctes : "
         << nombreSousChaines << endl;

    if (!plusGrandes.empty()) {
        cout << "Plus grande(s) sous-chaine(s) commune(s) :" << endl;
        for (const string& sousChaine : plusGrandes) {
            cout << "- " << sousChaine << endl;
        }

        cout << "Longueur maximale : " << plusGrandes[0].length() << endl;

        cout << "\nMots concernes :" << endl;
        for (const string& mot : motsConcernes) {
            cout << "- " << mot << endl;
        }
    } else {
        cout << "Aucune sous-chaine commune dans au moins "
             << "deux mots." << endl;
    }

    cout << "\nTemps d'execution :" << endl;
    afficherTemps(temps);
}

int main() {
    vector<string> mots = chargerMots("mot1.txt");

    if (mots.empty()) {
        cout << "Aucun mot charge depuis le fichier mot1.txt." << endl;
        return 1;
    }

    cout << "==============================================" << endl;
    cout << " COMPARAISON DES STRUCTURES DE DONNEES" << endl;
    cout << "==============================================" << endl;
    cout << "Nombre de mots charges : " << mots.size() << endl;

    // Variables pour les resultats
    vector<string> resultatVector;
    vector<string> resultatListe;
    vector<string> resultatSet;
    vector<string> resultatHash;

    vector<string> motsConcernesVector;
    vector<string> motsConcernesListe;
    vector<string> motsConcernesSet;
    vector<string> motsConcernesHash;

    size_t nombreSousChainesVector = 0;
    size_t nombreSousChainesListe = 0;
    size_t nombreSousChainesSet = 0;
    size_t nombreSousChainesHash = 0;

    // Variables pour mesurer les temps
    long long tempsVector;
    long long tempsListe;
    long long tempsSet;
    long long tempsHash;

    // Traitement 1 : VECTOR
    cout << "\n==============================================" << endl;
    cout << "TRAITEMENT 1 : VECTOR" << endl;
    cout << "==============================================" << endl;

    tempsVector = mesurerTemps([&]() {
        resultatVector = trouverPlusGrandeCommuneVector(
            mots,
            motsConcernesVector,
            nombreSousChainesVector
        );
    });

    afficherTemps(tempsVector);

    cout << "Nombre de sous-chaines distinctes : "
         << nombreSousChainesVector << endl;
    cout << "Plus grande(s) sous-chaine(s) commune(s) :" << endl;
    if (resultatVector.empty()) {
        cout << "Aucune" << endl;
    } else {
        for (const string& sousChaine : resultatVector) {
            cout << "- " << sousChaine << endl;
        }
        cout << "Longueur maximale : " << resultatVector[0].length() << endl;
    }
    cout << "Nombre de mots concernes : "
         << motsConcernesVector.size() << endl;

    // Traitement 2 : LIST ORDONNEE
    cout << "\n==============================================" << endl;
    cout << "TRAITEMENT 2 : LIST ORDONNEE" << endl;
    cout << "==============================================" << endl;

    tempsListe = mesurerTemps([&]() {
        resultatListe = trouverPlusGrandeCommuneListeOrdonnee(
            mots,
            motsConcernesListe,
            nombreSousChainesListe
        );
    });

    afficherTemps(tempsListe);

    cout << "Nombre de sous-chaines distinctes : "
         << nombreSousChainesListe << endl;
    cout << "Plus grande(s) sous-chaine(s) commune(s) :" << endl;
    if (resultatListe.empty()) {
        cout << "Aucune" << endl;
    } else {
        for (const string& sousChaine : resultatListe) {
            cout << "- " << sousChaine << endl;
        }
        cout << "Longueur maximale : " << resultatListe[0].length() << endl;
    }
    cout << "Nombre de mots concernes : "
         << motsConcernesListe.size() << endl;

    // Traitement 3 : SET ORDONNE
    cout << "\n==============================================" << endl;
    cout << "TRAITEMENT 3 : SET ORDONNE" << endl;
    cout << "==============================================" << endl;

    tempsSet = mesurerTemps([&]() {
        resultatSet = trouverPlusGrandeCommuneSet(
            mots,
            motsConcernesSet,
            nombreSousChainesSet
        );
    });

    afficherTemps(tempsSet);

    cout << "Nombre de sous-chaines distinctes : "
         << nombreSousChainesSet << endl;
    cout << "Plus grande(s) sous-chaine(s) commune(s) :" << endl;
    if (resultatSet.empty()) {
        cout << "Aucune" << endl;
    } else {
        for (const string& sousChaine : resultatSet) {
            cout << "- " << sousChaine << endl;
        }
        cout << "Longueur maximale : " << resultatSet[0].length() << endl;
    }
    cout << "Nombre de mots concernes : "
         << motsConcernesSet.size() << endl;

    // Traitement 4 : UNORDERED_MAP
    cout << "\n==============================================" << endl;
    cout << "TRAITEMENT 4 : UNORDERED_MAP" << endl;
    cout << "==============================================" << endl;

    tempsHash = mesurerTemps([&]() {
        resultatHash = trouverPlusGrandeCommuneUnorderedMap(
            mots,
            motsConcernesHash,
            nombreSousChainesHash
        );
    });

    afficherTemps(tempsHash);

    cout << "Nombre de sous-chaines distinctes : "
         << nombreSousChainesHash << endl;
    cout << "Plus grande(s) sous-chaine(s) commune(s) :" << endl;
    if (resultatHash.empty()) {
        cout << "Aucune" << endl;
    } else {
        for (const string& sousChaine : resultatHash) {
            cout << "- " << sousChaine << endl;
        }
        cout << "Longueur maximale : " << resultatHash[0].length() << endl;
    }
    cout << "Nombre de mots concernes : "
         << motsConcernesHash.size() << endl;

    // Comparaison des temps d'execution
    cout << "\n==============================================" << endl;
    cout << "COMPARAISON DES TEMPS D'EXECUTION" << endl;
    cout << "==============================================" << endl;

    cout << left << setw(25) << "Structure"
         << right << setw(20) << "Temps (ns)" << endl;
    cout << string(45, '-') << endl;

    cout << left << setw(25) << "Vector"
         << right << setw(20) << tempsVector << endl;

    cout << left << setw(25) << "List ordonnee"
         << right << setw(20) << tempsListe << endl;

    cout << left << setw(25) << "Set ordonne"
         << right << setw(20) << tempsSet << endl;

    cout << left << setw(25) << "Unordered_map"
         << right << setw(20) << tempsHash << endl;

    cout << "==============================================" << endl;

    return 0;
}
