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
LIBS:psu-2x15v-ac-dc-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Pédale Vite v2 — 2x15V AC to DC converter for the audio interface"
Date "2019-10-28"
Rev "Draft design"
Comp "Laurent DE SORAS"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L D_ALT D2
U 1 1 58B6B9BC
P 1800 4550
F 0 "D2" V 1800 4450 50  0000 C CNN
F 1 "1N4004" V 1900 4600 50  0000 C CNN
F 2 "Diodes_THT:D_DO-41_SOD81_P7.62mm_Horizontal" H 1800 4550 50  0001 C CNN
F 3 "" H 1800 4550 50  0000 C CNN
	1    1800 4550
	0    1    1    0   
$EndComp
$Comp
L D_ALT D1
U 1 1 58B6B9C3
P 1800 4050
F 0 "D1" V 1800 3950 50  0000 C CNN
F 1 "1N4004" V 1900 4100 50  0000 C CNN
F 2 "Diodes_THT:D_DO-41_SOD81_P7.62mm_Horizontal" H 1800 4050 50  0001 C CNN
F 3 "" H 1800 4050 50  0000 C CNN
	1    1800 4050
	0    1    1    0   
$EndComp
$Comp
L D_ALT D4
U 1 1 58B6B9CA
P 2100 4550
F 0 "D4" V 2100 4650 50  0000 C CNN
F 1 "1N4004" V 2000 4400 50  0000 C CNN
F 2 "Diodes_THT:D_DO-41_SOD81_P7.62mm_Horizontal" H 2100 4550 50  0001 C CNN
F 3 "" H 2100 4550 50  0000 C CNN
	1    2100 4550
	0    1    1    0   
$EndComp
$Comp
L D_ALT D3
U 1 1 58B6B9D1
P 2100 4050
F 0 "D3" V 2100 4150 50  0000 C CNN
F 1 "1N4004" V 2000 3900 50  0000 C CNN
F 2 "Diodes_THT:D_DO-41_SOD81_P7.62mm_Horizontal" H 2100 4050 50  0001 C CNN
F 3 "" H 2100 4050 50  0000 C CNN
	1    2100 4050
	0    1    1    0   
$EndComp
$Comp
L CP C1
U 1 1 58B6B9DF
P 2600 4050
F 0 "C1" H 2625 4150 50  0000 L CNN
F 1 "2200u 50V" H 2350 3950 50  0000 L CNN
F 2 "Capacitors_THT:CP_Radial_D18.0mm_P7.50mm" H 2638 3900 50  0001 C CNN
F 3 "" H 2600 4050 50  0000 C CNN
	1    2600 4050
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 58B6B9E6
P 2900 4050
F 0 "C3" H 2925 4150 50  0000 L CNN
F 1 "100n" H 2925 3950 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.4mm_W2.1mm_P2.50mm" H 2938 3900 50  0001 C CNN
F 3 "" H 2900 4050 50  0000 C CNN
	1    2900 4050
	1    0    0    -1  
$EndComp
$Comp
L C C4
U 1 1 58B6B9F4
P 2900 4550
F 0 "C4" H 2925 4650 50  0000 L CNN
F 1 "100n" H 2925 4450 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.4mm_W2.1mm_P2.50mm" H 2938 4400 50  0001 C CNN
F 3 "" H 2900 4550 50  0000 C CNN
	1    2900 4550
	1    0    0    -1  
$EndComp
$Comp
L CP C2
U 1 1 58B6B9FB
P 2600 4550
F 0 "C2" H 2625 4650 50  0000 L CNN
F 1 "2200u 50V" H 2350 4450 50  0000 L CNN
F 2 "Capacitors_THT:CP_Radial_D18.0mm_P7.50mm" H 2638 4400 50  0001 C CNN
F 3 "" H 2600 4550 50  0000 C CNN
	1    2600 4550
	1    0    0    -1  
$EndComp
$Comp
L D_ALT D5
U 1 1 58B6BA02
P 3400 3400
F 0 "D5" H 3400 3500 50  0000 C CNN
F 1 "1N4004" H 3400 3300 50  0000 C CNN
F 2 "Diodes_THT:D_DO-41_SOD81_P7.62mm_Horizontal" H 3400 3400 50  0001 C CNN
F 3 "" H 3400 3400 50  0000 C CNN
	1    3400 3400
	1    0    0    -1  
$EndComp
$Comp
L D_ALT D6
U 1 1 58B6BA09
P 3400 5200
F 0 "D6" H 3400 5300 50  0000 C CNN
F 1 "1N4004" H 3400 5100 50  0000 C CNN
F 2 "Diodes_THT:D_DO-41_SOD81_P7.62mm_Horizontal" H 3400 5200 50  0001 C CNN
F 3 "" H 3400 5200 50  0000 C CNN
	1    3400 5200
	-1   0    0    1   
