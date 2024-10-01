UPDATE
	%1.HolidaysGroupsTable
SET
	%1.HolidaysGroupsTable.code = %2,
	%1.HolidaysGroupsTable.uuid01_HolidaysSetsTableUuid = %3,
	%1.HolidaysGroupsTable.uuid02_HolidaysSetsTableUuid = %4,
	%1.HolidaysGroupsTable.uuid03_HolidaysSetsTableUuid = %5,
	%1.HolidaysGroupsTable.uuid04_HolidaysSetsTableUuid = %6,
	%1.HolidaysGroupsTable.uuid05_HolidaysSetsTableUuid = %7
WHERE
	%1.HolidaysGroupsTable.UUID = '%8'
