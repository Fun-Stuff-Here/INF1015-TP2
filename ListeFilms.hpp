#pragma once
#include "structures.hpp"
#include <string>
#include <fstream>


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

