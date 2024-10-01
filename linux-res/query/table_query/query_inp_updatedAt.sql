UPDATE
	%1.inpsTable
SET
	%1.inpsTable.updatedAt = NOW()
WHERE
	%1.inpsTable.id = %2
