from Roslina import Roslina

class Trawa(Roslina):
    def __init__(self, swiat):
        super().__init__(swiat, 0)

    def kolor(self):
        return 'green'

    def nazwa(self):
        return 'trawa'
