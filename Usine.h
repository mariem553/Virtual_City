#ifndef USINE_H_INCLUDED
#define USINE_H_INCLUDED
#ifndef USINE_H
#define USINE_H
#include "Batiment.h"
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

class Usine : public Batiment
{
protected:
    double productionBase;         // production fixe de base selon l'usine
    double productionRessources;   // production du cycle actuel
    double productionDernierCycle; // production du cycle précédent
    double pollution;              // pollution cumulée

public:
    // Constructeurs
    //Usine();
    Usine(const string &n, double eau, double elec, double prodBase);
    // Méthodes principales
    double produireRessources();
    double calculerPollution();
    double calculerImpactRessources();
     // Cycle
    void lancerCycle(char s) override;   // fonction saisonnière que tu voulais garder
    void resetCycle()override;
     // Affichage
    void afficherDetails();
    // Getters
    double getProduction() const;
    double getPollution() const;

    // Setters
    void setProduction(double p);
    void setPollution(double pol);

};

#endif



#endif // USINE_H_INCLUDED
