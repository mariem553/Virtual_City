#include "Ecole.h"
#include"Batiment.h"
using namespace std;

// Constructeurs


Ecole::Ecole(const string &nom, double eau, double elec, int capacite,int enfantsActuel,int enseignants)
    : Batiment(nom, 'E', eau, elec),
      capaciteEnfants(capacite),
      enfantsActuels(enfantsActuel),
      enseignants(enseignants)

    {  baseEau= eau;     // Sauvegarde de la consommation d'eau de base pour reset
       baseElec =elec;   // Sauvegarde de la consommation d'électricité de base pour reset
        // Calcul de l'effet éducatif : proportionnel à la capacité
        effetEducation=capacite*0.25;
 }


// Lancer un cycle
void Ecole::lancerCycle(char s) {
    // Impact sur la satisfaction AVANT variation saison
    effetSatisfaction += effetEducation + (enfantsActuels * 0.01) + (enseignants * 0.05);

    if (effetSatisfaction>100) effetSatisfaction=100;

    // Variation saisonnière
    if (s == 'H') {
        consommationElectricite *= 1.1;  // +10% électricité (chauffage)
        effetSatisfaction-=10;      // Baisse de satisfaction due au froid
        effetEducation -= 2;
    }
    else if (s == 'E') {
        consommationElectricite *= 1.15;       // +15% électricité (climatisation)
    }
    }
// Reset cycle
void Ecole::resetCycle() {

    consommationEau = baseEau;              // Retour à la consommation d'eau initiale
    consommationElectricite = baseElec;    // Retour à la consommation d'électricité initiale
}

// Affichage des détails

void Ecole::afficherDetails() {
    cout << "Batiment #" << id << " - " << nom << " (Type E)\n";
    cout << "Satisfaction : " << effetSatisfaction << "%\n";
    cout << "Consommation eau : " << consommationEau << "\n";
    cout << "Consommation electricite : " << consommationElectricite << "\n";
    cout << "Enfants : " << enfantsActuels << "/" << capaciteEnfants << "\n";
    cout << "Enseignants : " << enseignants << "\n";
    if (effetEducation>10) effetEducation=10;
    cout << "Effet education : +" << effetEducation << "\n";
}
int Ecole:: getEnfants()  { return enfantsActuels; }
int Ecole:: getEnseignants(){ return enseignants; }
int Ecole::getCapacite()  { return capaciteEnfants; }

double Ecole::calculerImpactRessources() {
    return consommationEau + consommationElectricite;
}
double Ecole::getEducation(){return effetEducation;}

void Ecole:: SetEducation(double e){
    effetEducation+=e;
}
