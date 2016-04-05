import plotly.offline as pl
import plotly.graph_objs as go
import numpy as np
import sh

from os import path
from itertools import product

pl.init_notebook_mode()

dir = '/Users/kuba/Code/ETI_private/MN/MN2x/out'

K = 256

def mn2(mt, n):
    cmd = sh.Command("sh")
    out = str(cmd(path.join(dir, 'mn2r'), mt, n))
    return float(out)

def mn2re(mt, n):
    a = mn2(mt, n)
    b = mn2("gauss", 0)
    return abs(b - a) / b

X = np.arange(1, K, 1)

def title(t, i, a):
    t + '\n'*2 + "Rys. " + str(i) + " " + a

def plot_re_n(mt):

    data = go.Scatter(
        x = X,
        y = [mn2re(mt, x) for x in X],
        mode = 'lines+markers',
        name = mt
    )
    
    layout = go.Layout(
        title='Błąd względny (' + mt + ')',
        width=960,
        height=720,
        #hovermode='closest',
        xaxis=dict(title='n'),
        # yaxis=dict(title='Błąd względny', type='log', autorange=True),
        yaxis=dict(title='Błąd względny'),
    )
    
    fig = go.Figure(data=[data], layout=layout)
    
    pl.iplot(fig, show_link=False)
