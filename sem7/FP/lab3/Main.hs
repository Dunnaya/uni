module Main (main) where

import Data.Char (isAlpha, toLower)
import Data.List (elemIndex, sortBy)
import Data.Maybe (fromMaybe)
import Data.Ord (comparing)
import System.Environment (getArgs)
import System.IO

type Symbol = Char
type Wrd = [Symbol]

newtype Punct = Punct Symbol
  deriving (Show, Eq)

data Token = TWord Wrd | TPunct Punct | TSpace
  deriving (Show, Eq)

newtype Sentence = Sentence [Token]
  deriving (Show, Eq)

type Text = [Sentence]

-- сollapse tabs, line breaks and repeated spaces into single spaces
normalize :: String -> String
normalize = unwords . words . dropWhile (== '\xFEFF')

isApostrophe :: Symbol -> Bool
isApostrophe c = c `elem` "'ʼ’"

-- a word may contain an apostrophe or a hyphen between letters
spanWord :: String -> (Wrd, String)
spanWord (c:cs)
  | isAlpha c = let (w, r) = spanWord cs in (c : w, r)
spanWord (c:d:cs)
  | (isApostrophe c || c == '-') && isAlpha d =
      let (w, r) = spanWord (d : cs) in (c : w, r)
spanWord s = ([], s)

tokenize :: String -> [Token]
tokenize [] = []
tokenize s@(c:cs)
  | isAlpha c = let (w, rest) = spanWord s in TWord w : tokenize rest
  | c == ' ' = TSpace : tokenize cs
  | otherwise = TPunct (Punct c) : tokenize cs

isEnd :: Token -> Bool
isEnd (TPunct (Punct c)) = c `elem` ".!?…"
isEnd _ = False

splitSentences :: [Token] -> Text
splitSentences ts =
  case dropWhile (== TSpace) ts of
    [] -> []
    ts' -> let (s, rest) = takeSentence [] ts' in Sentence s : splitSentences rest
  where
    takeSentence acc [] = (reverse acc, [])
    takeSentence acc (t:rs)
      | isEnd t =
          let (more, rest) = span isEnd rs
           in (reverse acc ++ [t] ++ more, rest)
      | otherwise = takeSentence (t : acc) rs

parseText :: String -> Text
parseText = splitSentences . tokenize . normalize

sentenceWords :: Sentence -> [Wrd]
sentenceWords (Sentence ts) = [w | TWord w <- ts]

textWords :: Text -> [Wrd]
textWords = concatMap sentenceWords

ukrAlphabet :: String
ukrAlphabet = "абвгґдеєжзиіїйклмнопрстуфхцчшщьюя"

vowels :: String
vowels = "аеєиіїоуюяaeiouy"

firstLetter :: Wrd -> Symbol
firstLetter = toLower . head

startsWithVowel :: Wrd -> Bool
startsWithVowel w = not (null w) && firstLetter w `elem` vowels

-- latin letters are ordered after Cyrillic ones
letterOrder :: Symbol -> Int
letterOrder c = fromMaybe (100 + fromEnum c) (elemIndex c ukrAlphabet)

-- sortBy is stable: words with the same first letter keep their text order
task8 :: Text -> [Wrd]
task8 =
  sortBy (comparing (letterOrder . firstLetter))
    . filter startsWithVowel
    . textWords

main :: IO ()
main = do
  args <- getArgs
  let (inPath, outPath) = case args of
        [i, o] -> (i, o)
        [i] -> (i, "result.txt")
        _ -> ("text.txt", "result.txt")
  hSetEncoding stdout utf8
  input <- openFile inPath ReadMode
  hSetEncoding input utf8
  txt <- hGetContents input
  let text = parseText txt
      result = task8 text
  output <- openFile outPath WriteMode
  hSetEncoding output utf8
  mapM_ (hPutStrLn output) result
  hClose output
  putStrLn ("Sentences in text: " ++ show (length text))
  putStrLn ("Words starting with a vowel: " ++ show (length result))
  putStrLn ("Result written to " ++ outPath)
  hClose input