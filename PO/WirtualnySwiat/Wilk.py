from Zwierze import Zwierze

class Wilk(Zwierze):
    def __init__(self, swiat):
        super().__init__(swiat, 9, 5)

    def kolor(self):
        return 'light slate gray'

    def nazwa(self):
        return 'wilk'
