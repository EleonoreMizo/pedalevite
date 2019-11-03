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
LIBS:switches
LIBS:74xgxx
LIBS:ac-dc
LIBS:actel
LIBS:allegro
LIBS:Altera
LIBS:analog_devices
LIBS:battery_management
LIBS:bbd
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
LIBS:logo
LIBS:maxim
LIBS:mechanical
LIBS:microchip_dspic33dsc
LIBS:microchip_pic10mcu
LIBS:microchip_pic12mcu
LIBS:microchip_pic16mcu
LIBS:microchip_pic18mcu
LIBS:microchip_pic32mcu
LIBS:motor_drivers
LIBS:motors
LIBS:msp430
LIBS:nordicsemi
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
LIBS:transf
LIBS:ttl_ieee
LIBS:video
LIBS:wiznet
LIBS:Worldsemi
LIBS:Xicor
LIBS:zetex
LIBS:Zilog
LIBS:Symbols_DCDC-ACDC-Converter_RevC_20Jul2012
LIBS:Symbols_EN60617_13Mar2013
LIBS:Symbols_EN60617-10_HF-Radio_DRAFT_12Sep2013
LIBS:Symbols_ICs-Diskrete_RevD10
LIBS:Symbols_ICs-Opto_RevB_16Sep2013
LIBS:Symbols_Microcontroller_Philips-NXP_RevA_06Oct2013
LIBS:Symbols_Socket-DIN41612_RevA
LIBS:Symbols_Transformer-Diskrete_RevA
LIBS:SymbolsSimilarEN60617+oldDIN617-RevE8
LIBS:audio-board-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 7
Title "Pédale Vite v2 — Audio interface"
Date "2019-10-28"
Rev ""
Comp "Laurent DE SORAS"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_01X02 P17
U 1 1 58BC390A
P 3100 3650
F 0 "P17" H 3050 3800 50  0000 C CNN
F 1 "OUT2ASYM" H 2850 3550 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 3100 3650 50  0001 C CNN
F 3 "" H 3100 3650 50  0000 C CNN
	1    3100 3650
	-1   0    0    -1  
$EndComp
$Comp
L CONN_01X02 P16
U 1 1 58BC3904
P 3000 3650
F 0 "P16" H 2950 3800 50  0000 C CNN
F 1 "OUT1ASYM" H 2750 3550 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 3000 3650 50  0001 C CNN
F 3 "" H 3000 3650 50  0000 C CNN
	1    3000 3650
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P15
U 1 1 58BC3860
P 4150 2600
F 0 "P15" H 4100 2750 50  0000 C CNN
F 1 "IN2G" H 4000 2500 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 4150 2600 50  0001 C CNN
F 3 "" H 4150 2600 50  0000 C CNN
	1    4150 2600
	0    1    -1   0   
$EndComp
$Comp
L CONN_01X02 P14
U 1 1 58BC385A
P 1950 2600
F 0 "P14" H 1900 2750 50  0000 C CNN
F 1 "IN1G" H 1800 2500 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 1950 2600 50  0001 C CNN
F 3 "" H 1950 2600 50  0000 C CNN
	1    1950 2600
	0    -1   -1   0   
$EndComp
$Comp
L CONN_01X02 P9
U 1 1 58BC3745
P 4200 1250
F 0 "P9" H 4150 1400 50  0000 C CNN
F 1 "IN2SWA" H 4000 1150 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 4200 1250 50  0001 C CNN
F 3 "" H 4200 1250 50  0000 C CNN
	1    4200 1250
	-1   0    0    -1  
$EndComp
$Comp
L CONN_01X02 P8
U 1 1 58BC36D9
P 2000 1250
F 0 "P8" H 1950 1400 50  0000 C CNN
F 1 "IN1SWA" H 1800 1150 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 2000 1250 50  0001 C CNN
F 3 "" H 2000 1250 50  0000 C CNN
	1    2000 1250
	-1   0    0    -1  
$EndComp
$Comp
L CONN_02X03 P7
U 1 1 58BC3653
P 4150 1700
F 0 "P7" H 4150 1900 50  0000 C CNN
F 1 "IN2TRS" H 3900 1550 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x03_Pitch2.54mm" H 4150 500 50  0001 C CNN
F 3 "" H 4150 500 50  0000 C CNN
	1    4150 1700
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X03 P6
U 1 1 58BC34CB
P 1950 1700
F 0 "P6" H 1950 1900 50  0000 C CNN
F 1 "IN1TRS" H 1700 1550 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x03_Pitch2.54mm" H 1950 500 50  0001 C CNN
F 3 "" H 1950 500 50  0000 C CNN
	1    1950 1700
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X03 P19
U 1 1 58BC33F1
P 3100 4100
F 0 "P19" H 3050 4300 50  0000 C CNN
F 1 "OUT2XLR" H 2850 3950 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 3100 4100 50  0001 C CNN
F 3 "" H 3100 4100 50  0000 C CNN
	1    3100 4100
	-1   0    0    -1  
