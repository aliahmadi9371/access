-- 1 --> Database Name

SELECT
	%1.inpsTable.id,
        %1.inpsTable.readerExpId,
        %1.inpsTable.readerId,
        %1.inpsTable.reqDateTime,
        %1.inpsTable.UUID
FROM
        %1.inpsTable
WHERE
        (
		%1.inpsTable.isSent = 0 OR
		ISNULL(%1.inpsTable.isSent)
	) AND
	(
		ISNULL(%1.inpsTable.updatedAt) OR
		TIMESTAMPDIFF(SECOND, %1.inpsTable.updatedAt, NOW()) > 30
	)
ORDER BY
        %1.inpsTable.reqDateTime
DESC
LIMIT
        1
