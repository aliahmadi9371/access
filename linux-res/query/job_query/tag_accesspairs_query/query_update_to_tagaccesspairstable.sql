UPDATE
	%1.tag_AccessPairsTable
SET
	%1.tag_AccessPairsTable.code = %2,
	%1.tag_AccessPairsTable.type = %3,
	%1.tag_AccessPairsTable.uuid__tag_terminalsTableUuid = %4,
	%1.tag_AccessPairsTable.uuid__AccessShiftsTableUuid = %5
WHERE
	%1.tag_AccessPairsTable.UUID = '%6'
