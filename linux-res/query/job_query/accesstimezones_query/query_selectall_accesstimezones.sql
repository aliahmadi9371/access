SELECT
	*
FROM
	%1.AccessTimezonesTable
WHERE
	ISNULL(%1.AccessTimezonesTable.syncedAt) OR
	%1.AccessTimezonesTable.syncedAt < '%2'
ORDER BY
	%1.AccessTimezonesTable.id
DESC
LIMIT
	1
