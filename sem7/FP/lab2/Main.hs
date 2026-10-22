{-# LANGUAGE BangPatterns #-}
module Main (main) where

import qualified Data.Vector.Unboxed as U
import Control.Parallel.Strategies (parListChunk, rdeepseq, withStrategy)
import Control.Exception (evaluate)
import Data.List (foldl')
import GHC.Clock (getMonotonicTime)
import GHC.Conc (getNumCapabilities)
import System.Environment (getArgs)
import Text.Printf (printf)

type Row = U.Vector Double

-- forward elimination
forward :: Bool -> Int -> [Row] -> [Row]
forward _ _ [] = []
forward par chunk rows =
  let (pivot, rest) = pickPivot rows
      p0  = U.head pivot
      len = U.length pivot
      elim r =
        let !f = U.head r / p0
        in U.generate (len - 1) (\j -> r U.! (j + 1) - f * pivot U.! (j + 1))
      rest' | par       = withStrategy (parListChunk chunk rdeepseq) (map elim rest)
            | otherwise = map elim rest
  in pivot : forward par chunk rest'

-- partial pivoting: choose the row with the largest |first element|
pickPivot :: [Row] -> (Row, [Row])
pickPivot rs =
  let best = snd (maximum [ (abs (U.head r), i) | (r, i) <- zip rs [0 :: Int ..] ])
      (a, b : c) = splitAt best rs
  in (b, a ++ c)

-- back substitution: x_i = (b_i - sum a_ij * x_j) / a_ii, from the last row up
backward :: [Row] -> U.Vector Double
backward rows = foldl' step U.empty (reverse rows)
  where
    step !xs r =
      let m = U.length r - 1
          b = U.last r
          s = U.sum (U.zipWith (*) (U.slice 1 (m - 1) r) xs)
      in U.cons ((b - s) / U.head r) xs

solve :: Bool -> Int -> [[Double]] -> [Double] -> [Double]
solve par chunk a b =
  let rows = [ U.fromList (r ++ [bi]) | (r, bi) <- zip a b ]
  in U.toList (backward (forward par chunk rows))

-- deterministic test system with a dominant diagonal (always solvable)
genSystem :: Int -> ([[Double]], [Double])
genSystem n = (a, b)
  where
    lcg :: Int -> Int
    lcg s = (s * 1103515245 + 12345) `mod` 2147483648
    vals = map (\s -> fromIntegral (s `mod` 2001) / 1000 - 1) (drop 1 (iterate lcg 42)) :: [Double]
    (rowsV, restV) = go n vals
    go 0 v = ([], v)
    go k v = let (r, v') = splitAt n v; (rs, v'') = go (k - 1) v' in (r : rs, v'')
    a = [ [ if i == j then x + fromIntegral n else x | (j, x) <- zip [0 ..] r ]
        | (i, r) <- zip [0 :: Int ..] rowsV ]
    b = take n restV

-- maximum residual max |Ax - b|.
residual :: [[Double]] -> [Double] -> [Double] -> Double
residual a b x = maximum [ abs (sum (zipWith (*) r x) - bi) | (r, bi) <- zip a b ]

timed :: String -> IO a -> IO (a, Double)
timed name act = do
  t0 <- getMonotonicTime
  r  <- act
  t1 <- getMonotonicTime
  printf "%-12s %.3f s\n" name (t1 - t0)
  return (r, t1 - t0)

main :: IO ()
main = do
  args <- getArgs
  let n = case args of (s : _) -> read s; _ -> 600
  caps <- getNumCapabilities

  -- correctness check on a small system
  let a0 = [[2, 1, -1], [-3, -1, 2], [-2, 1, 2]]
      b0 = [8, -11, -3]
  putStrLn ("3x3 check (expected [2,3,-1]): " ++ show (solve True 1 a0 b0))

  -- benchmark: sequential vs parallel.
  let (a, b) = genSystem n
  _ <- evaluate (sum (map sum a) + sum b)
  printf "n = %d, threads (-N) = %d\n" n caps
  (xs, _) <- timed "sequential" (evaluate (forceList (solve False 1 a b)))
  (xp, _) <- timed "parallel"   (evaluate (forceList (solve True 8 a b)))
  printf "residual (sequential) = %.3e\n" (residual a b xs)
  printf "residual (parallel)   = %.3e\n" (residual a b xp)
  where forceList l = sum l `seq` l