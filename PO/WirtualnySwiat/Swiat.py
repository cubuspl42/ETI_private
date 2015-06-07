import tkinter
import random

from Czlowiek import Czlowiek
from Wilk import Wilk
from Owca import Owca
from Lis import Lis
from Zolw import Zolw
from Antylopa import Antylopa

from Trawa import Trawa
from Mlecz import Mlecz
from Guarana import Guarana
from WilczeJagody import WilczeJagody

class Swiat:
    SZEROKOSC_IKONY=16
    SZEROKOSC_SWIATA=20
    OFFSET=4

    _organizmy = []
    _poleTekstowe = None
    _dx = 0
    _dy = 0
    _aktywujTarcze = False

    def __init__(self):
        self._wypelnijPlansze()
        self._inicjalizujOkno()
        self._rysujSwiat()

    def _idzDoGory(self, event):
        self._dx = 0
        self._dy = -1

    def _idzWDol(self, event):
        self._dx = 0
        self._dy = 1

    def _idzWLewo(self, event):
        self._dx = -1
        self._dy = 0

    def _idzWPrawo(self, event):
        self._dx = 1
        self._dy = 0

    def aktywujTarcze(self, event):
        self._aktywujTarcze = True

    def _inicjalizujOkno(self):
        master = tkinter.Tk()

        master.bind('<Left>', self._idzWLewo)
        master.bind('<Right>', self._idzWPrawo)
        master.bind('<Up>', self._idzDoGory)
        master.bind('<Down>', self._idzWDol)
        master.bind('<space>', self.aktywujTarcze)

        size = Swiat.SZEROKOSC_SWIATA * Swiat.SZEROKOSC_IKONY + Swiat.OFFSET * 2
        self._canvas = tkinter.Canvas(master, width=size, height=size)
        self._canvas.pack()

        self._poleTekstowe = tkinter.Text(master, height=4, width=40)
        self._poleTekstowe.pack()

        przycisk = tkinter.Button(master, text="Nowa tura", width=10, command=self.wykonajTure)
        przycisk.pack()

    def polaSasiadujace(self, pozycja, dystans=1):
        x, y = pozycja
        pola = []
        for i in range(dystans):
            j = i + 1
            if(x < Swiat.SZEROKOSC_SWIATA - j):
                pola.append((x+j, y))
            if(x >= j):
                pola.append((x-j, y))
            if(y < Swiat.SZEROKOSC_SWIATA - j):
                pola.append((x, y+j))
            if(y >= j):
                pola.append((x, y-j))
        return pola

    def wolnePolaSasiadujace(self, pozycja, dystans=1):
        pola = self.polaSasiadujace(pozycja, dystans)
        wolnePola = []
        for p in pola:
            if not self.organizmNaPozycji(p):
                wolnePola.append(p)
        return wolnePola

    def przesunOrganizm(self, organizm, pozycja):
        obronca = self.organizmNaPozycji(pozycja)
        skadAtakuje = organizm.polozenie()
        organizm.ustawPolozenie(pozycja)
        if obronca:
            obronca.kolizja(organizm, skadAtakuje)

    def organizmNaPozycji(self, pozycja):
        for o in self._organizmy:
            if o.polozenie() == pozycja:
                return o
        return None

    def czlowiek(self):
        for o in self._organizmy:
            if type(o) is Czlowiek:
                return o
        return None

    def dodajOrganizm(self, organizm):
        self._organizmy.append(organizm)

    def usunOrganizm(self, organizm):
        self._organizmy.remove(organizm)

    def _dodajOrganizmLosowo(self, organizm):
        pozycja = (0, 0)

        while True:
            pozycja = (random.randrange(Swiat.SZEROKOSC_SWIATA), random.randrange(Swiat.SZEROKOSC_SWIATA))
            if not self.organizmNaPozycji(pozycja):
                break

        organizm.ustawPolozenie(pozycja)
        self.dodajOrganizm(organizm)

    def _dodajKilkaOrganizmow(self, Klasa, min, max):
        for _ in range(random.randint(min, max)):
            organizm = Klasa(self)  # nowy organizm klasy Klasa
            self._dodajOrganizmLosowo(organizm)

    def _wypelnijPlansze(self):
        self._dodajKilkaOrganizmow(Czlowiek, 1, 1)

        self._dodajKilkaOrganizmow(Wilk, 2, 15)
        self._dodajKilkaOrganizmow(Owca, 2, 5)
        self._dodajKilkaOrganizmow(Lis, 2, 5)
        self._dodajKilkaOrganizmow(Zolw, 2, 5)
        self._dodajKilkaOrganizmow(Antylopa, 2, 5)

        self._dodajKilkaOrganizmow(Trawa, 2, 5)
        self._dodajKilkaOrganizmow(Mlecz, 2, 5)
        self._dodajKilkaOrganizmow(Guarana, 2, 5)
        self._dodajKilkaOrganizmow(WilczeJagody, 2, 5)

    def raportujZdarzenie(self, komunikat):
        self._poleTekstowe.config(state=tkinter.NORMAL)
        self._poleTekstowe.insert(tkinter.END, komunikat + '\n')
        self._poleTekstowe.config(state=tkinter.DISABLED)

    def _wykonajAkcje(self):
        czlowiek = self.czlowiek()
        if czlowiek:
            czlowiek.ustawKierunek(self._dx, self._dy)
            if self._aktywujTarcze:
                czlowiek.aktywujTarcze()

        self._aktywujTarcze = False
        self._dx = 0
        self._dy = 0

        organizmy = sorted(self._organizmy, key=lambda o: (o.inicjatywa(), o.wiek()))
        for o in organizmy:
            if o.zywy():
                o.akcja()

    def _rysujSwiat(self):
        self._canvas.delete(tkinter.ALL) # usunięcie wszystkich elementów z canvas
        for o in self._organizmy:
            x, y = o.polozenie()
            self._canvas.create_rectangle(Swiat.OFFSET + x * Swiat.SZEROKOSC_IKONY,
                                          Swiat.OFFSET + y * Swiat.SZEROKOSC_IKONY,
                                          Swiat.OFFSET + x * Swiat.SZEROKOSC_IKONY + Swiat.SZEROKOSC_IKONY,
                                          Swiat.OFFSET + y * Swiat.SZEROKOSC_IKONY + Swiat.SZEROKOSC_IKONY,
                                          fill = o.kolor())

    def wykonajTure(self):
        self._wyczyscPoleTekstowe()
        self._wykonajAkcje()
        self._rysujSwiat()

    def symulujSwiat(self):
        tkinter.mainloop()

    def _wyczyscPoleTekstowe(self):
        self._poleTekstowe.config(state=tkinter.NORMAL)
        self._poleTekstowe.delete(1.0, tkinter.END)
        self._poleTekstowe.config(state=tkinter.DISABLED)
