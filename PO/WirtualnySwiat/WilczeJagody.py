from Roslina import Roslina

class WilczeJagody(Roslina):
    def __init__(self, swiat):
        super().__init__(swiat, 99)

    def kolizja(self, atakujacy, skadAtakuje):
        atakujacy.zabij()

    def nazwa(self):
        return 'wilcze jagody'

    def kolor(self):
        return 'blue4'
