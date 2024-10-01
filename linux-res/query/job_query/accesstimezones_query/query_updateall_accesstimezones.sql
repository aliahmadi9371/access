UPDATE
	%1.AccessTimezonesTable
SET
	%1.AccessTimezonesTable.syncedAt = NOW()
WHERE
	%1.AccessTimezonesTable.UUID = '%2'
