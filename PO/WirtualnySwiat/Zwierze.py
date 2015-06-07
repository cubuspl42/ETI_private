import random

from Organizm import Organizm

class Zwierze(Organizm):
    def __init__(self, swiat, sila, inicjatywa):
        super().__init__(swiat, sila, inicjatywa)

    def akcja(self):
        self.przesunNaLosoweSasiedniePole()
        super().akcja()
