UPDATE
        %1.HolidaysSetsTable
SET
        %1.HolidaysSetsTable.syncedAt = NOW()
WHERE
        %1.HolidaysSetsTable.UUID = '%2'
