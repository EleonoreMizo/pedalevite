EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:pedalevite
LIBS:74xgxx
LIBS:ac-dc
LIBS:actel
LIBS:allegro
LIBS:Altera
LIBS:analog_devices
LIBS:battery_management
LIBS:bbd
LIBS:bosch
LIBS:brooktre
LIBS:cmos_ieee
LIBS:dc-dc
LIBS:diode
LIBS:elec-unifil
LIBS:ESD_Protection
LIBS:ftdi
LIBS:gennum
LIBS:graphic
LIBS:hc11
LIBS:ir
LIBS:Lattice
LIBS:leds
LIBS:logo
LIBS:maxim
LIBS:mechanical
LIBS:microchip_dspic33dsc
LIBS:microchip_pic10mcu
LIBS:microchip_pic12mcu
LIBS:microchip_pic16mcu
LIBS:microchip_pic18mcu
LIBS:microchip_pic24mcu
LIBS:microchip_pic32mcu
LIBS:modules
LIBS:motor_drivers
LIBS:motors
LIBS:msp430
LIBS:nordicsemi
LIBS:nxp
LIBS:nxp_armmcu
LIBS:onsemi
LIBS:Oscillators
LIBS:Power_Management
LIBS:powerint
LIBS:pspice
LIBS:references
LIBS:relays
LIBS:rfcom
LIBS:sensors
LIBS:silabs
LIBS:stm8
LIBS:stm32
LIBS:supertex
LIBS:switches
LIBS:transf
LIBS:triac_thyristor
LIBS:ttl_ieee
LIBS:video
LIBS:wiznet
LIBS:Worldsemi
LIBS:Xicor
LIBS:zetex
LIBS:Zilog
LIBS:misc-boards-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 5 6
Title "Pédale Vite v2 — Auxiliary boards"
Date "2019-10-22"
Rev ""
Comp "Laurent DE SORAS"
Comment1 "Licensed under WTFPL v2 / CC0"
Comment2 ""
Comment3 ""
Comment4 "Navigation buttons"
$EndDescr
$Comp
L CONN_02X04 P400
U 1 1 5D8AB70B
P 4450 3050
F 0 "P400" H 4450 3300 50  0000 C CNN
F 1 "NAV SW" H 4450 2800 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x04_Pitch2.54mm" H 4450 1850 50  0001 C CNN
F 3 "" H 4450 1850 50  0000 C CNN
	1    4450 3050
	1    0    0    -1  
$EndComp
Text Notes 3600 2200 0    60   ~ 0
1, 2: GND, 3: Select, 5: Escape,\n4: Up, 6: Down, 8: Left, 7: Right
$Comp
L SW_Push SW402
U 1 1 5D8B4871
P 5300 3000
F 0 "SW402" H 5350 3100 50  0000 L CNN
F 1 "UP" H 5300 2940 50  0000 C CNN
F 2 "pedale-vite:SW_MEC_5GTH9" H 5300 3200 50  0001 C CNN
F 3 "" H 5300 3200 50  0001 C CNN
	1    5300 3000
	1    0    0    -1  
$EndComp
$Comp
L SW_Push SW403
U 1 1 5D8B491C
P 5300 3300
F 0 "SW403" H 5350 3400 50  0000 L CNN
F 1 "DOWN" H 5300 3240 50  0000 C CNN
F 2 "pedale-vite:SW_MEC_5GTH9" H 5300 3500 50  0001 C CNN
F 3 "" H 5300 3500 50  0001 C CNN
	1    5300 3300
	1    0    0    -1  
$EndComp
$Comp
L SW_Push SW404
U 1 1 5D8B4945
P 5300 3600
F 0 "SW404" H 5350 3700 50  0000 L CNN
F 1 "LEFT" H 5300 3540 50  0000 C CNN
F 2 "pedale-vite:SW_MEC_5GTH9" H 5300 3800 50  0001 C CNN
F 3 "" H 5300 3800 50  0001 C CNN
	1    5300 3600
	1    0    0    -1  
$EndComp
$Comp
L SW_Push SW400
U 1 1 5D8B4971
P 3600 3000
F 0 "SW400" H 3650 3100 50  0000 L CNN
F 1 "SELECT" H 3600 2940 50  0000 C CNN
F 2 "pedale-vite:SW_MEC_5GTH9" H 3600 3200 50  0001 C CNN
F 3 "" H 3600 3200 50  0001 C CNN
	1    3600 3000
	-1   0    0    -1  
