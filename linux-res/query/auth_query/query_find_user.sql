-- 1: Datebase name
-- 2: uniqueId

SELECT
	*
FROM
	%1.tag_UsersTable
WHERE
	%1.tag_UsersTable.uniqueId = %2
LIMIT
	1
