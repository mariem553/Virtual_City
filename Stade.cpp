#include "Stade.h"
#include "Ville.h"
#include "Batiment.h"

// Constructeur du Stade : initialise un bâtiment de type 'S' avec sa capacité
Stade::Stade(const string& nom, double eau, double elec, int cap)
    : Batiment(nom, 'S', eau, elec),
      capacite(cap)
{
    // Sauvegarde des valeurs initiales pour le reset des cycles
    EauInit = eau;
    ElecInit = elec;
}

int Stade::getCapacite() const {
    return capacite;
}

// Calcule le revenu d'un événement : participants × prix du billet
float Stade::calculerRevenu(int participants, float prixTicket) const {
    return participants * prixTicket;
}

// Méthode principale pour organiser un événement dans le stade
void Stade::specialiteEvenement(Ville& v, int choix, int participants) {
    cout << "\n=== Organisation d'un evenement au Stade '" << nom << "' ===\n";

    float prixTicket = 0;
    float coutOrganisation = 0;

    // Configuration selon le type d'événement
    switch(choix) {
        case 1:  // Match de football
            prixTicket = 20;
            coutOrganisation = 300;
            cout << "Match de football organise!" << endl;
            break;
        case 2:  // Concert
            prixTicket = 50;
            coutOrganisation = 700;
            cout << "Concert organise!" << endl;
            break;
        case 3:  // Compétition sportive
            prixTicket = 30;
            coutOrganisation = 500;
            cout << "Competition sportive organisee!" << endl;
            break;
        default:
            cout << "Choix invalide.\n";
            return;
    }

    // Vérification de la capacité du stade
    if (participants > capacite) {
        cout << " Stade plein ! Capacité max = " << capacite << "\n";
        participants = capacite;  // Limite au nombre maximum de places
    }

    // Calculs financiers
    float revenu = calculerRevenu(participants, prixTicket);
    float gainNet = revenu - coutOrganisation;

    // Mise à jour du budget de la ville
    v.setBudget(v.getBudget() + gainNet);

    // Bonus de satisfaction selon le type d'événement
    float satisfactionBonus = 0;
    switch(choix) {
        case 1: satisfactionBonus = 10; break;  // Football
        case 2: satisfactionBonus = 15; break;  // Concert
        case 3: satisfactionBonus = 12; break;  // Compétition
    }

    // Application du bonus de satisfaction
    v.setSatisfaction(v.getSatisfaction() + satisfactionBonus);
    if (v.getSatisfaction() > 100) v.setSatisfaction(100);  // Limite à 100%

    // Affichage des résultats
    cout << "\nEvenement reussi !\n";
    cout << "Participants: " << participants << "\n";
    cout << "Gain net: " << gainNet << " DT\n";
    cout << "Satisfaction +" << satisfactionBonus << "%\n";
    cout << "Nouveau budget: " << v.getBudget() << " DT\n";
}

// Affichage des détails du stade
void Stade::afficherDetails() {
    cout << "\nStade #"<< id << ":" << nom << "\n";
    cout << "Satisfaction :" << effetSatisfaction << " %\n";
    cout << "Capacite: " << capacite << "\n";
    cout << "Eau consommee: " << consommationEau << "\n";
    cout << "Electricite consommee: " << consommationElectricite << "\n";
}

// Calcul de l'impact total sur les ressources
double Stade::calculerImpactRessources() {
    return consommationEau + consommationElectricite;
}

// Cycle saisonnier : ajuste consommations et satisfaction selon la saison
void Stade::lancerCycle(char s) {
    // Hiver : plus d'éclairage, baisse de satisfaction
    if (s == 'H') {
        consommationElectricite *= 1.1;  // +10% électricité
        effetSatisfaction -= 20;          // Baisse de satisfaction
    }

    // Été : légère hausse de consommation, augmentation de satisfaction
    if (s == 'E') {
        consommationElectricite *= 1.05;  // +5% électricité
        effetSatisfaction += 10;           // Hausse de satisfaction
    }
}

void Stade::resetCycle() {
    consommationEau = EauInit;           // Retour à la consommation d'eau initiale
    consommationElectricite = ElecInit;  // Retour à la consommation d'électricité initiale
}
