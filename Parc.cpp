#include "Parc.h"
#include "Ville.h"
using namespace std;
/*Parc::Parc()
    : Batiment(), surface(0.0), effetBienEtre(0.0) { }*/

Parc::Parc(const string &n, double eau, double elec, double su)
    : Batiment(n, 'P', eau, elec),
      surface(su) {effetBienEtre=su*50;}

// permet d’augmenter ou diminuer le bien-être du parc
void Parc::ameliorerBienEtre(double p) {
    effetBienEtre += p;
    cout << "L effet bien etre change de" << p<< endl;

    if (effetBienEtre>10) effetBienEtre=10;
    if(effetBienEtre<0) effetBienEtre=0;
}

// Cycle saisonnier : modifie la satisfaction selon la saison
void Parc::lancerCycle(char s)
{
    if (s == 'P') {        // Printemps
        effetSatisfaction += 20;
    }
    else if (s == 'E') {   // Été
        effetSatisfaction += 10;
    }
    else if (s == 'A') {   // Automne
        effetSatisfaction -= 5;
    }
    else if (s == 'H') {   // Hiver
        effetSatisfaction -= 20; // parc vide et froid
    }


    if (effetSatisfaction > 100) effetSatisfaction = 100;

}
//no resetcycle car on n'a rien pour reinitialise

double Parc::getSurface() const {
    return surface;
}

double Parc::getEffetBienEtre() const {
    return effetBienEtre;
}
void Parc::setEffetBienEtre(double e){
    effetBienEtre=e;

}
double Parc::calculerImpactRessources() {
    return consommationEau + consommationElectricite; // hérité de Batiment
}
void Parc::afficherDetails() {
    Batiment::afficherDetails();
    cout << "Surface : " << surface << " km2 "<<endl;
    cout << "Bien-etre : +" << effetBienEtre << endl;
}
