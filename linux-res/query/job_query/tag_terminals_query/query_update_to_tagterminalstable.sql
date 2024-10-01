UPDATE
	%1.tag_TerminalsTable
SET
	%1.tag_TerminalsTable.readerId = %2,
	%1.tag_TerminalsTable.type = %3,
	%1.tag_TerminalsTable.readerExpId = %4,
	%1.tag_TerminalsTable.active = %5,
	%1.tag_TerminalsTable.deactiveByDateTime = %6,
	%1.tag_TerminalsTable.deactiveStartDateTime = %7,
	%1.tag_TerminalsTable.deactiveStopDateTime = %8
WHERE
	%1.tag_TerminalsTable.UUID = '%9'
