#ifndef MAISON_H_INCLUDED
#define MAISON_H_INCLUDED
#include<iostream>
#include<string>
#include "Batiment.h"
class Maison : public Batiment {
protected:
    int capaciteHabitants;
    int habitantsActuels;
public:
    //Maison();
    Maison(const string &, double, double, int, int);
    //Méthodes
    void ajouterHabitants(int);
    void retirerHabitants(int);
    void afficherDetails() override;
    double calculerImpactRessources() override;
    void lancerCycle(char s) override;
    void resetCycle()override;
    //getters
    int getHabitants()const;
    int getCapacite()const;
    //setters
    void setHabitants(int h);
};

#endif // MAISON_H_INCLUDED
