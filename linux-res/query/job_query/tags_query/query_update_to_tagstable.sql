UPDATE
	%1.tagsTable
SET
	%1.tagsTable.code = %2,
	%1.tagsTable.uuidTagged = %3,
	%1.tagsTable.taggedType = %4,
	%1.tagsTable.uuidTag = %5,
	%1.tagsTable.tagType = %6
WHERE
	%1.tagsTable.UUID = '%7'
