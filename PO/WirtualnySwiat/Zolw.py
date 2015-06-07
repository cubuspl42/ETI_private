import random
from Zwierze import Zwierze

class Zolw(Zwierze):
    def __init__(self, swiat):
        super().__init__(swiat, 2, 1)

    def akcja(self):
        if random.randrange(4) == 0:
            super().akcja()

    def kolor(self):
        return 'brown4'

    def nazwa(self):
        return 'żółw'

    def kolizja(self, atakujacy, skadAtakuje):
        if(atakujacy.sila() < 5):
            atakujacy.ustawPolozenie(skadAtakuje)
        else:
            super().kolizja(atakujacy, skadAtakuje)
