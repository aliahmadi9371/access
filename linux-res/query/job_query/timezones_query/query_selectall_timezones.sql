SELECT
	*
FROM
	%1.TimezonesTable
WHERE
	ISNULL(%1.TimezonesTable.syncedAt) OR
	%1.TimezonesTable.syncedAt < '%2'
ORDER BY
	%1.TimezonesTable.id
DESC
LIMIT
	1
