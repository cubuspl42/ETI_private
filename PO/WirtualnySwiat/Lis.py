import random
from Zwierze import Zwierze

class Lis(Zwierze):
    def __init__(self, swiat):
        super().__init__(swiat, 3, 7)

    def akcja(self):
        pola = self._swiat.polaSasiadujace(self.polozenie())
        polaZeSlabszymi = []
        for p in pola:
            o = self._swiat.organizmNaPozycji(p)
            if not o or o.sila() <= self.sila():
                polaZeSlabszymi.append(p)
        if polaZeSlabszymi:
            self._swiat.przesunOrganizm(self, random.choice(polaZeSlabszymi))

    def nazwa(self):
        return 'lis'

    def kolor(self):
        return 'dark orange'