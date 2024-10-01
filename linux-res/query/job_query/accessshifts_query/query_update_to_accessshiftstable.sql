UPDATE
	%1.AccessShiftsTable
SET
	%1.AccessShiftsTable.code = %2,
	%1.AccessShiftsTable.type = %3,
	%1.AccessShiftsTable.uuidSat__AccessTimezonesTableUuid = %4,
	%1.AccessShiftsTable.uuidSun__AccessTimezonesTableUuid = %5,
	%1.AccessShiftsTable.uuidMon__AccessTimezonesTableUuid = %6,
	%1.AccessShiftsTable.uuidTue__AccessTimezonesTableUuid = %7,
	%1.AccessShiftsTable.uuidWed__AccessTimezonesTableUuid = %8,
	%1.AccessShiftsTable.uuidThu__AccessTimezonesTableUuid = %9,
	%1.AccessShiftsTable.uuidFri__AccessTimezonesTableUuid = %10,
	%1.AccessShiftsTable.uuidHol__AccessTimezonesTableUuid = %11
WHERE
	%1.AccessShiftsTable.UUID = '%12'
