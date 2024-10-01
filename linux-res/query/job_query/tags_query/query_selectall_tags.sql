SELECT
        *
FROM
        %1.tagsTable
WHERE
        ISNULL(%1.tagsTable.syncedAt) OR
        %1.tagsTable.syncedAt < '%2'
ORDER BY
        %1.tagsTable.id
DESC
LIMIT
	1
