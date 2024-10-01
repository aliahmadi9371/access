UPDATE
	%1.TimezonesTable
SET
	%1.TimezonesTable.syncedAt = NOW()
WHERE
	%1.TimezonesTable.UUID = '%2'
