INSERT INTO
	%1.tag_HolidaysTable
	(
		%1.tag_HolidaysTable.code,
		%1.tag_HolidaysTable.type,
		%1.tag_HolidaysTable.uuid_HolidaysGroupsTableUuid,
		%1.tag_HolidaysTable.UUID
	)
	VALUES
	(
		%2,
		%3,
		%4,
		'%5'
	)