$EndComp
$Comp
L CP C6
U 1 1 58B6BA10
P 3900 4550
F 0 "C6" H 3925 4650 50  0000 L CNN
F 1 "1000u 35V" H 3650 4450 50  0000 L CNN
F 2 "Capacitors_THT:CP_Radial_D12.5mm_P5.00mm" H 3938 4400 50  0001 C CNN
F 3 "" H 3900 4550 50  0000 C CNN
	1    3900 4550
	1    0    0    -1  
$EndComp
$Comp
L LM7815CT IC1
U 1 1 58B6BA29
P 3400 3850
F 0 "IC1" H 3200 4050 50  0000 C CNN
F 1 "LM7815CT" H 3400 4050 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-220_Vertical" H 3400 3950 50  0001 C CIN
F 3 "" H 3400 3850 50  0000 C CNN
	1    3400 3850
	1    0    0    -1  
$EndComp
$Comp
L LM7915CT IC2
U 1 1 58B6BA30
P 3400 4750
F 0 "IC2" H 3200 4550 50  0000 C CNN
F 1 "LM7915CT" H 3400 4550 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-220_Vertical" H 3400 4650 50  0001 C CIN
F 3 "" H 3400 4750 50  0000 C CNN
	1    3400 4750
	1    0    0    -1  
$EndComp
$Comp
L C C7
U 1 1 58B6BA3D
P 4200 4050
F 0 "C7" H 4225 4150 50  0000 L CNN
F 1 "100n" H 4225 3950 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.4mm_W2.1mm_P2.50mm" H 4238 3900 50  0001 C CNN
F 3 "" H 4200 4050 50  0000 C CNN
	1    4200 4050
	1    0    0    -1  
$EndComp
$Comp
L C C8
U 1 1 58B6BA44
P 4200 4550
F 0 "C8" H 4225 4650 50  0000 L CNN
F 1 "100n" H 4225 4450 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D3.4mm_W2.1mm_P2.50mm" H 4238 4400 50  0001 C CNN
F 3 "" H 4200 4550 50  0000 C CNN
	1    4200 4550
	1    0    0    -1  
$EndComp
$Comp
L CP C5
U 1 1 58B6BA4B
P 3900 4050
F 0 "C5" H 3925 4150 50  0000 L CNN
F 1 "1000u 35V" H 3650 3950 50  0000 L CNN
F 2 "Capacitors_THT:CP_Radial_D12.5mm_P5.00mm" H 3938 3900 50  0001 C CNN
F 3 "" H 3900 4050 50  0000 C CNN
	1    3900 4050
	1    0    0    -1  
$EndComp
$Comp
L L L1
U 1 1 58BD4A5D
P 4450 3800
F 0 "L1" V 4400 3800 50  0000 C CNN
F 1 "22u" V 4525 3800 50  0000 C CNN
F 2 "Inductors_NEOSID:Neosid_Inductor_Sd8" H 4450 3800 50  0001 C CNN
F 3 "" H 4450 3800 50  0000 C CNN
	1    4450 3800
	0    1    1    0   
$EndComp
$Comp
L L L2
U 1 1 58BD4B56
P 4450 4800
F 0 "L2" V 4400 4800 50  0000 C CNN
F 1 "22u" V 4525 4800 50  0000 C CNN
F 2 "Inductors_NEOSID:Neosid_Inductor_Sd8" H 4450 4800 50  0001 C CNN
F 3 "" H 4450 4800 50  0000 C CNN
	1    4450 4800
	0    1    1    0   
$EndComp
$Comp
L CONN_01X03 P1
U 1 1 58BE8012
P 4900 4300
F 0 "P1" H 4900 4500 50  0000 C CNN
F 1 "CONN_01X03" V 5000 4300 50  0001 C CNN
F 2 "Connectors_Molex:Molex_KK-6410-03_03x2.54mm_Straight" H 4900 4300 50  0001 C CNN
F 3 "" H 4900 4300 50  0000 C CNN
	1    4900 4300
	1    0    0    -1  
$EndComp
Text Notes 2500 3200 0    60   ~ 0
Power regulation board
$Comp
L PWR_FLAG #FLG01
U 1 1 58C1F4F9
P 1500 5400
F 0 "#FLG01" H 1500 5495 50  0001 C CNN
F 1 "PWR_FLAG" H 1500 5580 50  0000 C CNN
F 2 "" H 1500 5400 50  0000 C CNN
F 3 "" H 1500 5400 50  0000 C CNN
	1    1500 5400
	1    0    0    1   
$EndComp
$Comp
L PWR_FLAG #FLG02
U 1 1 58C1FF8B
P 1500 3900
F 0 "#FLG02" H 1500 3995 50  0001 C CNN
F 1 "PWR_FLAG" H 1500 4080 50  0000 C CNN
F 2 "" H 1500 3900 50  0000 C CNN
F 3 "" H 1500 3900 50  0000 C CNN
	1    1500 3900
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG03
U 1 1 58C21DD3
P 1400 5200
F 0 "#FLG03" H 1400 5295 50  0001 C CNN
F 1 "PWR_FLAG" H 1400 5380 50  0000 C CNN
F 2 "" H 1400 5200 50  0000 C CNN
F 3 "" H 1400 5200 50  0000 C CNN
	1    1400 5200
	1    0    0    1   
