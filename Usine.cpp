#include "Usine.h"
#include "Batiment.h"

using namespace std;
/*Usine::Usine()
    : Batiment(),
      productionBase(100),
      productionRessources(0),
      productionDernierCycle(0),
      pollution(0) {}*/

Usine::Usine(const string &n, double eau, double elec, double prodBase)
    : Batiment(n, 'U', eau, elec),
      productionBase(prodBase), //la production théorique d’une usine, sans aucun changement ni influence extérieure.
      productionRessources(0), //la production réelle du cycle actuel
      productionDernierCycle(0),  //la productionRessources du cycle précédent, une mémoire du passé.
      pollution(0) {}

double Usine::produireRessources() {
    productionDernierCycle = productionRessources; // mémoriser le cycle précédent
    return productionRessources;
}

double Usine::calculerImpactRessources() {
    return consommationEau + consommationElectricite;
}

double Usine::getProduction() const {
    return productionRessources;
}

double Usine::getPollution() const {
    return pollution;
}

void Usine::setProduction(double p) {
    productionRessources = p;
}

void Usine::setPollution(double pol) {
    pollution = pol;
    if (pollution<0) pollution=0;
}

// --- La fonction saisonnelle que tu veux garder ---
void Usine::lancerCycle(char s)
{
    // 1) efficacité principale (0.8 → 1.2)
    double efficacite = 0.8 + (rand() % 41) / 100.0;

    // 2) facteur saisonnier
    double facteurSaison = 1.0;

        if (s == 'P') {          // Printemps
            facteurSaison = 1.10;
        }
        else if (s == 'E') {     // Été
            facteurSaison = 0.90;
            pollution += 2;
        }
        else if (s == 'A') {     // Automne
            facteurSaison = 1.00;
        }
        else if (s == 'H') {     // Hiver
            facteurSaison = 0.95;
            consommationElectricite += 10;
        }
        else {
            // caractère inconnu, on ne change rien
        }
    // production finale ajustée selon la saison
    productionRessources = productionBase * efficacite * facteurSaison;

    // produire et calculer pollution
    setPollution(pollution+= 3 * efficacite);

}

void Usine::resetCycle() {
    productionRessources = 0;       // remise à zéro de la production

}

void Usine::afficherDetails() {
    Batiment::afficherDetails();
    std::cout << "Production base : " << productionBase << endl;
    std::cout << "Production cycle actuel : " << productionRessources << endl;
    std::cout << "Pollution : " << pollution << endl;
}
