INSERT INTO
	%1.tagsTable
	(
		%1.tagsTable.code,
		%1.tagsTable.uuidTagged,
		%1.tagsTable.taggedType,
		%1.tagsTable.uuidTag,
		%1.tagsTable.tagType,
		%1.tagsTable.UUID
	)
	VALUES
	(
		%2,
		%3,
		%4,
		%5,
		%6,
		'%7'
	)