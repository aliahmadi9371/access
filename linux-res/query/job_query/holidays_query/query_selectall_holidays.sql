SELECT
        *
FROM
        %1.HolidaysTable
WHERE
        ISNULL(%1.HolidaysTable.syncedAt) OR
        %1.HolidaysTable.syncedAt < '%2'
ORDER BY
        %1.HolidaysTable.id
DESC
LIMIT
	1
