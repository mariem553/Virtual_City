#ifndef PARC_H_INCLUDED
#define PARC_H_INCLUDED
#include "Batiment.h"
#include<iostream>
#include<string>
class Parc : public Batiment {
protected:
    double surface;
    double effetBienEtre;

public:
    //Parc();
    Parc(const string &, double , double , double);
    //Méthodes
    void ameliorerBienEtre( double ) ;
    double calculerImpactRessources() override;
    void afficherDetails() override;
    void lancerCycle(char s) override;
    //getters
    double getSurface() const;
    double getEffetBienEtre() const;
    //setters
    void setEffetBienEtre(double e);
};

#endif // PARC_H_INCLUDED
