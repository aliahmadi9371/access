SELECT
	*
FROM
	%1.AccessShiftsTable
WHERE
	ISNULL(%1.AccessShiftsTable.syncedAt) OR
	%1.AccessShiftsTable.syncedAt < '%2'
ORDER BY
	%1.AccessShiftsTable.id
DESC
LIMIT
	1
