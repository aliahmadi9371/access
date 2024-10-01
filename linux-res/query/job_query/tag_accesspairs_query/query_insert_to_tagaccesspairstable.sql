INSERT INTO
	%1.tag_AccessPairsTable
	(
		%1.tag_AccessPairsTable.code,
		%1.tag_AccessPairsTable.type,
		%1.tag_AccessPairsTable.uuid__tag_terminalsTableUuid,
		%1.tag_AccessPairsTable.uuid__AccessShiftsTableUuid,
		%1.tag_AccessPairsTable.UUID
	)
	VALUES
	(
		%2,
		%3,
		%4,
		%5,
		'%6'
	)
