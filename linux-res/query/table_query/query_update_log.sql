UPDATE 
	%1.logsTable
SET
	%1.logsTable.isSent = 1
WHERE
	%1.logsTable.UUID = '%2'
