SELECT
        *
FROM
        %1.HolidaysSetsTable
WHERE
        ISNULL(%1.HolidaysSetsTable.syncedAt) OR
        %1.HolidaysSetsTable.syncedAt < '%2'
ORDER BY
        %1.HolidaysSetsTable.id
DESC
LIMIT
	1
