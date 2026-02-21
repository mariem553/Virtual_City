#include "Ville.h"
#include "maison.h"
using namespace std;

/* Constructeur par défaut commenté
Ville::Ville()
    : nom("inconnu"), budget(10000), population(0), satisfaction(0),
      eauTotal(5000), electriciteTotal(5000) {}*/

// Constructeur paramétré : initialise une ville avec son nom et des valeurs par défaut
Ville::Ville(const string& n)
    : nom(n), budget(10000), population(0), satisfaction(0),
      eauTotal(5000), electriciteTotal(5000) {
    // Sauvegarde des valeurs initiales pour pouvoir les restaurer plus tard
    eauInit = eauTotal;      // Sauvegarde la quantité d'eau initiale (5000)
    elecInit = electriciteTotal; // Sauvegarde la quantité d'électricité initiale (5000)
    budgetInit = budget;      // Sauvegarde le budget initial (10000)
}


void Ville::resetCycleVille() {
    eauTotal = eauInit;           // Restaure l'eau à sa valeur initiale
    electriciteTotal = elecInit;  // Restaure l'électricité à sa valeur initiale
    budget = budgetInit;          // Restaure le budget à sa valeur initiale

    // Réinitialise le cycle de chaque bâtiment (s'ils ont une méthode resetCycle)
    for (auto b : batiments) {
        b->resetCycle();   // Appelle la méthode resetCycle de chaque bâtiment
    }
    cout << "Les ressources ont été réinitialisées.\n";
}

// Ajoute un bâtiment en vérifiant les ressources nécessaires
void Ville::ajouterBatiment(Batiment* b)
{
    if (!b) return;  // Vérifie que le pointeur n'est pas nul

    // Récupère les consommations du bâtiment
    int eauRequise = b->getConsommationEau();
    int elecRequise = b->getConsommationElectricite();

    // Consomme les ressources
    eauTotal -= eauRequise;
    electriciteTotal -= elecRequise;

    // Ajoute le bâtiment à la liste
    batiments.push_back(b);
    cout << "Batiment " << b->getNom() << " ajoute." << endl;

    // Met à jour les calculs globaux
    calculerConsommationTotale();
    calculerTotalEau();
    calculerTotalElec();
    majPopulation();  // Met à jour la population (si c'est une maison)
}

//Supprime un bâtiment et restaure ses ressources consommées
void Ville::supprimerBatiment(int id)
{
    // Parcourt la liste des bâtiments
    for (auto it = batiments.begin(); it != batiments.end(); ++it)
    {
        if ((*it)->getId() == id)  // Trouve le bâtiment par son ID
        {
            // Restaure les ressources consommées par le bâtiment
            eauTotal += (*it)->getConsommationEau();
            electriciteTotal += (*it)->getConsommationElectricite();

            // Si c'est une maison, ajuste la population
            if ((*it)->getType() == 'H'){
                Maison* m = dynamic_cast<Maison*>(*it);
                setPopulation(getPopulation() - m->getHabitants());
            }

            // Libère la mémoire et supprime de la liste
            delete *it;
            batiments.erase(it);
            cout << "Bâtiment supprimé et ressources restaurées.\n";

            // Met à jour les calculs globaux
            calculerConsommationTotale();
            calculerTotalEau();
            calculerTotalElec();
            return;
        }
    }
    cout << "Aucun bâtiment trouvé avec cet id.\n";
}

double Ville::geteauInit(){
    return eauInit;
}

double Ville::getelecInit(){
    return elecInit;
}

double Ville::calculerConsommationTotale() const
{
    int total = 0;
    for (auto b : batiments)
        total += b->calculerImpactRessources();  // Somme des impacts de chaque bâtiment
    return total;
}

// Calcule la consommation totale d'eau de tous les bâtiments
double Ville::calculerTotalEau() const {
    double total = 0;
    for (auto b : batiments)
        total += b->getConsommationEau();  // Somme des consommations d'eau
    return total;
}

// Calcule la consommation totale d'électricité de tous les bâtiments
double Ville::calculerTotalElec() const {
    double total = 0;
    for (auto b : batiments)
        total += b->getConsommationElectricite();  // Somme des consommations d'électricité
    return total;
}

// Calcule la satisfaction moyenne de la ville basée sur les bâtiments
double Ville::calculerSatisfaction() {
    if (batiments.empty())
       return satisfaction = 0;  // Si pas de bâtiments, satisfaction = 0

    double totalSatisfaction = 0;

    // Somme les satisfactions de tous les bâtiments
    for (auto b : batiments) {
        totalSatisfaction += b->getEffetSatisfaction();
    }

    // Moyenne des satisfactions
    satisfaction = totalSatisfaction / batiments.size();
    return satisfaction;
}

// Modifie la satisfaction de tous les bâtiments d'un certain pourcentage
void Ville::CalculerSatisfactionBat(double pourcentage) {
    double x = pourcentage / batiments.size();  // Répartit le pourcentage sur tous les bâtiments
    for (auto b : batiments)
    {
        double effet = b->getEffetSatisfaction();
        double nouvelleValeur = effet + x;  // Applique la modification
        b->setEffetSatisfaction(nouvelleValeur);
    }
}

// Modifie la quantité d'eau disponible dans la ville
void Ville::setEau(double quantite) {
    eauTotal = quantite;
}

// Modifie la quantité d'électricité disponible dans la ville
void Ville::setElectricite(double quantite) {
    electriciteTotal = quantite;
}

// Accesseurs (getters)
double Ville::getEau() const { return eauTotal; }
double Ville::getElectricite() const { return electriciteTotal; }
double Ville::getSatisfaction() const { return satisfaction; }

// Mutateurs (setters)
void Ville::setSatisfaction(double s) { satisfaction = s; }

// Retourne la liste des bâtiments (référence constante)
const vector<Batiment*>& Ville::getBatiments() const { return batiments; }

void Ville::setPopulation(double p) { population = p; }
double Ville::getPopulation() const { return population; }
double Ville::getBudget() { return budget; }
void Ville::setBudget(double b) { budget = b; }

// Affiche toutes les informations de la ville dans la console
void Ville::afficherVille() {

    cout << "===== Ville : " << nom << " =====\n";
    cout << "Budget : " << budget << endl;
    cout << "Population : " << population << endl;
    cout << "Satisfaction : " << satisfaction << "%\n";

    cout << "Eau restante : " << eauTotal << endl;
    cout << "electricite restante : " << electriciteTotal << endl;

    cout << "Consommation totale d'eau : " << calculerTotalEau() << endl;
    cout << "Consommation totale d'electricite : " << calculerTotalElec() << endl;

    cout << "\n--- Batiments ---\n";
    for (auto b : batiments)
        b->afficherDetails();  // Affiche les détails de chaque bâtiment

    cout << "==============================\n";
}

// Met à jour la population totale en sommant les habitants de toutes les maisons
void Ville::majPopulation() {
    population = 0;
    for (auto b : batiments) {
        if (b->getType() == 'H') {  // Vérifie si c'est une maison
            Maison* m = dynamic_cast<Maison*>(b);
            if (m) population += m->getHabitants();  // Ajoute les habitants
        }
    }
}

// Destructeur : libère la mémoire de tous les bâtiments
Ville::~Ville()
{
    for (auto b : batiments)
        delete b;  // Libère chaque bâtiment
}
