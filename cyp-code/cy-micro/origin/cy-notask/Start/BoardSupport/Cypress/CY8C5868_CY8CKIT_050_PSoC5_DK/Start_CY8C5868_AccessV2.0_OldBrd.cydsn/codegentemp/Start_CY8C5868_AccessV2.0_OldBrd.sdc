# THIS FILE IS AUTOMATICALLY GENERATED
# Project: C:\Users\ali\Desktop\cyp\cy-notask\Start\BoardSupport\Cypress\CY8C5868_CY8CKIT_050_PSoC5_DK\Start_CY8C5868_AccessV2.0_OldBrd.cydsn\Start_CY8C5868_AccessV2.0_OldBrd.cyprj
# Date: Tue, 23 Jul 2024 05:24:42 GMT
#set_units -time ns
create_clock -name {CyILO} -period 1000000 -waveform {0 500000} [list [get_pins {ClockBlock/ilo}] [get_pins {ClockBlock/clk_100k}] [get_pins {ClockBlock/clk_1k}] [get_pins {ClockBlock/clk_32k}]]
create_clock -name {CyIMO} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/imo}]]
create_clock -name {CyPLL_OUT} -period 20.833333333333332 -waveform {0 10.4166666666667} [list [get_pins {ClockBlock/pllout}]]
create_clock -name {CyMASTER_CLK} -period 20.833333333333332 -waveform {0 10.4166666666667} [list [get_pins {ClockBlock/clk_sync}]]
create_generated_clock -name {CyBUS_CLK} -source [get_pins {ClockBlock/clk_sync}] -edges {1 2 3} [list [get_pins {ClockBlock/clk_bus_glb}]]
create_generated_clock -name {UART_Linux_IntClock} -source [get_pins {ClockBlock/clk_sync}] -edges {1 105 209} -nominal_period 2166.6666666666665 [list [get_pins {ClockBlock/dclk_glb_0}]]
create_generated_clock -name {UART_Debug_IntClock} -source [get_pins {ClockBlock/clk_sync}] -edges {1 105 209} -nominal_period 2166.6666666666665 [list [get_pins {ClockBlock/dclk_glb_1}]]


# Component constraints for C:\Users\ali\Desktop\cyp\cy-notask\Start\BoardSupport\Cypress\CY8C5868_CY8CKIT_050_PSoC5_DK\Start_CY8C5868_AccessV2.0_OldBrd.cydsn\TopDesign\TopDesign.cysch
# Project: C:\Users\ali\Desktop\cyp\cy-notask\Start\BoardSupport\Cypress\CY8C5868_CY8CKIT_050_PSoC5_DK\Start_CY8C5868_AccessV2.0_OldBrd.cydsn\Start_CY8C5868_AccessV2.0_OldBrd.cyprj
# Date: Tue, 23 Jul 2024 05:24:31 GMT
