import random
from Organizm import Organizm

class Roslina(Organizm):
    def __init__(self, swiat, sila):
        super().__init__(swiat, sila, 0)

    def akcja(self):
        self.podejmijProbeRozprzestrzenienia()
        super().akcja()

    def podejmijProbeRozprzestrzenienia(self):
        if(random.randrange(100) == 0):
            self.rozmnozSie()
