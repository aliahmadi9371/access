UPDATE
        %1.HolidaysGroupsTable
SET
        %1.HolidaysGroupsTable.syncedAt = NOW()
WHERE
        %1.HolidaysGroupsTable.UUID = '%2'
