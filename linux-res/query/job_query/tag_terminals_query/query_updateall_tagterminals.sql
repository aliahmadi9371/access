UPDATE
        %1.tag_TerminalsTable
SET
        %1.tag_TerminalsTable.syncedAt = NOW()
WHERE
        %1.tag_TerminalsTable.UUID = '%2'
