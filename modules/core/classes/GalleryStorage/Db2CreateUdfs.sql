CREATE FUNCTION BIT_AND (X SMALLINT, Y SMALLINT) RETURNS INTEGER
BEGIN ATOMIC
-- Author: Larry Menard, 11/2005
-- AND two bits
IF x = 1  AND y = 1 THEN
  RETURN 1;
ELSE 
  RETURN 0;
END IF;
END;

CREATE FUNCTION G2_BIT_AND (BITSTRING1 CHAR(32), BITSTRING2 CHAR(32)) RETURNS CHAR(32)
BEGIN ATOMIC
-- Author: Larry Menard, 11/2005
-- Bitwise AND (two streams of 32 bits)
-- Declarations	
DECLARE counter INTEGER;
DECLARE tempint INTEGER;
DECLARE retval CHAR(32);
-- AND bit by bit
SET counter = 1;
WHILE counter <= 32 DO
  SET tempint = BIT_AND(CAST(SUBSTR(BITSTRING1, counter, 1) AS SMALLINT),
                        CAST(SUBSTR(BITSTRING2, counter, 1) AS SMALLINT));
  IF counter = 1 THEN
    SET retval = CAST(tempint AS CHAR(1));
  ELSE
    SET retval = SUBSTR(retval, 1, counter - 1) || CAST (tempint AS CHAR(1));
  END IF;
  SET counter = counter + 1;
END WHILE;
RETURN retval;
END;

CREATE FUNCTION G2_LIKE (subject VARCHAR(8000), pattern VARCHAR(8000)) RETURNS INTEGER
BEGIN ATOMIC
--
-- Author: Larry Menard, 11/2005
--
-- G2_LIKE()
--
-- A UDF to perform LIKE() processing unencumbered by the limitations of DB2's LIKE().
-- DB2's LIKE() doesn't support column values or concatenation of non-literals.
--
-- This UDF assumes the concatenation of the pattern parts is done on the invocation of
-- this function.  Alternatively, this function would have to be able to handle varying
-- numbers of args.
--
-- This UDF doesn't currently support escape character.
--
-- Wildcards supported:
--
--   '%' = 0 or more characters
--
--   '_' = 1 character
--
DECLARE subject_position, pattern_position,
        percent_position, underscore_position,
        pattern_chunk_end, rc, character_required INTEGER;
DECLARE next_chunk VARCHAR(8000);
-- Get rid of a few oddball cases (when no subject string provided) right off the bat
IF LENGTH(subject) = 0 AND LENGTH(pattern) = 0  -- ('', '')
THEN
  RETURN 1;
END IF;
IF (LENGTH(subject) = 0) AND (SUBSTR(pattern, 1, 1) != '%')  -- ('', 'x%')
THEN
  RETURN 0;
END IF;
-- Oddballs are out of the way, we know that a subject line exists.  Let's rock.
SET subject_position = 1;
SET pattern_position = 1;
WHILE (pattern_position <= LENGTH(pattern)) DO
  IF subject_position > LENGTH(subject)            -- We've reached the end of the subject line,
  AND SUBSTR(pattern, pattern_position, 1) != '%'  -- and remainder of pattern starts with anything 
  THEN                                             -- other than '%'
    RETURN 0;
  END IF;
  IF (SUBSTR(pattern, pattern_position, 1) = '_') OR
  (SUBSTR(pattern, pattern_position, 1) = SUBSTR(subject, subject_position, 1))
  THEN                                     -- Current two characters match (including '_' wildcard)
    -- nop;
  ELSE                                     -- Current two characters do not match
    IF SUBSTR(pattern, pattern_position, 1) = '%'
    THEN  -- start processing a 'chunk'
      --
      -- Is there more pattern after the current '%'?
      --
      IF LENGTH(pattern) > pattern_position
      THEN
        -- Are there any more wildcards ('%' or '_') in the pattern after this one?
        SET percent_position = LOCATE('%', pattern, pattern_position + 1);
        SET underscore_position = LOCATE('_', pattern, pattern_position + 1);
        --
        -- There is another wildcard later in the pattern,
        --
        IF (percent_position > 0) OR (underscore_position > 0)
        THEN                                       -- Either one or both of them are non-0
          -- get the position of the nearest wildcard
          IF percent_position = 0                  -- Percent is 0, so next wildcard must be '_'
          THEN                                     -- Don't include the wildcard in the chunk     
            SET pattern_chunk_end = underscore_position - 1;
          ELSE
            IF underscore_position = 0             -- Underscore is 0, so next wildcard must be '%'
            THEN                                   -- Don't include the wildcard in the chunk
              SET pattern_chunk_end = percent_position - 1;     
            ELSE                                   -- Neither are 0
              IF percent_position < underscore_position  -- '%' is closer than '_'
              THEN                                       -- Don't include the wildcard in the chunk
                SET pattern_chunk_end = percent_position - 1;    -- '_' is closer than '%'
              ELSE                                               -- Don't include the wildcard
                SET pattern_chunk_end = underscore_position - 1; -- in the chunk
              END IF;
            END IF;
          END IF;
        ELSE
          --
          -- There are no more wildcards in the pattern,
          --
          SET pattern_chunk_end = LENGTH(pattern);
        END IF;
        -- If there's another wildcard immediately following this one, there isn't really a chunk 
        -- to process. If that next wildcard is a '_', we want to simply increment the pattern and 
        -- subject positions. If that next wildcard is a '%', we want to increment only the pattern
        -- pointer. But if that next wildcard is '_' and occurs at the very end of the pattern,that
        -- puts us out of the WHILE loop, so we can't confirm that a valid character follows in the
        -- source.So we set a 'character_required' variable here and test it after the WHILE loop.
        IF pattern_chunk_end = pattern_position
        THEN
          IF SUBSTR(pattern, pattern_position + 1, 1) = '_'
          THEN
            IF pattern_position + 1 = LENGTH(pattern)
            THEN
              SET character_required = 1;
            ELSE
              -- nop;
            END IF;
          ELSE
            IF SUBSTR(pattern, pattern_position + 1, 1) = '%'
            THEN
              -- Since it will be incremented at the end of the while loop, the way
              --  to defeat the incrementing is to decrement it now
              SET subject_position = subject_position - 1;
            END IF;
          END IF;
        ELSE
          -- Do a LOCATE() of the next chunk of the pattern
          --  up to that next wildcard, or if no wildcard, the end of the subject string.
          -- If that LOCATE() returns 0, match obviously failed, so return false.
          SET next_chunk = SUBSTR(pattern,
                                  pattern_position + 1,
                                  pattern_chunk_end - pattern_position);
          SET rc = LOCATE(next_chunk, subject, subject_position);
          IF rc = 0
          THEN
            RETURN 1;
          ELSE
            -- set subject_position to the end of the 'next_chunk' string in the subject string 
            SET subject_position = LOCATE(next_chunk, subject, subject_position) -- (then -1)
                                   + LENGTH(next_chunk)
                                   - 1;  -- it will be incremented at the bottom of the loop
            SET pattern_position = pattern_chunk_end;
          END IF;  -- LOCATE() of chunk = 0
        END IF;  -- consecutive wildcards y/n
      ELSE
        RETURN 1;  -- no more pattern after current '%'
      END IF;  -- more characters after current wildcard
    ELSE
      RETURN 0;
    END IF;  -- if current pattern char is '%'
  END IF;  -- current characters match
  SET subject_position = subject_position + 1;
  SET pattern_position = pattern_position + 1;
