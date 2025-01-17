INSERT INTO
	%1.logsTable
		(
			%1.logsTable.userUUID,
			%1.logsTable.userUniqueId,
			%1.logsTable.readerUUID,
			%1.logsTable.readerId,
			%1.logsTable.readerExpId,
			%1.logsTable.reqDateTime,
			%1.logsTable.isHoliday,
			%1.logsTable.errorCode,
			%1.logsTable.errorString,
			%1.logsTable.isSent,
			%1.logsTable.UUID,
			%1.logsTable.isUser,
			%1.logsTable.updatedAt,
			%1.logsTable.isSuccess
		)
	VALUES
		(
			'%2',
			%3,
			'%4',
			%5,
			%6,
			'%7',
			%8,
			%9,
			'%10',
			NULL,
			UUID(),
			%11,
			NULL,
			%12
		)
