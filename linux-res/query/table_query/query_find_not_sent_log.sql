-- 1 --> Database Name

SELECT
        %1.logsTable.id,
        %1.logsTable.userUniqueId,
        %1.logsTable.readerExpId,
        %1.logsTable.readerId,
        %1.logsTable.reqDateTime,
        %1.logsTable.isHoliday,
        %1.logsTable.isUser,
        %1.logsTable.errorCode,
        %1.logsTable.errorString,
        %1.logsTable.UUID,
	%1.logsTable.updatedAt,
	%1.logsTable.isSuccess
FROM
        %1.logsTable
WHERE
        (
		%1.logsTable.isSent = 0 OR
		ISNULL(%1.logsTable.isSent)
	) AND
	(
		ISNULL(%1.logsTable.updatedAt) OR
		TIMESTAMPDIFF(SECOND, %1.logsTable.updatedAt, NOW()) > 30
	)
ORDER BY
        %1.logsTable.reqDateTime
DESC
LIMIT
        1
