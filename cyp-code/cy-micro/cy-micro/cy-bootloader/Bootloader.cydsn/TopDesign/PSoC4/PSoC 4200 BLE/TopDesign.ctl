-- =============================================================================
-- The following directives assign pins to the locations specific for the
-- CY8CKIT-042-BLE and CY8CKIT-042-BLE-A kit.
-- =============================================================================

-- === I2C ===
attribute port_location of \I2C_Slave:scl(0)\ : label is "PORT(3,5)";
attribute port_location of \I2C_Slave:sda(0)\ : label is "PORT(3,4)";

-- === RGB LED ===
attribute port_location of Bootloader_StatusLED(0) : label is "PORT(3,7)"; -- BLUE LED