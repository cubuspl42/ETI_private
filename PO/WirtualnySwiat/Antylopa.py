import random

from Organizm import Organizm
from Zwierze import Zwierze

class Antylopa(Zwierze):
    def __init__(self, swiat):
        super().__init__(swiat, 4, 4)

    def akcja(self):
        self.przesunNaLosoweSasiedniePole(2)
        Organizm.akcja(self)

    def kolizja(self, atakujacy, skadAtakuje):
        if random.randrange(2):
            self.przesunNaLosoweSasiedniePole()
        else:
            super().kolizja(atakujacy, skadAtakuje)

    def nazwa(self):
        return 'antylopa'

    def kolor(self):
        return 'wheat'