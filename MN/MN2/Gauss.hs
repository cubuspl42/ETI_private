module Gauss 
( _linearSolve
, _linearSolveGS
, xik1i
) where

import Debug.Trace
import Data.Matrix

(>->) = elementwise (-)
(>*>) = multStd

_combineRows k a i = combineRows i (-aik/akk) k a
    where   aik = getElem i k a
            akk = getElem k k a

_linearSolve' a k = foldl (_combineRows k) (scaleRow (1/akk) k a) (filter (/=k) [1..m])
    where   m = nrows a
            akk = getElem k k a

_linearSolve a = foldl _linearSolve' a [1..m]
    where m = nrows a

xik1i :: Matrix Double -> Matrix Double -> Int -> [Double] -> [Double]
xik1i a b 0 xjk = []
xik1i a b i xjk = xjk1 ++ [xik1]
    where   n = ncols a
            aii = getElem i i a
            aij j = getElem i j a
            bi = getElem i 1 b
            aij1 = map aij [1..(i-1)]
            aij2 = map aij [(i+1)..n]
            xjk1 = xik1i a b (i-1) xjk
            saijxjk1 = sum (zipWith (*) aij1 xjk1)
            saijxjk = sum (zipWith (*) aij2 (drop i xjk))
            xik1 = (bi - saijxjk1 - saijxjk) / aii

_linearSolveGS :: Matrix Double -> Matrix Double -> Int -> Matrix Double
_linearSolveGS a b ni = transpose (fromLists [xs])
    where   n = ncols a
            x0 = replicate n 0.5
            xs = iterate (xik1i a b n) x0 !! ni
