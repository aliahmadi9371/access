SELECT
        *
FROM
        %1.tag_UsersTable
WHERE
        ISNULL(%1.tag_UsersTable.syncedAt) OR
        %1.tag_UsersTable.syncedAt < '%2'
ORDER BY
        %1.tag_UsersTable.id
DESC
LIMIT
	1