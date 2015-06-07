from Organizm import Organizm
from Zwierze import Zwierze

class Czlowiek(Zwierze):
    _dx = 0
    _dy = 0
    _poziomNaladowaniaTarczy = 0

    def __init__(self, swiat):
        super().__init__(swiat, 5, 4)

    def ustawKierunek(self, dx, dy):
        self._dx = dx
        self._dy = dy

    def akcja(self):
        if self._poziomNaladowaniaTarczy > 0:
            self._poziomNaladowaniaTarczy = self._poziomNaladowaniaTarczy - 1
        x, y = self.polozenie()
        nowe_polozenie = (x + self._dx, y + self._dy)
        if nowe_polozenie in self._swiat.polaSasiadujace(self.polozenie()):
            self._swiat.przesunOrganizm(self, nowe_polozenie)
        Organizm.akcja(self) # ominięcie implementacji z klasy Zwierze

    def kolizja(self, atakujacy, skadAtakuje):
        if self._poziomNaladowaniaTarczy > 0 :
            atakujacy.przesunNaLosoweWolneSasiedniePole()
        else:
            super().kolizja(atakujacy, skadAtakuje)

    def nazwa(self):
        return 'człowiek'

    def aktywujTarcze(self):
        if self._poziomNaladowaniaTarczy == 0:
            self._poziomNaladowaniaTarczy = 5

    def kolor(self):
        return 'RoyalBlue2'