#ifndef STADE_H_INCLUDED
#define STADE_H_INCLUDED
#include "Batiment.h"
#include <iostream>
#include <string>

using namespace std;

class Ville; // Pré-déclaration

class Stade : public Batiment {
protected:
    int capacite;
    float plaisirEvenement;
    double EauInit;
    double ElecInit;

public:
    Stade(const string&,double,double,int);

    // Getters
    int getCapacite() const;

    // Méthodes principales
    float calculerRevenu(int participants, float prixTicket) const;
    void specialiteEvenement(Ville& v , int c,int);

    // Héritées
    void afficherDetails() override;
    double calculerImpactRessources() override;
    void lancerCycle(char s) override;
    void resetCycle() override;
};

#endif
