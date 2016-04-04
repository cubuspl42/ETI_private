-- import Data.Matrix
import Data.Ratio
import Data.List
import Numeric.LinearAlgebra
import Data.Maybe
import qualified Data.Matrix
import Gauss

type Re = Double

data GameDesc = GameDesc { n :: Int, trps :: [(Int, Int)] }

count xs = foldl (+) 0 (map (\i -> if i then 1 else 0) xs)

d :: Int
d = 6

agab n = [(g, a, b) | g <- [1,2], b <- [0..(n-1)], a <- [0..(n-1)]]

k gd ki = snd (head ((filter (\p -> (fst p) == ki) (trps gd)) ++ [(0, 0)]))

di gd i = i + (k gd i)

bb :: GameDesc -> Int -> Int -> Re
bb gd g a
    | g == 1 && a+d >= (n gd) = (fi (-(a+d-(n gd)+1))) / (fi d)
    | otherwise = 0
    where fi = fromIntegral

aa' gd c ci = (fi (count (map (== ci) (map (di gd) [(c+1)..(min (c+d) (n gd-1))])))) / (fi d)
    where fi = fromIntegral

aa :: GameDesc -> Int -> Int -> Int -> (Int, Int, Int) -> Re 
aa gd g a b (gi, ai, bi)
    | (g,a,b) == (gi,ai,bi) = -1 
    | g == 1 && g /= gi && b == bi = aa' gd a ai
    | g == 2 && g /= gi && a == ai = aa' gd b bi
    | otherwise = 0

eq gd (g, a, b) = (map (aa gd g a b) (agab (n gd)), bb gd g a)

am :: GameDesc -> (Matrix (Re), [Re])
am gd = (fromLists rows, bs)
    where
    eqs = (unzip (map (eq gd) (agab (n gd))))    
    rows = fst eqs
    bs = snd eqs 

p1 = GameDesc 2 [(1, -1)]

p2 = GameDesc 6 [(4, -2), (5, -2)]

gd5 = GameDesc 28 [(1, -1), (2, -2), (3, -3), (7, -1), (13, -4), (14, -5), (17, -1), (20, -1), (21, -2), (22, -3), (24, -1), (25, -2), (26, -3)]

gd9 = GameDesc 28 [(2, -1), (4, -4), (5, -2), (6, -5), (7, -6), (10, -1), (13, -2), (15, -1), (18, -4),(20, -9), (22, -3), (24, -1), (26, -26)]

gdf1 = GameDesc 27 [(4, -2), (5, -2), (7, -5), (9, -3), (14, -12), (15, -2), (17, -7), (19, -8), (21, -3), (22, -16), (25, -9)]

shx (xa, (g, a, b)) = (show xa) ++ " * x" ++ (show g) ++ "." ++ (show a) ++ "." ++ (show b)

sheq n (xas, b) =
    (intercalate " + " (map shx (filter f (zip xas (agab n))))) ++ " + " ++ (show (-b)) ++ " = 0"
    where f (xa, _) = xa /= 0
sheqs n xass bs =
    intercalate "\n" (map (sheq n) (zip xass bs))

cvm = (Data.Matrix.fromLists . Numeric.LinearAlgebra.toLists)

main = do
    -- putStrLn (sheqs (n p) (toLists a) b)
    putStrLn (show (rows a))
    putStrLn (show (cols a))
    --putStrLn (show a)
    -- putStr (prettyMatrix a)
    -- putStrLn (show b)
    putStrLn (show (shfst x))
    --putStrLn (show x)
    --putStrLn (show ab)
    --putStrLn (show x')
    --putStrLn (show (shfst' x'))
    putStrLn (show (shfst' x''))
    --putStrLn (show x'')
        where
        a = fst (am p)
        b = snd (am p)
        b' = tr (fromLists [b])
        x = fromJust (linearSolve a b')
        x' = _linearSolve ab
        x'' = _linearSolveGS a' b'' 128
        -- x'' = _xik1 a' b'' 2 1
        a' = cvm a
        b'' = cvm b'
        ab = (a' Data.Matrix.<|> (Data.Matrix.transpose (Data.Matrix.fromLists [b])))
        p = gdf1
        shfst = head . head . toLists
        shfst' = last . head . Data.Matrix.toLists