$EndComp
$Comp
L SW_Push SW401
U 1 1 5D8B4A24
P 3600 3300
F 0 "SW401" H 3650 3400 50  0000 L CNN
F 1 "ESCAPE" H 3600 3240 50  0000 C CNN
F 2 "pedale-vite:SW_MEC_5GTH9" H 3600 3500 50  0001 C CNN
F 3 "" H 3600 3500 50  0001 C CNN
	1    3600 3300
	-1   0    0    -1  
$EndComp
$Comp
L SW_Push SW405
U 1 1 5D8B4A52
P 3600 3600
F 0 "SW405" H 3650 3700 50  0000 L CNN
F 1 "RIGHT" H 3600 3540 50  0000 C CNN
F 2 "pedale-vite:SW_MEC_5GTH9" H 3600 3800 50  0001 C CNN
F 3 "" H 3600 3800 50  0001 C CNN
	1    3600 3600
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3800 3000 4200 3000
Wire Wire Line
	3800 3300 3900 3300
Wire Wire Line
	3900 3300 3900 3100
Wire Wire Line
	3900 3100 4200 3100
Wire Wire Line
	3800 3600 4000 3600
Wire Wire Line
	4000 3600 4000 3200
Wire Wire Line
	4000 3200 4200 3200
Wire Wire Line
	4700 3000 5100 3000
Wire Wire Line
	4700 3100 5000 3100
Wire Wire Line
	5000 3100 5000 3300
Wire Wire Line
	5000 3300 5100 3300
Wire Wire Line
	4700 3200 4900 3200
Wire Wire Line
	4900 3200 4900 3600
Wire Wire Line
	4900 3600 5100 3600
Wire Wire Line
	4200 2900 4100 2900
Wire Wire Line
	4100 2900 4100 2700
Wire Wire Line
	3300 2700 5600 2700
Wire Wire Line
	4800 2700 4800 2900
Wire Wire Line
	4800 2900 4700 2900
Wire Wire Line
	5600 2700 5600 3600
Wire Wire Line
	5600 3000 5500 3000
Connection ~ 4800 2700
Wire Wire Line
	5600 3300 5500 3300
Connection ~ 5600 3000
Wire Wire Line
	5600 3600 5500 3600
Connection ~ 5600 3300
Wire Wire Line
	3300 2700 3300 3600
Wire Wire Line
	3300 3000 3400 3000
Connection ~ 4100 2700
Wire Wire Line
	3300 3300 3400 3300
Connection ~ 3300 3000
Wire Wire Line
	3300 3600 3400 3600
Connection ~ 3300 3300
$Comp
L Mounting_Hole MK401
U 1 1 5D8D2963
P 6600 2900
F 0 "MK401" H 6600 3100 50  0000 C CNN
F 1 "Mounting_Hole" H 6600 3025 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3" H 6600 2900 50  0001 C CNN
F 3 "" H 6600 2900 50  0001 C CNN
	1    6600 2900
	1    0    0    -1  
$EndComp
$Comp
L Mounting_Hole MK402
U 1 1 5D8D296A
P 7200 2900
F 0 "MK402" H 7200 3100 50  0000 C CNN
F 1 "Mounting_Hole" H 7200 3025 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3" H 7200 2900 50  0001 C CNN
F 3 "" H 7200 2900 50  0001 C CNN
	1    7200 2900
	1    0    0    -1  
$EndComp
$Comp
L Mounting_Hole MK403
U 1 1 5D8D2971
P 6600 3300
F 0 "MK403" H 6600 3500 50  0000 C CNN
F 1 "Mounting_Hole" H 6600 3425 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3" H 6600 3300 50  0001 C CNN
F 3 "" H 6600 3300 50  0001 C CNN
	1    6600 3300
	1    0    0    -1  
$EndComp
$Comp
L Mounting_Hole MK404
U 1 1 5D8D2978
P 7200 3300
F 0 "MK404" H 7200 3500 50  0000 C CNN
F 1 "Mounting_Hole" H 7200 3425 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3" H 7200 3300 50  0001 C CNN
F 3 "" H 7200 3300 50  0001 C CNN
	1    7200 3300
	1    0    0    -1  
$EndComp
$EndSCHEMATC