END WHILE;
-- We've reached the end of the pattern
IF (subject_position - 1) < LENGTH(subject)  -- there are more characters in the subject
THEN
  IF (character_required = 1)  
  THEN            -- See that really horrible case where there are two consecutive wildcards and
    RETURN 1;     --  the second wildcard is '_' and  that '_' is the last character in the pattern
  ELSE  -- if there is more subject left, that's bad
    RETURN 0;
  END IF;
END IF;      
RETURN 1;
--
-- Testcases for G2_LIKE
--
--
-- Successful 'should match' scenarios
--
-- values g2_like ('The quick brown fox jumped over the lazy dog', 
--                 'The quick % fox jumped over the lazy dog')
-- values g2_like ('The quick brown fox jumped over the lazy dog', '%')
-- values g2_like ('F', '%')@
-- values g2_like ('The quick brown fox jumped over the lazy dog', 
--                 'The quick % fox jum_ed over the lazy dog')
-- values g2_like ('The quick brown fox jumped over the lazy do', 
--                 'The quick ' || '%' || ' fox jum_ed %ver the lazy dog')
-- values g2_like ('The quick brown fox jumped over the lazy d', 
--                 'The quick ' || '%' || ' fox jum_ed %ver the lazy dog')
-- values g2_like ('The quick brown fox jumped over the lazy dog', 
--                 'The quick % fox jum_ed %ver the lazy dog')
-- values g2_like ('The quick brown fox jumped over the lazy dog', 
--                 'The quick ' || '%' || ' fox jum_ed %ver the lazy dog')
-- values g2_like ('The quick brown fox jumped over the lazy dog', 
--                 'The quick ' || '%' || ' fox jum_ed %ver the lazy do%')
-- values g2_like ('The quick brown fox jumped over the lazy dog', 
--                 '%The quick%jumped over the lazy dog')
-- values g2_like ('', '')
-- values g2_like ('The quick brown fox jumped over the lazy dog', 
--                 'The quick % fox jumped over the lazy dog%')
-- values g2_like ('The quick brown fox jumped over the lazy dog',
--                 '%The quick%jumped over the lazy dog%')
-- values g2_like ('The quick brown fox jumped over the lazy dog', 
--                 'The quick % fox jumped ___r__he lazy dog%')
-- values g2_like ('The quick brown fox jumped over the lazy dog', '_%_')
--
-- Successful 'should not match' scenarios
--
-- values g2_like ('The quick brown fox jumped over the lazy dog', '__')
-- values g2_like ('The quick brown fox jumped over the lazy dog', '')
-- values g2_like ('F', 'Bar')
-- values g2_like ('The quick brown fox jumped over the lazy dog',
--                 '_The quick%jumped over the lazy dog')
-- values g2_like ('', 'Bar')
-- values g2_like ('The quick brown fox jumped over the lazy dog', 
--                 'The quick%jumped over the lazy dog_')         
END;

CREATE FUNCTION G2_BIT_OR (INTEGER, VARCHAR(32))
 EXTERNAL NAME 'g2_db2_jar:g2_db2_bit_or!g2_db2_bit_or'
 RETURNS VARCHAR(32) FOR BIT DATA
 RETURNS NULL ON NULL INPUT
 FENCED
 NOT VARIANT
 NO SQL
 PARAMETER STYLE DB2GENERAL
 LANGUAGE JAVA
 NO EXTERNAL ACTION
 FINAL CALL
 DISALLOW PARALLEL
