SELECT
        *
FROM
        %1.tag_HolidaysTable
WHERE
        ISNULL(%1.tag_HolidaysTable.syncedAt) OR
        %1.tag_HolidaysTable.syncedAt < '%2'
ORDER BY
        %1.tag_HolidaysTable.id
DESC
LIMIT
	1
