UPDATE
        %1.tag_UsersTable
SET
        %1.tag_UsersTable .syncedAt = NOW()
WHERE
        %1.tag_UsersTable.UUID = '%2'