$EndComp
$Comp
L Screw_Terminal_1x02 J7
U 1 1 58BDCC9D
P 1100 4500
F 0 "J7" H 1100 4750 50  0000 C TNN
F 1 "Screw_Terminal_1x02" V 950 4500 50  0001 C TNN
F 2 "Connectors_Terminal_Blocks:TerminalBlock_Pheonix_MKDS1.5-2pol" H 1100 4275 50  0001 C CNN
F 3 "" H 1075 4500 50  0001 C CNN
	1    1100 4500
	1    0    0    -1  
$EndComp
$Comp
L Screw_Terminal_1x02 J9
U 1 1 58BDCD66
P 1100 4900
F 0 "J9" H 1100 4700 50  0000 C TNN
F 1 "Screw_Terminal_1x02" V 950 4900 50  0001 C TNN
F 2 "Connectors_Terminal_Blocks:TerminalBlock_Pheonix_MKDS1.5-2pol" H 1100 4675 50  0001 C CNN
F 3 "" H 1075 4900 50  0001 C CNN
	1    1100 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 4400 1800 4200
Wire Wire Line
	2100 4200 2100 4400
Wire Wire Line
	1800 4800 3000 4800
Wire Wire Line
	1800 3800 3000 3800
Wire Wire Line
	2600 3900 2600 3800
Connection ~ 2600 3800
Wire Wire Line
	2900 3400 2900 3900
Connection ~ 2900 3800
Connection ~ 2600 4800
Connection ~ 2900 4800
Wire Wire Line
	3900 3400 3900 3900
Connection ~ 3900 3800
Connection ~ 3400 4300
Wire Wire Line
	2900 3400 3250 3400
Wire Wire Line
	3550 3400 3900 3400
Wire Wire Line
	2600 4200 2600 4400
Connection ~ 2600 4300
Wire Wire Line
	2900 4200 2900 4400
Connection ~ 2900 4300
Wire Wire Line
	3800 4800 4300 4800
Wire Wire Line
	3900 4700 3900 5200
Connection ~ 3900 4800
Wire Wire Line
	3900 5200 3550 5200
Wire Wire Line
	2900 5200 3250 5200
Connection ~ 1800 4400
Connection ~ 2100 4400
Wire Wire Line
	1800 4700 1800 4800
Wire Wire Line
	2100 4800 2100 4700
Wire Wire Line
	1300 4600 1400 4600
Wire Wire Line
	3400 4100 3400 4500
Wire Wire Line
	3900 4200 3900 4400
Connection ~ 3900 4300
Wire Wire Line
	4200 4200 4200 4400
Connection ~ 2100 3800
Wire Wire Line
	2900 4700 2900 5200
Wire Wire Line
	1950 5000 1950 4400
Connection ~ 2100 4800
Wire Wire Line
	2600 4700 2600 4800
Wire Wire Line
	4200 4800 4200 4700
Wire Wire Line
	2100 3900 2100 3800
Wire Wire Line
	1300 5000 1950 5000
Wire Wire Line
	1400 4800 1300 4800
Wire Wire Line
	1400 4300 1400 5200
Connection ~ 1400 4600
Wire Wire Line
	1300 4400 1800 4400
Wire Wire Line
	1800 3900 1800 3800
Wire Wire Line
	4200 3800 4200 3900
Connection ~ 4200 4800
Wire Wire Line
	3800 3800 4300 3800
Connection ~ 4200 3800
Connection ~ 4200 4300
Wire Wire Line
	4700 3800 4600 3800
Wire Wire Line
	4700 4800 4600 4800
Wire Wire Line
	4700 4200 4700 3800
Wire Wire Line
	4700 4800 4700 4400
Wire Wire Line
	1950 4400 2100 4400
Wire Wire Line
	1400 4300 4700 4300
Connection ~ 1400 4800
Wire Wire Line
	1500 5400 1500 5000
Connection ~ 1500 5000
Wire Wire Line
	1500 3900 1500 4400
Connection ~ 1500 4400
Wire Wire Line
	2300 5700 2400 5700
Connection ~ 2300 4300
Wire Wire Line
	2300 5800 2400 5800
Connection ~ 2300 5700
Wire Wire Line
	2300 5500 3000 5500
Wire Wire Line
	3000 5500 3000 5800
Wire Wire Line
	3000 5700 2900 5700
Connection ~ 2300 5500
Wire Wire Line
	3000 5800 2900 5800
Connection ~ 3000 5700
$Comp
L CONN_02X02 P20
U 1 1 58F5EDD3
P 2650 5750
F 0 "P20" H 2650 5900 50  0000 C CNN
F 1 "CONN_02X02" H 2650 5600 50  0001 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x02_Pitch2.54mm" H 2650 4550 50  0001 C CNN
F 3 "" H 2650 4550 50  0001 C CNN
	1    2650 5750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 4300 2300 5800
$EndSCHEMATC
