# coding=utf-8
import plotly.offline as pl
import plotly.graph_objs as go
import numpy as np
import sh
import json

from os import path

pl.init_notebook_mode()

bin_path = path.join(__file__, 'out/mn3')

K = 256

def dump(A):
    ' '.join(map(str, A))

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
    out = cmd(n, _in=_in)
    return json.loads(out)

X = np.arange(1, K, 1)

def title(t, i, a):
    t + '\n'*2 + "Rys. " + str(i) + " " + a

def plot_spline_n(X, Y, N):
    assert len(X) == len(Y)

    data = go.Scatter(
        x = X,
        y = mn3(X, Y, N),
        mode = 'lines+markers',
        name = 'Data'
    )
    
    layout = go.Layout(
        title='Błąd względny (' + mt + ')',
        width=960,
        height=720,
        #hovermodcd e='closest',
        xaxis=dict(title='n'),
        # yaxis=dict(title='Błąd względny', type='log', autorange=True),
        yaxis=dict(title='Błąd względny'),
    )
    
    fig = go.Figure(data=[data], layout=layout)
    
    pl.iplot(fig, show_link=False)
