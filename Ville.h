#ifndef VILLE_H_INCLUDED
#define VILLE_H_INCLUDED
#include <vector>
#include <iostream>
#include "Batiment.h"
class Ville {
protected:
    const string  nom;
    double budget;
    double population;
    double satisfaction;
    double eauTotal;
    double electriciteTotal;
    vector <Batiment * > batiments; //liste des batiments
    double  eauInit;
    double elecInit ;
    double budgetInit ;

public:
    //Ville();
    Ville(const string& );
    //Méthodes
    void ajouterBatiment(Batiment* b);
    void supprimerBatiment(int id);
    void resetCycleVille() ;
    double calculerConsommationTotale() const;
    double calculerSatisfaction();
    double calculerTotalEau() const;
    double calculerTotalElec() const;
    void CalculerSatisfactionBat(double p);
    void majPopulation();
    void afficherVille();
    //getters
    double getEau() const;
    double getElectricite() const;
    double getSatisfaction() const;
    double getPopulation()const;
    const vector<Batiment*>& getBatiments() const;
    double getBudget();
    double geteauInit();
    double getelecInit();
    //setters
    void setElectricite(double q);
    void setEau(double q);
    void setSatisfaction(double s);
    void setBudget(double b);
    void setPopulation(double p);
    //destructor
    ~Ville();
};


#endif // VILLE_H_INCLUDED
