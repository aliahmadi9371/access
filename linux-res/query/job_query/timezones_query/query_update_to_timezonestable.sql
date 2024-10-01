UPDATE
	%1.TimezonesTable
SET
	%1.TimezonesTable.code = %2,
	%1.TimezonesTable.startTime = %3,
	%1.TimezonesTable.stopTime = %4
WHERE
	%1.TimezonesTable.UUID = '%5'
