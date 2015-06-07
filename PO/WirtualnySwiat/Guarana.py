from Roslina import Roslina

class Guarana(Roslina):
    def __init__(self, swiat):
        super().__init__(swiat, 0)

    def kolizja(self, atakujacy, skadAtakuje):
        atakujacy.dodajDoSily(3)
        super().kolizja(atakujacy, skadAtakuje)

    def nazwa(self):
        return 'guarana'

    def kolor(self):
        return 'red'
