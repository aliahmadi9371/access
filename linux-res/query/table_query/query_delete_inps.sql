DELETE
FROM
        %1.inpsTable
WHERE
        %1.inpsTable.reqDateTime <= CURDATE() - INTERVAL 3 DAY AND
        %1.inpsTable.isSent = 1
ORDER BY
        %1.inpsTable.reqDateTime
ASC
LIMIT
        10
