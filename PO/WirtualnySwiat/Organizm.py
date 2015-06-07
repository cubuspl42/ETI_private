import random

class Organizm:
    _sila = 0
    _inicjatywa = 0
    _polozenie = (0, 0)
    _swiat = None
    _wiek = 0
    _zywy = True

    def __init__(self, swiat, sila, inicjatywa):
        self._swiat = swiat
        self._sila = sila
        self._inicjatywa = inicjatywa

    def akcja(self):
        self._wiek = self._wiek + 1

    def przesunNaLosoweSasiedniePole(self, dystans=1):
        pola = self._swiat.polaSasiadujace(self.polozenie(), dystans)
        self._swiat.przesunOrganizm(self, random.choice(pola))

    def przesunNaLosoweWolneSasiedniePole(self, dystans=1):
        wolnePola = self._swiat.wolnePolaSasiadujace(self.polozenie(), dystans)
        if wolnePola:
            self._swiat.przesunOrganizm(self, random.choice(wolnePola))

    def rozmnozSie(self):
        if(self._swiat.wolnePolaSasiadujace(self.polozenie())):
            Klasa = type(self)
            potomek = Klasa(self._swiat)
            self._swiat.dodajOrganizm(potomek)
            potomek.ustawPolozenie(self.polozenie())
            potomek.przesunNaLosoweWolneSasiedniePole()
            self._swiat.raportujZdarzenie(self.nazwa() + " rozmnaża się")

    def kolizja(self, atakujacy, skadAtakuje):
        if(type(self) == type(atakujacy)):
            atakujacy.ustawPolozenie(skadAtakuje)
            self.rozmnozSie()
        else:
            if(self.sila() <= atakujacy.sila()):
                self._swiat.raportujZdarzenie(atakujacy.nazwa() + " pożera " + self.nazwa())
                self.zabij()
            else:
                self._swiat.raportujZdarzenie(self.nazwa() + " pożera " + atakujacy.nazwa())
                atakujacy.zabij()

    def kolor(self):
        return 'black'

    def nazwa(self):
        return 'organizm'

    def inicjatywa(self):
        return self._inicjatywa

    def ustawPolozenie(self, polozenie):
        self._polozenie = polozenie

    def polozenie(self):
        return self._polozenie

    def sila(self):
        return self._sila

    def dodajDoSily(self, dodatkowaSila):
        self._sila = self._sila + dodatkowaSila

    def wiek(self):
        return self._wiek

    def zabij(self):
        self._zywy = False
        self._swiat.usunOrganizm(self)

    def zywy(self):
        return self._zywy
