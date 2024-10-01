UPDATE
        %1.tagsTable
SET
        %1.tagsTable.syncedAt = NOW()
WHERE
        %1.tagsTable.UUID = '%2'
