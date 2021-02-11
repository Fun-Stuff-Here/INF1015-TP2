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


struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront d�fini apr�s.


struct ListeActeurs {
	int capacite, nElements;
	Acteur** elements; // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
};

struct Film
{
	std::string titre, realisateur; // Titre et nom du r�alisateur (on suppose qu'il n'y a qu'un r�alisateur).
	int anneeSortie, recette; // Ann�e de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs;
};


class ListeFilms
{
public:
	explicit ListeFilms();
	explicit ListeFilms(std::string nomFichier); // creerListe
	~ListeFilms();
	void ajouterFilm(Film* ptrFilm);
	Acteur* trouverActeur(std::string nom) const;
	void enleverFilm(Film* PtrFilm);
	void afficherListeFilms() const;
	void afficherFilm(const Film& film)const;
	void afficherActeur(const Acteur& acteur) const;
	void detruireFilm(Film* ptrFilm);
	void detruireListeFilms();
	Film* operator[] (const std::size_t index) const;


private:
	int capacite_, nElements_;
	Film** elements_;
	void doublerCapacite();
	Film* lireFilm(std::istream& fichier);
	Acteur* lireActeur(std::istream& fichier);
	Acteur* trouverActeurListeActeurs(std::string nomRechercher, ListeActeurs& listeActeurs) const;


};


struct Acteur
{
	std::string nom; int anneeNaissance; char sexe;
	ListeFilms joueDans;
};

