INSERT INTO
	%1.TimezonesTable
	(
		%1.TimezonesTable.code,
		%1.TimezonesTable.startTime,
		%1.TimezonesTable.stopTime,
		%1.TimezonesTable.UUID
	)
	VALUES
	(
		%2,
		%3,
		%4,
		'%5'
	)
