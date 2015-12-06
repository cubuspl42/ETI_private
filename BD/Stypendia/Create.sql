CREATE TABLE Stale (
	RokAkademicki INT PRIMARY KEY CHECK ( RokAkademicki >= 1970 ), /* Unix epoch */
	Prog INT NOT NULL,
	KwotaMinimalna INT NOT NULL,
	KwotaMaksymalna INT NOT NULL,
);

CREATE TABLE Studenci (
	NumerIndeksu INT PRIMARY KEY,
	Pesel char(11) NOT NULL,
	Imie varchar(128) NOT NULL,
	Nazwisko varchar(128) NOT NULL,
	CzyAktualnieNaLiscieStudentow bit NOT NULL,
);

CREATE TABLE WnioskiOStypendium ( /* ERD! */
	NumerWniosku INT IDENTITY(1, 1) PRIMARY KEY,
	NumerKorygowanegoWniosku INT FOREIGN KEY REFERENCES WnioskiOStypendium, /* NULLABLE */
	RokAkademicki INT REFERENCES Stale NOT NULL,
	NumerIndeksuStudenta INT REFERENCES Studenci NOT NULL,
	DataZlozeniaWniosku DATE NOT NULL,
	NumerRachunkuBankowego varchar(128), /* NULLABLE */
);

CREATE TABLE DaneCzlonkaRodziny (
	NumerWniosku INT FOREIGN KEY REFERENCES WnioskiOStypendium NOT NULL,
	Pesel char(11) NOT NULL,
	Imie varchar(128) NOT NULL,
	Nazwisko varchar(128) NOT NULL,
	MiesiecznyDochod INT NOT NULL CHECK ( MiesiecznyDochod >= 0 ),
	PRIMARY KEY (NumerWniosku, Pesel)
);

CREATE TABLE WyplatyMiesieczne (
	Miesiac INT CHECK ( Miesiac >= 1 AND Miesiac <= 12), /* Styczeñ - Grudzieñ */
	NumerWniosku INT FOREIGN KEY REFERENCES WnioskiOStypendium NOT NULL,
	KwotaWyplaty INT NOT NULL CHECK ( KwotaWyplaty >= 0 ),
	PRIMARY KEY (Miesiac, NumerWniosku)
);
