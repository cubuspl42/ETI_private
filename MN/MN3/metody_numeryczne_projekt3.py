# coding=utf-8
import plotly.offline as pl
import plotly.graph_objs as go
import numpy as np
import sh
import json

from math import log10
from os import path
from IPython.core.display import display, HTML

pl.init_notebook_mode()

bin_path = path.join(path.dirname(path.realpath(__file__)), 'out/mn3')

rys_i = 0

def rys(desc):
    global rys_i
    rys_i += 1
    return 'Rys. ' + str(rys_i) + '. ' + desc

def dump(A):
    return ' '.join(map(str, A))

def mn3(X, Y, N):
    assert len(X) == len(Y)
    n = len(X)
    lines = [
        dump([n, N]),
        dump(X),
        dump(Y)
    ]
    _in = '\n'.join(lines)
    cmd = sh.Command(bin_path)
    out = cmd(_in=_in)
    j = json.loads(str(out))
    return (j[0], j[1])

def _plot(X, Y, X2, Y2, desc, s21, yax):
    assert len(X) == len(Y)

    data1 = go.Scatter(
        x = X,
        y = Y,
        mode = 'lines',
        name = s21
    )

    data2 = go.Scatter(
        x = X2,
        y = Y2,
        mode = 'lines',
        name = s21 + ' (interpolowane)'
    )
    
    layout = go.Layout(
        title='Charakterystyka transmisji',
        width=960,
        height=720,
        xaxis=dict(title='Freq [GHz]'),
        yaxis=dict(title=yax),
    )
    
    fig = go.Figure(data=[data1, data2], layout=layout)
    
    pl.iplot(fig, show_link=False)

    display(HTML('<center>' + rys(desc) + '</center><br><br><br>'))

def plot(X, Y, N):
    X2, Y2 = mn3(X, Y, N)
    desc = 'Wykres zadanej funkcji w formie dyskretnej oraz interpolowanej (' + str(N) + ' wartości)'
    _plot(X, Y, X2, Y2, desc, 'S21', '')

def plot_db(X, Y, N):
    def S21_dB(x):
        # x = abs(x) + 0.0001
        return 20*log10(x)
    X2, Y2 = mn3(X, Y, N)
    Y = list(map(S21_dB, Y))
    Y2 = list(map(S21_dB, Y2))
    desc = 'Wykres logarytmiczny zadanej funkcji w formie dyskretnej oraz interpolowanej (' + str(N) + ' wartości)'
    _plot(X, Y, X2, Y2, desc, 'S21_dB', '[dB]')