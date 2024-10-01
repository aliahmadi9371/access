-- 1: Database Name: IroTeam or ...
-- 2: expId in tag_TerminalsTable
-- 3: readerId in tag_TerminalsTable

SELECT
	*
FROM
	%1.tag_TerminalsTable
WHERE
	%1.tag_TerminalsTable.readerId = %2 AND
	%1.tag_TerminalsTable.readerExpId = %3
LIMIT
	1
