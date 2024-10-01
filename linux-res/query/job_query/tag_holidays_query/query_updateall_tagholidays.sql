UPDATE
        %1.tag_HolidaysTable
SET
        %1.tag_HolidaysTable.syncedAt = NOW()
WHERE
        %1.tag_HolidaysTable.UUID = '%2'
