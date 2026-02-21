#include "Maison.h"

/*Maison::Maison()
    : Batiment(), capaciteHabitants(0), habitantsActuels(0) {}*/


//parametré
Maison::Maison(const string& nom, double eau, double elec, int capacite, int habi)
    : Batiment(nom, 'H', eau, elec),
      capaciteHabitants(capacite), habitantsActuels(habi) {} //type de batiment est habitation H

void Maison::ajouterHabitants(int nb) {
    habitantsActuels += nb;
    if (habitantsActuels > capaciteHabitants)
        habitantsActuels = capaciteHabitants;
}

void Maison::retirerHabitants(int nb) {
    habitantsActuels -= nb;
    if (habitantsActuels < 0)
        habitantsActuels = 0;
}

double Maison:: calculerImpactRessources()  {
 return consommationEau* habitantsActuels+ consommationElectricite*habitantsActuels;
}
void Maison::afficherDetails()  {
    Batiment::afficherDetails();
    cout << "Habitants : " << habitantsActuels << "/" << capaciteHabitants << endl;
}
int Maison::getHabitants() const {
    return habitantsActuels; }
int Maison::getCapacite() const {
    return capaciteHabitants; }
void Maison ::setHabitants(int h){
        habitantsActuels=h;}

void Maison::lancerCycle(char s){
    // Si la maison consomme peu → gain de satisfaction
    double consommation = 0.0;
    if (s == 'E') {          // Été
    consommation = 50;   // climatisation
    effetSatisfaction -= 10;
    }
    else if (s == 'H') {     // Hiver
    consommation = 40;   // chauffage
    effetSatisfaction -= 15;
    }
    else if (s == 'P') {     // Printemps
    effetSatisfaction += 10;
    }



    if (effetSatisfaction> 100) effetSatisfaction = 100;
    if (effetSatisfaction < 0)  effetSatisfaction = 0;


    // Application des modifications
    setEffetSatisfaction(effetSatisfaction);
    setConsommationElectricite(consommation);
    setConsommationEau(consommation);
}

  void Maison::resetCycle()
{
    // Migration naturelle
    if (getEffetSatisfaction() < 50 && habitantsActuels>0)
{
        // Perd au moins 1 habitant, ou 10% des habitants
        habitantsActuels -= max(1, habitantsActuels / 10);
    }

}


