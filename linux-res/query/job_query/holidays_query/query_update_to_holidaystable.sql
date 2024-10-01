UPDATE
	%1.HolidaysTable
SET
	%1.HolidaysTable.code = %2,
	%1.HolidaysTable.date = %3
WHERE
	%1.HolidaysTable.UUID = '%4'
