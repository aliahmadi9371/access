-- 1: Database Name: IroTeam or ...
-- 2: UUID in tag_UsersTable
-- 3: now date in format "MM-dd"

SELECT 
	%1.HolidaysTable.id,
	%1.HolidaysTable.code,
	%1.HolidaysTable.date,
	%1.HolidaysTable.UUID,
	%1.tag_HolidaysTable.type
FROM
	%1.HolidaysTable
INNER JOIN
	%1.HolidaysSetsTable
ON
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid01_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid02_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid03_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid04_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid05_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid06_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid07_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid08_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid09_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid10_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid11_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid12_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid13_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid14_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid15_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid16_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid17_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid18_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid19_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid20_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid21_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid22_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid23_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid24_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid25_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid26_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid27_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid28_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid29_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid30_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid31_HolidaysTableUuid OR
	%1.HolidaysTable.UUID = %1.HolidaysSetsTable.uuid32_HolidaysTableUuid
INNER JOIN
	%1.HolidaysGroupsTable
ON
	%1.HolidaysSetsTable.UUID = %1.HolidaysGroupsTable.uuid01_HolidaysSetsTableUuid OR
	%1.HolidaysSetsTable.UUID = %1.HolidaysGroupsTable.uuid02_HolidaysSetsTableUuid OR
	%1.HolidaysSetsTable.UUID = %1.HolidaysGroupsTable.uuid03_HolidaysSetsTableUuid OR
	%1.HolidaysSetsTable.UUID = %1.HolidaysGroupsTable.uuid04_HolidaysSetsTableUuid OR
	%1.HolidaysSetsTable.UUID = %1.HolidaysGroupsTable.uuid05_HolidaysSetsTableUuid
INNER JOIN
	%1.tag_HolidaysTable
ON
	%1.HolidaysGroupsTable.UUID = %1.tag_HolidaysTable.uuid_HolidaysGroupsTableUuid
INNER JOIN
	%1.tagsTable
ON
	%1.tag_HolidaysTable.UUID = %1.tagsTable.uuidTag AND
	%1.tag_HolidaysTable.type = %1.tagsTable.tagType
INNER JOIN
	%1.tag_UsersTable
ON
	%1.tagsTable.uuidTagged = %1.tag_UsersTable.UUID AND
	%1.tagsTable.taggedType = %1.tag_UsersTable.type
WHERE
	%1.tag_UsersTable.UUID = '%2' AND
	DATE_FORMAT(%1.HolidaysTable.date, '%m-%d') = '%3'
LIMIT
	1
