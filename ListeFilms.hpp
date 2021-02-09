#pragma once
#pragma region "Includes"//{
#include "ListeFilms.hpp"
//#include "structures.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>
#include "cppitertools/range.hpp"
#include "gsl/span"

#pragma endregion//}


struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.


struct ListeActeurs {
	int capacite, nElements;
	Acteur** elements; // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
};

struct Film
{
	std::string titre, realisateur; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
	int anneeSortie, recette; // Année de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs;
};


class ListeFilms
{
public:
	ListeFilms();
	ListeFilms(int capacite);
	ListeFilms(std::string nomFichier); // creerListe
	~ListeFilms();
	void ajouterFilm(Film* ptrFilm);
	Acteur* trouverActeur(std::string nom) const;
	void enleverFilm(Film* PtrFilm);
	void afficherListeFilms() const;
	Film** getFilms() const;
	int size() const;
	ListeFilms copy(int capacite);
	Film* getFilm(unsigned index) const;
	void afficherFilm(const Film& film)const;
	void afficherActeur(const Acteur& acteur) const;
	void detruireFilm(Film* ptrFilm);


private:
	int capacite_, nElements_;
	Film** elements_;
	void doublerCapacite();
	Film* lireFilm(std::istream& fichier);
	Acteur* lireActeur(std::istream& fichier);
	Acteur* trouverActeurListeActeurs(std::string nomRechercher, ListeActeurs& listeActeurs) const;
	void setFilm(unsigned index, Film* ptrFilm);


};


struct Acteur
{
	std::string nom; int anneeNaissance; char sexe;
	ListeFilms joueDans;
};

