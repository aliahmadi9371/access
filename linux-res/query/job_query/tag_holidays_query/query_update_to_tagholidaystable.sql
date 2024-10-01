UPDATE
	%1.tag_HolidaysTable
SET
	%1.tag_HolidaysTable.code = %2,
	%1.tag_HolidaysTable.type = %3,
	%1.tag_HolidaysTable.uuid_HolidaysGroupsTableUuid = %4
WHERE
	%1.tag_HolidaysTable.UUID = '%5'
