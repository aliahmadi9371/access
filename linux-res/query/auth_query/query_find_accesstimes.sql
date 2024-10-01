-- 1: Database Name: IroTeam or ...
-- 2: Day Of Week: Sat, Sun, Mon, Tue, Wed, Thu, Fri, Hol
-- 3: id in tag_UsersTable
-- 4: id in tag_TerminalsTable

SELECT
	%1.TimezonesTable.id,
	%1.TimezonesTable.code,
	%1.TimezonesTable.startTime,
	%1.TimezonesTable.stopTime,
	%1.TimezonesTable.UUID
FROM
	%1.TimezonesTable
INNER JOIN
	%1.AccessTimezonesTable
ON
	%1.TimezonesTable.UUID = %1.AccessTimezonesTable.uuid01__TimezonesTableUuid OR
	%1.TimezonesTable.UUID = %1.AccessTimezonesTable.uuid02__TimezonesTableUuid OR
	%1.TimezonesTable.UUID = %1.AccessTimezonesTable.uuid03__TimezonesTableUuid OR
	%1.TimezonesTable.UUID = %1.AccessTimezonesTable.uuid04__TimezonesTableUuid OR
	%1.TimezonesTable.UUID = %1.AccessTimezonesTable.uuid05__TimezonesTableUuid OR
	%1.TimezonesTable.UUID = %1.AccessTimezonesTable.uuid06__TimezonesTableUuid OR
	%1.TimezonesTable.UUID = %1.AccessTimezonesTable.uuid08__TimezonesTableUuid OR
	%1.TimezonesTable.UUID = %1.AccessTimezonesTable.uuid09__TimezonesTableUuid OR
	%1.TimezonesTable.UUID = %1.AccessTimezonesTable.uuid07__TimezonesTableUuid OR
	%1.TimezonesTable.UUID = %1.AccessTimezonesTable.uuid10__TimezonesTableUuid OR
	%1.TimezonesTable.UUID = %1.AccessTimezonesTable.uuid11__TimezonesTableUuid OR
	%1.TimezonesTable.UUID = %1.AccessTimezonesTable.uuid12__TimezonesTableUuid
INNER JOIN
	%1.AccessShiftsTable
ON
	%1.AccessTimezonesTable.UUID = %1.AccessShiftsTable.uuid%2__AccessTimezonesTableUuid
INNER JOIN
	%1.tag_AccessPairsTable
ON
	%1.AccessShiftsTable.UUID = %1.tag_AccessPairsTable.uuid__AccessShiftsTableUuid
INNER JOIN
	%1.tag_TerminalsTable
ON
	%1.tag_AccessPairsTable.uuid__tag_terminalsTableUuid = %1.tag_TerminalsTable.UUID
INNER JOIN
	%1.tagsTable
ON
	%1.tag_AccessPairsTable.UUID = %1.tagsTable.uuidTag AND
	%1.tag_AccessPairsTable.type = %1.tagsTable.tagType
INNER JOIN
	%1.tag_UsersTable
ON
	%1.tagsTable.uuidTagged = %1.tag_UsersTable.UUID AND
	%1.tagsTable.taggedType = %1.tag_UsersTable.type
WHERE
	%1.tag_UsersTable.UUID = '%3' AND
	%1.tag_TerminalsTable.UUID = '%4'
ORDER BY
	%1.TimezonesTable.id
ASC