$EndComp
$Comp
L CONN_01X03 P18
U 1 1 58BC3378
P 3000 4100
F 0 "P18" H 2950 4300 50  0000 C CNN
F 1 "OUT1XLR" H 2750 3950 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 3000 4100 50  0001 C CNN
F 3 "" H 3000 4100 50  0000 C CNN
	1    3000 4100
	1    0    0    -1  
$EndComp
Text GLabel 3500 4200 2    60   Input ~ 0
OUT2XLR3
Text GLabel 3500 4100 2    60   Input ~ 0
OUT2XLR2
Text GLabel 3500 3700 2    60   Input ~ 0
OUT2ASYMT
Text GLabel 2600 4200 0    60   Input ~ 0
OUT1XLR3
Text GLabel 2600 4100 0    60   Input ~ 0
OUT1XLR2
Text GLabel 2600 3700 0    60   Input ~ 0
OUT1ASYMT
Text GLabel 4100 3000 3    60   Input ~ 0
IN2G2
Text GLabel 4200 3000 3    60   Input ~ 0
IN2G1
Text GLabel 4600 2300 2    60   Input ~ 0
IN2SWB6
Text GLabel 4600 2200 2    60   Input ~ 0
IN2SWB5
Text GLabel 4600 2100 2    60   Input ~ 0
IN2SWB4
Text GLabel 3700 2300 0    60   Input ~ 0
IN2SWB3
Text GLabel 3700 2200 0    60   Input ~ 0
IN2SWB2
Text GLabel 3700 2100 0    60   Input ~ 0
IN2SWB1
Text GLabel 4600 1300 2    60   Input ~ 0
IN2SWA2
Text GLabel 4600 1200 2    60   Input ~ 0
IN2SWA1
Text GLabel 2000 3000 3    60   Input ~ 0
IN1G2
Text GLabel 1900 3000 3    60   Input ~ 0
IN1G1
Text GLabel 2400 2300 2    60   Input ~ 0
IN1SWB6
Text GLabel 2400 2200 2    60   Input ~ 0
IN1SWB5
Text GLabel 2400 2100 2    60   Input ~ 0
IN1SWB4
Text GLabel 1500 2300 0    60   Input ~ 0
IN1SWB3
Text GLabel 1500 2200 0    60   Input ~ 0
IN1SWB2
Text GLabel 1500 2100 0    60   Input ~ 0
IN1SWB1
Text GLabel 2400 1300 2    60   Input ~ 0
IN1SWA2
Text GLabel 2400 1200 2    60   Input ~ 0
IN1SWA1
$Comp
L CONN_01X03 P5
U 1 1 58BC244E
P 4100 1200
F 0 "P5" H 4050 1400 50  0000 C CNN
F 1 "IN2XLR" H 3900 1050 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 4100 1200 50  0001 C CNN
F 3 "" H 4100 1200 50  0000 C CNN
	1    4100 1200
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X03 P4
U 1 1 58BC2330
P 1900 1200
F 0 "P4" H 1850 1400 50  0000 C CNN
F 1 "IN1XLR" H 1700 1050 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 1900 1200 50  0001 C CNN
F 3 "" H 1900 1200 50  0000 C CNN
	1    1900 1200
	1    0    0    -1  
$EndComp
Text GLabel 4600 1800 2    60   Input ~ 0
IN2TRSRN
Text GLabel 4600 1700 2    60   Input ~ 0
IN2TRSTN
Text GLabel 3700 1800 0    60   Input ~ 0
IN2TRSR
Text GLabel 3700 1700 0    60   Input ~ 0
IN2TRST
Text GLabel 2400 1800 2    60   Input ~ 0
IN1TRSRN
Text GLabel 2400 1700 2    60   Input ~ 0
IN1TRSTN
Text GLabel 1500 1800 0    60   Input ~ 0
IN1TRSR
Text GLabel 1500 1700 0    60   Input ~ 0
IN1TRST
Text GLabel 3700 1300 0    60   Input ~ 0
IN2XLR3
Text GLabel 3700 1200 0    60   Input ~ 0
IN2XLR2
Text GLabel 1500 1300 0    60   Input ~ 0
IN1XLR3
Text GLabel 1500 1200 0    60   Input ~ 0
IN1XLR2
$Comp
L GNDA #PWR01
U 1 1 58D54B66
P 1000 4700
F 0 "#PWR01" H 1000 4450 50  0001 C CNN
F 1 "GNDA" H 1000 4550 50  0000 C CNN
F 2 "" H 1000 4700 50  0000 C CNN
F 3 "" H 1000 4700 50  0000 C CNN
	1    1000 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 3600 2800 3600
