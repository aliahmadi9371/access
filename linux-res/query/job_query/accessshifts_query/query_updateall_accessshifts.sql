UPDATE
	%1.AccessShiftsTable
SET
	%1.AccessShiftsTable.syncedAt = NOW()
WHERE
	%1.AccessShiftsTable.UUID = '%2'
