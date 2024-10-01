UPDATE
        %1.tag_AccessPairsTable
SET
        %1.tag_AccessPairsTable.syncedAt = NOW()
WHERE
        %1.tag_AccessPairsTable.UUID = '%2'
