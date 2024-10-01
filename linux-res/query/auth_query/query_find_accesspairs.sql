-- 1: Database Name: IroTeam or ...
-- 2: id in tag_UsersTable
-- 3: id in tag_TerminalsTable

SELECT
	%1.tag_AccessPairsTable.UUID
FROM
	%1.tag_AccessPairsTable
INNER JOIN
	%1.tagsTable
ON
	%1.tag_AccessPairsTable.type = %1.tagsTable.tagType AND
	%1.tag_AccessPairsTable.UUID = %1.tagsTable.uuidTag
INNER JOIN
	%1.tag_UsersTable
ON
	%1.tagsTable.uuidTagged = %1.tag_UsersTable.UUID AND
	%1.tagsTable.taggedType = %1.tag_UsersTable.type
INNER JOIN
	%1.tag_TerminalsTable
ON
	%1.tag_AccessPairsTable.uuid__tag_terminalsTableUuid = %1.tag_TerminalsTable.UUID
WHERE
	%1.tag_UsersTable.UUID = '%2' AND
	%1.tag_TerminalsTable.UUID = '%3'
LIMIT
	1
