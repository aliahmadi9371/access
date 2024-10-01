DELETE
FROM
	%1.logsTable
WHERE
	%1.logsTable.reqDateTime <= CURDATE() - INTERVAL 3 DAY AND
	%1.logsTable.isSent = 1
ORDER BY
	%1.logsTable.id
ASC
LIMIT
	10
