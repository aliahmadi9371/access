SELECT
        *
FROM
        %1.tag_TerminalsTable
WHERE
        ISNULL(%1.tag_TerminalsTable.syncedAt) OR
        %1.tag_TerminalsTable.syncedAt < '%2'
ORDER BY
        %1.tag_TerminalsTable.id
DESC
LIMIT
	1
