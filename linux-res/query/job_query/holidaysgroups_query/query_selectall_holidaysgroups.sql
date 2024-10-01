SELECT
        *
FROM
        %1.HolidaysGroupsTable
WHERE
        ISNULL(%1.HolidaysGroupsTable.syncedAt) OR
        %1.HolidaysGroupsTable.syncedAt < '%2'
ORDER BY
        %1.HolidaysGroupsTable.id
DESC
LIMIT
	1
