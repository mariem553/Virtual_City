#ifndef ECOLE_H_INCLUDED
#define ECOLE_H_INCLUDED
#include <iostream>
#include "Batiment.h"
#include <string>
using namespace std;

class Ecole : public Batiment {
protected:
    int capaciteEnfants;
    int enfantsActuels;
    int enseignants;
    double effetEducation;  // bonus satisfaction
    double baseEau;
    double baseElec ;

public:
    // Constructeurs
    Ecole(const string &nom, double eau, double elec, int capacite,int ,int);

    // methodes
    void accueillirEnfants(int n);
    void accueillirEnseignants(int n);
    double calculerImpactRessources();
    void afficherDetails() override;
    void lancerCycle(char s) override;
    void resetCycle() override;
    // Getters
    int  getEnfants();
    int  getEnseignants();
    int  getCapacite();
    double getEducation();
    double getEffetEducation() const { return effetEducation; }
    //setters
    void SetEducation(double e);

};

#endif // ECOLE_H_INCLUDED
