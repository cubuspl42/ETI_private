from Zwierze import Zwierze

class Owca(Zwierze):
    def __init__(self, swiat):
        super().__init__(swiat, 4, 4)

    def kolor(self):
        return 'white'

    def nazwa(self):
        return 'owca'
