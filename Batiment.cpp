#include "batiment.h"
int Batiment:: compteurId=0;
/*Batiment::Batiment()
: id(++compteurId), nom("inconnu"), type('I'), consommationEau(0.0),
      consommationElectricite(0.0), effetSatisfaction(0.0) {}*/

//PARAMETRé
Batiment::Batiment(const string &nom, char type, double eau, double elec): id(++compteurId), nom(nom), type(type),
      consommationEau(eau), consommationElectricite(elec),
      effetSatisfaction(100.0) {}

void  Batiment::afficherDetails() {
    cout << "Batiment #" << id << " - " << nom << " (Type " << type << ") "<< " \nsatisafaction " << effetSatisfaction << "\nconsommation eau "<< consommationEau<< "\nconsommation electricite" << consommationElectricite<<"\n";
}
void Batiment::setConsommationEau(double e){
        consommationEau+=e;
}

const string & Batiment:: getNom() const {return nom;}
int Batiment::getId() const { return id; }
char Batiment::getType() const { return type; }
double Batiment::getEffetSatisfaction() const {return effetSatisfaction;}
double Batiment::getConsommationEau() const {return consommationEau; }
double Batiment::getConsommationElectricite() const {return consommationElectricite; }
void Batiment:: setConsommationElectricite(double c) {consommationElectricite+= c;}
void  Batiment:: setEffetSatisfaction(double e) {effetSatisfaction=e;}
