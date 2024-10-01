UPDATE
	%1.tag_UsersTable
SET
	%1.tag_UsersTable.uniqueId = %2,
	%1.tag_UsersTable.type = %3,
	%1.tag_UsersTable.active = %4,
	%1.tag_UsersTable.creditStartDateTime = %5,
	%1.tag_UsersTable.creditStopDateTime = %6,
	%1.tag_UsersTable.deactiveByDateTime = %7,
	%1.tag_UsersTable.deactiveStartDateTime = %8,
	%1.tag_UsersTable.deactiveStopDateTime = %9
WHERE
	%1.tag_UsersTable.UUID = '%10'