Wire Wire Line
	2600 3700 2800 3700
Wire Wire Line
	1000 4000 2800 4000
Wire Wire Line
	2600 4100 2800 4100
Wire Wire Line
	2800 4200 2600 4200
Wire Wire Line
	3500 4200 3300 4200
Wire Wire Line
	3300 4100 3500 4100
Wire Wire Line
	5200 4000 3300 4000
Wire Wire Line
	3300 3700 3500 3700
Wire Wire Line
	5200 3600 3300 3600
Wire Wire Line
	2000 2800 2000 3000
Wire Wire Line
	1900 3000 1900 2800
Wire Wire Line
	4100 2800 4100 3000
Wire Wire Line
	4200 3000 4200 2800
Wire Wire Line
	4400 2300 4600 2300
Wire Wire Line
	4600 2200 4400 2200
Wire Wire Line
	4400 2100 4600 2100
Wire Wire Line
	2400 2300 2200 2300
Wire Wire Line
	2200 2200 2400 2200
Wire Wire Line
	2400 2100 2200 2100
Wire Wire Line
	1500 2300 1700 2300
Wire Wire Line
	1700 2200 1500 2200
Wire Wire Line
	1500 2100 1700 2100
Wire Wire Line
	3900 2300 3700 2300
Wire Wire Line
	3700 2200 3900 2200
Wire Wire Line
	3900 2100 3700 2100
Wire Wire Line
	4600 1300 4400 1300
Wire Wire Line
	4400 1200 4600 1200
Wire Wire Line
	2200 1300 2400 1300
Wire Wire Line
	2400 1200 2200 1200
Wire Wire Line
	4400 1800 4600 1800
Wire Wire Line
	4600 1700 4400 1700
Wire Wire Line
	2200 1800 2400 1800
Wire Wire Line
	2400 1700 2200 1700
Wire Wire Line
	3900 1300 3700 1300
Wire Wire Line
	3700 1200 3900 1200
Wire Wire Line
	3900 1100 3200 1100
Wire Wire Line
	3700 1800 3900 1800
Wire Wire Line
	3900 1700 3700 1700
Wire Wire Line
	1700 1800 1500 1800
Wire Wire Line
	1500 1700 1700 1700
Wire Wire Line
	1500 1300 1700 1300
Wire Wire Line
	1700 1200 1500 1200
Wire Wire Line
	1000 1100 1700 1100
Wire Wire Line
	1000 1100 1000 4700
Wire Wire Line
	5200 4600 1000 4600
Wire Wire Line
	1000 1600 1700 1600
Wire Wire Line
	1000 3400 3000 3400
Wire Wire Line
	3000 3400 3000 1600
Wire Wire Line
	3000 1600 2200 1600
Connection ~ 1000 3400
Wire Wire Line
	5200 1600 5200 4600
Wire Wire Line
	5200 1600 4400 1600
Connection ~ 1000 4600
Wire Wire Line
	3200 3400 5200 3400
Wire Wire Line
	3200 1100 3200 3400
Wire Wire Line
	3200 1600 3900 1600
Connection ~ 5200 3400
Connection ~ 1000 4000
Connection ~ 1000 3600
Connection ~ 5200 3600
Connection ~ 5200 4000
$Comp
L CONN_02X03 P10
U 1 1 58D651CA
P 1950 2200
F 0 "P10" H 1950 2400 50  0000 C CNN
F 1 "IN1SWB" H 1950 2000 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x03_Pitch2.54mm" H 1950 1000 50  0001 C CNN
F 3 "" H 1950 1000 50  0001 C CNN
	1    1950 2200
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X03 P11
U 1 1 58D653BE
P 4150 2200
F 0 "P11" H 4150 2400 50  0000 C CNN
F 1 "IN2SWB" H 4150 2000 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x03_Pitch2.54mm" H 4150 1000 50  0001 C CNN
F 3 "" H 4150 1000 50  0001 C CNN
	1    4150 2200
	1    0    0    -1  
$EndComp
Connection ~ 3200 1600
Connection ~ 1000 1600
$EndSCHEMATC
