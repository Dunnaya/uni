{-# LANGUAGE OverloadedStrings #-}
{-# LANGUAGE ScopedTypeVariables #-}

module Main (main) where

import Control.Exception (SomeException, catch)
import Control.Monad (forM_, unless)
import Data.Int (Int64)
import Data.Proxy (Proxy (..))
import Data.String (fromString)
import Data.Text (Text)
import qualified Data.Text as T
import Database.MySQL.Simple
import System.Environment (lookupEnv)
import System.IO
import Text.Read (readMaybe)

data Student = Student
  { stId :: Int, stName :: Text, stGroup :: Text, stPhone :: Text }

data Teacher = Teacher
  { tId :: Int, tName :: Text, tPosition :: Text, tPhone :: Text }

data Section = Section
  { secId :: Int, secName :: Text, secCoachId :: Int, secCoachName :: Text }

data Schedule = Schedule
  { shId :: Int, shSectionId :: Int, shSectionName :: Text
  , shWeekday :: Text, shStart :: Text, shEnd :: Text, shPlace :: Text }

data Competition = Competition
  { cId :: Int, cTitle :: Text, cSectionId :: Int, cSectionName :: Text
  , cDate :: Text, cPlace :: Text }

class Entity a where
  entityName :: Proxy a -> String
  describe   :: a -> String
  searchE    :: Connection -> String -> IO [a]
  fetchE     :: Connection -> Int -> IO (Maybe a)
  
  promptE    :: Maybe a -> IO a
  insertE    :: Connection -> a -> IO Int64
  updateE    :: Connection -> a -> IO Int64
  deleteE    :: Connection -> Proxy a -> Int -> IO Int64
  
  hint       :: Connection -> Proxy a -> IO ()
  hint _ _ = return ()

instance Entity Student where
  entityName _ = "Students"
  describe s = show (stId s) ++ ". " ++ T.unpack (stName s)
    ++ ", group " ++ T.unpack (stGroup s) ++ ", phone " ++ T.unpack (stPhone s)
  searchE c q = map conv <$> query c
    "SELECT id, full_name, grp, phone FROM students \
    \WHERE full_name LIKE ? OR grp LIKE ? ORDER BY full_name" (like q, like q)
    where conv (i, n, g, p) = Student i n g p
  fetchE c i = firstMay . map conv <$> query c
    "SELECT id, full_name, grp, phone FROM students WHERE id = ?" (Only i)
    where conv (j, n, g, p) = Student j n g p
  promptE old = do
    n <- askReq "Full name" (stName <$> old)
    g <- ask "Group" (stGroup <$> old)
    p <- ask "Phone" (stPhone <$> old)
    return (Student (maybe 0 stId old) n g p)
  insertE c s = execute c
    "INSERT INTO students (full_name, grp, phone) VALUES (?,?,?)"
    (stName s, stGroup s, stPhone s)
  updateE c s = execute c
    "UPDATE students SET full_name = ?, grp = ?, phone = ? WHERE id = ?"
    (stName s, stGroup s, stPhone s, stId s)
  deleteE c _ i = execute c "DELETE FROM students WHERE id = ?" (Only i)

instance Entity Teacher where
  entityName _ = "Teachers"
  describe t = show (tId t) ++ ". " ++ T.unpack (tName t)
    ++ ", " ++ T.unpack (tPosition t) ++ ", phone " ++ T.unpack (tPhone t)
  searchE c q = map conv <$> query c
    "SELECT id, full_name, position, phone FROM teachers \
    \WHERE full_name LIKE ? OR position LIKE ? ORDER BY full_name" (like q, like q)
    where conv (i, n, g, p) = Teacher i n g p
  fetchE c i = firstMay . map conv <$> query c
    "SELECT id, full_name, position, phone FROM teachers WHERE id = ?" (Only i)
    where conv (j, n, g, p) = Teacher j n g p
  promptE old = do
    n <- askReq "Full name" (tName <$> old)
    g <- ask "Position" (tPosition <$> old)
    p <- ask "Phone" (tPhone <$> old)
    return (Teacher (maybe 0 tId old) n g p)
  insertE c t = execute c
    "INSERT INTO teachers (full_name, position, phone) VALUES (?,?,?)"
    (tName t, tPosition t, tPhone t)
  updateE c t = execute c
    "UPDATE teachers SET full_name = ?, position = ?, phone = ? WHERE id = ?"
    (tName t, tPosition t, tPhone t, tId t)
  deleteE c _ i = execute c "DELETE FROM teachers WHERE id = ?" (Only i)

instance Entity Section where
  entityName _ = "Sports sections"
  describe s = show (secId s) ++ ". " ++ T.unpack (secName s)
    ++ " (coach: " ++ T.unpack (secCoachName s) ++ ", id=" ++ show (secCoachId s) ++ ")"
  searchE c q = map conv <$> query c
    "SELECT s.id, s.name, s.coach_id, t.full_name FROM sections s \
    \JOIN teachers t ON t.id = s.coach_id \
    \WHERE s.name LIKE ? OR t.full_name LIKE ? ORDER BY s.name" (like q, like q)
    where conv (i, n, ci, cn) = Section i n ci cn
  fetchE c i = firstMay . map conv <$> query c
    "SELECT s.id, s.name, s.coach_id, t.full_name FROM sections s \
    \JOIN teachers t ON t.id = s.coach_id WHERE s.id = ?" (Only i)
    where conv (j, n, ci, cn) = Section j n ci cn
  promptE old = do
    n  <- askReq "Section name" (secName <$> old)
    ci <- askInt "Coach (teacher) ID" (secCoachId <$> old)
    return (Section (maybe 0 secId old) n ci "")
  insertE c s = execute c
    "INSERT INTO sections (name, coach_id) VALUES (?,?)" (secName s, secCoachId s)
  updateE c s = execute c
    "UPDATE sections SET name = ?, coach_id = ? WHERE id = ?"
    (secName s, secCoachId s, secId s)
  deleteE c _ i = execute c "DELETE FROM sections WHERE id = ?" (Only i)
  hint c _ = showTeachers c

instance Entity Schedule where
  entityName _ = "Section schedule"
  describe h = show (shId h) ++ ". " ++ T.unpack (shSectionName h) ++ ": "
    ++ T.unpack (shWeekday h) ++ " " ++ T.unpack (shStart h) ++ "-" ++ T.unpack (shEnd h)
    ++ ", " ++ T.unpack (shPlace h)
  searchE c q = map conv <$> query c (fromString (scheduleSql ++ "WHERE s.name LIKE ? OR h.weekday LIKE ? OR h.place LIKE ? \
    \ORDER BY s.name, h.start_time")) (like q, like q, like q)
    where conv (i, si, sn, d, a, b, p) = Schedule i si sn d a b p
  fetchE c i = firstMay . map conv <$> query c (fromString (scheduleSql ++ "WHERE h.id = ?")) (Only i)
    where conv (j, si, sn, d, a, b, p) = Schedule j si sn d a b p
  promptE old = do
    si <- askInt "Section ID" (shSectionId <$> old)
    d  <- askReq "Weekday" (shWeekday <$> old)
    a  <- askReq "Start time (HH:MM)" (shStart <$> old)
    b  <- askReq "End time (HH:MM)" (shEnd <$> old)
    p  <- ask "Place" (shPlace <$> old)
    return (Schedule (maybe 0 shId old) si "" d a b p)
  insertE c h = execute c
    "INSERT INTO section_schedule (section_id, weekday, start_time, end_time, place) \
    \VALUES (?,?,?,?,?)" (shSectionId h, shWeekday h, shStart h, shEnd h, shPlace h)
  updateE c h = execute c
    "UPDATE section_schedule SET section_id = ?, weekday = ?, start_time = ?, \
    \end_time = ?, place = ? WHERE id = ?"
    (shSectionId h, shWeekday h, shStart h, shEnd h, shPlace h, shId h)
  deleteE c _ i = execute c "DELETE FROM section_schedule WHERE id = ?" (Only i)
  hint c _ = showSections c

scheduleSql :: String
scheduleSql =
  "SELECT h.id, h.section_id, s.name, h.weekday, \
  \TIME_FORMAT(h.start_time, '%H:%i'), TIME_FORMAT(h.end_time, '%H:%i'), h.place \
  \FROM section_schedule h JOIN sections s ON s.id = h.section_id "

instance Entity Competition where
  entityName _ = "Competitions"
  describe x = show (cId x) ++ ". " ++ T.unpack (cDate x) ++ " - " ++ T.unpack (cTitle x)
    ++ " [" ++ T.unpack (cSectionName x) ++ "], " ++ T.unpack (cPlace x)
  searchE c q = map conv <$> query c (fromString (competitionSql ++ "WHERE c.title LIKE ? OR s.name LIKE ? OR c.place LIKE ? \
    \ORDER BY c.comp_date")) (like q, like q, like q)
    where conv (i, t, si, sn, d, p) = Competition i t si sn d p
  fetchE c i = firstMay . map conv <$> query c (fromString (competitionSql ++ "WHERE c.id = ?")) (Only i)
    where conv (j, t, si, sn, d, p) = Competition j t si sn d p
  promptE old = do
    t  <- askReq "Competition title" (cTitle <$> old)
    si <- askInt "Section ID" (cSectionId <$> old)
    d  <- askReq "Date (YYYY-MM-DD)" (cDate <$> old)
    p  <- ask "Place" (cPlace <$> old)
    return (Competition (maybe 0 cId old) t si "" d p)
  insertE c x = execute c
    "INSERT INTO competitions (title, section_id, comp_date, place) VALUES (?,?,?,?)"
    (cTitle x, cSectionId x, cDate x, cPlace x)
  updateE c x = execute c
    "UPDATE competitions SET title = ?, section_id = ?, comp_date = ?, place = ? WHERE id = ?"
    (cTitle x, cSectionId x, cDate x, cPlace x, cId x)
  deleteE c _ i = execute c "DELETE FROM competitions WHERE id = ?" (Only i)
  hint c _ = showSections c

competitionSql :: String
competitionSql =
  "SELECT c.id, c.title, c.section_id, s.name, DATE_FORMAT(c.comp_date, '%Y-%m-%d'), c.place \
  \FROM competitions c JOIN sections s ON s.id = c.section_id "

crud :: forall a. Entity a => Connection -> Proxy a -> IO ()
crud c p = do
  putStrLn ("\n " ++ entityName p ++ " ")
  putStrLn "1 - Show all\n2 - Search\n3 - Add\n4 - Edit\n5 - Delete\n0 - Back"
  ch <- prompt "Your choice"
  case ch of
    "0" -> return ()
    "1" -> safely (do xs <- searchE c "" :: IO [a]
                      printAll xs) >> crud c p
    "2" -> safely (do q  <- prompt "Search for"
                      xs <- searchE c q :: IO [a]
                      printAll xs) >> crud c p
    "3" -> safely (do hint c p
                      x <- promptE (Nothing :: Maybe a)
                      n <- insertE c x
                      putStrLn ("Rows added: " ++ show n)) >> crud c p
    "4" -> safely (do i  <- askInt "ID of the record to edit" Nothing
                      mx <- fetchE c i :: IO (Maybe a)
                      case mx of
                        Nothing  -> putStrLn "Record not found."
                        Just old -> do
                          putStrLn ("Current record: " ++ describe old)
                          putStrLn "(Press Enter to keep the current value)"
                          hint c p
                          new <- promptE (Just old)
                          n <- updateE c new
                          putStrLn ("Rows updated: " ++ show n)) >> crud c p
    "5" -> safely (do i <- askInt "ID of the record to delete" Nothing
                      n <- deleteE c p i
                      putStrLn ("Rows deleted: " ++ show n)) >> crud c p
    _   -> putStrLn "Invalid choice." >> crud c p

printAll :: Entity a => [a] -> IO ()
printAll [] = putStrLn "(nothing found)"
printAll xs = mapM_ (putStrLn . describe) xs

-- students & teachers <-> sections.
linksMenu :: Connection -> IO ()
linksMenu c = do
  putStrLn "\n Section members"
  putStrLn "1 - Add a student to a section\n2 - Remove a student from a section"
  putStrLn "3 - Add a teacher to a section\n4 - Remove a teacher from a section"
  putStrLn "5 - Section members\n6 - Sections of a student (search by name)\n0 - Back"
  ch <- prompt "Your choice"
  case ch of
    "0" -> return ()
    "1" -> safely (link "student_sections" "student_id" "Student ID" showStudents) >> linksMenu c
    "2" -> safely (unlink "student_sections" "student_id" "Student ID") >> linksMenu c
    "3" -> safely (link "teacher_sections" "teacher_id" "Teacher ID" showTeachers) >> linksMenu c
    "4" -> safely (unlink "teacher_sections" "teacher_id" "Teacher ID") >> linksMenu c
    "5" -> safely (sectionMembers c) >> linksMenu c
    "6" -> safely (studentSections c) >> linksMenu c
    _   -> putStrLn "Invalid choice." >> linksMenu c
  where
    link :: String -> String -> String -> (Connection -> IO ()) -> IO ()
    link tbl col lbl shower = do
      shower c
      showSections c
      a <- askInt lbl Nothing
      s <- askInt "Section ID" Nothing
      n <- execute c (fromString ("INSERT INTO " ++ tbl ++ " (" ++ col ++ ", section_id) VALUES (?,?)")) (a, s)
      putStrLn ("Rows added: " ++ show n)
    unlink tbl col lbl = do
      a <- askInt lbl Nothing
      s <- askInt "Section ID" Nothing
      n <- execute c (fromString ("DELETE FROM " ++ tbl ++ " WHERE " ++ col ++ " = ? AND section_id = ?")) (a, s)
      putStrLn ("Rows deleted: " ++ show n)

sectionMembers :: Connection -> IO ()
sectionMembers c = do
  showSections c
  s <- askInt "Section ID" Nothing
  sts <- query c
    "SELECT st.full_name, st.grp FROM students st \
    \JOIN student_sections ss ON ss.student_id = st.id \
    \WHERE ss.section_id = ? ORDER BY st.full_name" (Only s) :: IO [(Text, Text)]
  ts <- query c
    "SELECT t.full_name, t.position FROM teachers t \
    \JOIN teacher_sections ts ON ts.teacher_id = t.id \
    \WHERE ts.section_id = ? ORDER BY t.full_name" (Only s) :: IO [(Text, Text)]
  putStrLn "Students:"
  if null sts then putStrLn "  (none)" else forM_ sts $ \(n, g) -> putStrLn ("  " ++ T.unpack n ++ ", " ++ T.unpack g)
  putStrLn "Teachers:"
  if null ts then putStrLn "  (none)" else forM_ ts $ \(n, g) -> putStrLn ("  " ++ T.unpack n ++ ", " ++ T.unpack g)

studentSections :: Connection -> IO ()
studentSections c = do
  q <- prompt "Name (or part of it)"
  rows <- query c
    "SELECT st.full_name, sc.name FROM students st \
    \JOIN student_sections ss ON ss.student_id = st.id \
    \JOIN sections sc ON sc.id = ss.section_id \
    \WHERE st.full_name LIKE ? ORDER BY st.full_name, sc.name" (Only (like q)) :: IO [(Text, Text)]
  if null rows then putStrLn "(nothing found)"
  else forM_ rows $ \(n, s) -> putStrLn (T.unpack n ++ " -> " ++ T.unpack s)

reportsMenu :: Connection -> IO ()
reportsMenu c = do
  putStrLn "\n Reports"
  putStrLn "1 - Upcoming competitions\n2 - Number of students per section\n3 - Schedule by weekday\n0 - Back"
  ch <- prompt "Your choice"
  case ch of
    "0" -> return ()
    "1" -> safely upcoming >> reportsMenu c
    "2" -> safely stats >> reportsMenu c
    "3" -> safely byDay >> reportsMenu c
    _   -> putStrLn "Invalid choice." >> reportsMenu c
  where
    upcoming = do
      rows <- query_ c
        "SELECT DATE_FORMAT(c.comp_date, '%Y-%m-%d'), c.title, s.name, c.place \
        \FROM competitions c JOIN sections s ON s.id = c.section_id \
        \WHERE c.comp_date >= CURDATE() ORDER BY c.comp_date" :: IO [(Text, Text, Text, Text)]
      if null rows then putStrLn "(no upcoming competitions)"
      else forM_ rows $ \(d, t, s, p) ->
        putStrLn (T.unpack d ++ " - " ++ T.unpack t ++ " [" ++ T.unpack s ++ "], " ++ T.unpack p)
    stats = do
      rows <- query_ c
        "SELECT s.name, COUNT(ss.student_id) FROM sections s \
        \LEFT JOIN student_sections ss ON ss.section_id = s.id \
        \GROUP BY s.id, s.name ORDER BY s.name" :: IO [(Text, Int)]
      forM_ rows $ \(n, k) -> putStrLn (T.unpack n ++ ": " ++ show k ++ " students")
    byDay = do
      d <- prompt "Weekday (e.g. Tuesday)"
      xs <- searchE c (T.unpack (T.strip (T.pack d))) :: IO [Schedule]
      printAll xs

showTeachers, showSections, showStudents :: Connection -> IO ()
showTeachers c = do
  rows <- query_ c "SELECT id, full_name FROM teachers ORDER BY id" :: IO [(Int, Text)]
  putStrLn ("Teachers: " ++ unwords [show i ++ "=" ++ T.unpack n ++ ";" | (i, n) <- rows])
showSections c = do
  rows <- query_ c "SELECT id, name FROM sections ORDER BY id" :: IO [(Int, Text)]
  putStrLn ("Sections: " ++ unwords [show i ++ "=" ++ T.unpack n ++ ";" | (i, n) <- rows])
showStudents c = do
  rows <- query_ c "SELECT id, full_name FROM students ORDER BY id" :: IO [(Int, Text)]
  putStrLn ("Students: " ++ unwords [show i ++ "=" ++ T.unpack n ++ ";" | (i, n) <- rows])

like :: String -> Text
like q = T.pack ("%" ++ q ++ "%")

firstMay :: [a] -> Maybe a
firstMay (x : _) = Just x
firstMay []      = Nothing

safely :: IO () -> IO ()
safely act = act `catch` \(e :: SomeException) -> putStrLn ("Error: " ++ show e)

prompt :: String -> IO String
prompt label = putStr (label ++ ": ") >> hFlush stdout >> getLine

ask :: String -> Maybe Text -> IO Text
ask label old = do
  s <- prompt (label ++ maybe "" (\o -> " [" ++ T.unpack o ++ "]") old)
  let t = T.strip (T.pack s)
  return (if T.null t then maybe "" id old else t)

askReq :: String -> Maybe Text -> IO Text
askReq label old = do
  t <- ask label old
  if T.null t then putStrLn "This field is required." >> askReq label old else return t

askInt :: String -> Maybe Int -> IO Int
askInt label old = do
  s <- prompt (label ++ maybe "" (\o -> " [" ++ show o ++ "]") old)
  case (s, old) of
    ("", Just o) -> return o
    _ -> case readMaybe s of
           Just n  -> return n
           Nothing -> putStrLn "Please enter an integer." >> askInt label old

-- connection settings come from environment variables with defaults
main :: IO ()
main = do
  hSetEncoding stdout utf8
  hSetEncoding stdin utf8
  hSetBuffering stdout NoBuffering
  host <- maybe "127.0.0.1"   id <$> lookupEnv "DB_HOST"
  port <- maybe 3306 id . (>>= readMaybe) <$> lookupEnv "DB_PORT"
  user <- maybe "sport"       id <$> lookupEnv "DB_USER"
  pass <- maybe "sport123"    id <$> lookupEnv "DB_PASS"
  name <- maybe "sport_faculty" id <$> lookupEnv "DB_NAME"
  conn <- connect defaultConnectInfo
    { connectHost = host, connectPort = port, connectUser = user
    , connectPassword = pass, connectDatabase = name }
  -- should be utf-8
  _ <- execute_ conn "SET NAMES utf8mb4"
  mainMenu conn
  close conn
  putStrLn "Goodbye..."

mainMenu :: Connection -> IO ()
mainMenu c = do
  putStrLn "\n MAIN MENU"
  putStrLn "1 - Students\n2 - Teachers\n3 - Sports sections\n4 - Section schedule"
  putStrLn "5 - Competitions\n6 - Section members\n7 - Reports\n0 - Exit"
  ch <- prompt "Your choice"
  unless (ch == "0") $ do
    case ch of
      "1" -> crud c (Proxy :: Proxy Student)
      "2" -> crud c (Proxy :: Proxy Teacher)
      "3" -> crud c (Proxy :: Proxy Section)
      "4" -> crud c (Proxy :: Proxy Schedule)
      "5" -> crud c (Proxy :: Proxy Competition)
      "6" -> linksMenu c
      "7" -> reportsMenu c
      _   -> putStrLn "Invalid choice."
    mainMenu c