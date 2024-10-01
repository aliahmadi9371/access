-- =============================================================================
-- The following directives assign pins to the locations specific for the
-- CY8CKIT-149 kit.
-- =============================================================================

-- === I2C ===
attribute port_location of \I2C_Slave:scl(0)\ : label is "PORT(3,0)";
attribute port_location of \I2C_Slave:sda(0)\ : label is "PORT(3,1)";

-- === LED ===
attribute port_location of Bootloader_StatusLED(0) : label is "PORT(3,4)"; -- BLUE LED