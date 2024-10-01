SELECT
        *
FROM
        %1.tag_AccessPairsTable
WHERE
        ISNULL(%1.tag_AccessPairsTable.syncedAt) OR
        %1.tag_AccessPairsTable.syncedAt < '%2'
ORDER BY
        %1.tag_AccessPairsTable.id
DESC
LIMIT
	1
