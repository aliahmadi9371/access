INSERT INTO
	%1.tag_UsersTable
	(
		%1.tag_UsersTable.uniqueId,
		%1.tag_UsersTable.type,
		%1.tag_UsersTable.active,
		%1.tag_UsersTable.creditStartDateTime,
		%1.tag_UsersTable.creditStopDateTime,
		%1.tag_UsersTable.deactiveByDateTime,
		%1.tag_UsersTable.deactiveStartDateTime,
		%1.tag_UsersTable.deactiveStopDateTime,
		%1.tag_UsersTable.UUID
	)
	VALUES
	(
		%2,
		%3,
		%4,
		%5,
		%6,
		%7,
		%8,
		%9,
		'%10'
	)
