UPDATE
	%1.logsTable
SET
	%1.logsTable.updatedAt = NOW()
WHERE
	%1.logsTable.id = %2
