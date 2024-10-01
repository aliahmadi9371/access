UPDATE
        %1.HolidaysTable
SET
        %1.HolidaysTable.syncedAt = NOW()
WHERE
        %1.HolidaysTable.UUID = '%2'
