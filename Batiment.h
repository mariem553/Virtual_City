#ifndef BATIMENT_H_INCLUDED
#define BATIMENT_H_INCLUDED
#include <string>
#include <iostream>
#include "raylib.h"
using namespace std;
class Batiment {
protected:
    int id;
    const string nom;
    char type; //H = Habitation, U = Usine, P = Parc...
    double consommationEau;
    double consommationElectricite;
    double effetSatisfaction;
    static int compteurId;
   Vector3 pos;
public:
    //Batiment();
    Batiment(const string &, char , double , double  );
    //Méthodes
    virtual void afficherDetails() ;
    virtual double calculerImpactRessources()=0;
    virtual void resetCycle(){};
    virtual void lancerCycle(char s){}
    //getters
    int getId() const ;
    char getType() const ;
    const string & getNom() const;
    double getEffetSatisfaction() const;
    double getConsommationEau() const;
    double getConsommationElectricite() const;
    //setters
    void setEffetSatisfaction(double );
    void setConsommationEau(double e);
    void setConsommationElectricite(double);
    //decustrutor
    virtual ~Batiment(){}
    Vector3 getPosition() const { return pos; }
    void setPosition(Vector3 p) { pos = p; }
};


#endif // BATIMENT_H_INCLUDED
