#ifndef SIMULATION_H_INCLUDED
#define SIMULATION_H_INCLUDED

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "Ville.h"
#include "Batiment.h"
#include "Usine.h"
#include "Maison.h"
#include "Parc.h"
#include "Ecole.h"
#include "Stade.h"

using namespace std;

class Simulation {
protected:
    Ville* maville;          // La ville qui contient tous les bâtiments
    char saisonCourante;     // Saison actuelle

public:
    Simulation(Ville* v);
    //Methodes
    void changerSaison(char s);
    void lancerCycleGeneral();
    void resetCycleGeneral();
    void declencherEvenement(int c);
    //getters
    char getSaison();
};

#endif // SIMULATION_H_INCLUDED
