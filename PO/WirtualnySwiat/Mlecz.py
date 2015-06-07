from Organizm import Organizm
from Roslina import Roslina

class Mlecz(Roslina):
    def __init__(self, swiat):
        super().__init__(swiat, 0)

    def akcja(self):
        for _ in range(3):
            self.podejmijProbeRozprzestrzenienia()
        Organizm.akcja(self)

    def nazwa(self):
        return 'mlecz'

    def kolor(self):
        return 'yellow'
