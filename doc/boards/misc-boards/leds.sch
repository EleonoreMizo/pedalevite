EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 6
Title "Pédale Vite v2 — Auxiliary boards"
Date "2019-10-22"
Rev ""
Comp "Laurent DE SORAS"
Comment1 "Licensed under WTFPL v2 / CC0"
Comment2 ""
Comment3 ""
Comment4 "LED board"
$EndDescr
$Comp
L Device:LED_ALT D100
U 1 1 5D8A5BF4
P 4450 1400
F 0 "D100" H 4450 1500 50  0000 C CNN
F 1 "0 RED" H 4450 1300 50  0000 C CNN
F 2 "LEDs:LED_D5.0mm" H 4450 1400 50  0001 C CNN
F 3 "" H 4450 1400 50  0001 C CNN
	1    4450 1400
	-1   0    0    -1  
$EndComp
$Comp
L Device:LED_ALT D101
U 1 1 5D8A5C38
P 4450 1800
F 0 "D101" H 4450 1900 50  0000 C CNN
F 1 "1 GREEN" H 4450 1700 50  0000 C CNN
F 2 "LEDs:LED_D5.0mm" H 4450 1800 50  0001 C CNN
F 3 "" H 4450 1800 50  0001 C CNN
	1    4450 1800
	-1   0    0    -1  
$EndComp
$Comp
L Device:LED_ALT D102
U 1 1 5D8A5C5A
P 4450 2200
F 0 "D102" H 4450 2300 50  0000 C CNN
F 1 "2 RED" H 4450 2100 50  0000 C CNN
F 2 "LEDs:LED_D5.0mm" H 4450 2200 50  0001 C CNN
F 3 "" H 4450 2200 50  0001 C CNN
	1    4450 2200
	-1   0    0    -1  
$EndComp
$Comp
L conn:CONN_01X04 P100
U 1 1 5D8A6868
P 5500 1550
F 0 "P100" H 5500 1800 50  0000 C CNN
F 1 "LEDS" V 5600 1550 50  0000 C CNN
F 2 "Sockets_MOLEX_KK-System:Socket_MOLEX-KK-RM2-54mm_Lock_4pin_straight" H 5500 1550 50  0001 C CNN
F 3 "" H 5500 1550 50  0000 C CNN
	1    5500 1550
	1    0    0    1   
$EndComp
Wire Wire Line
	5300 1700 5200 1700
Wire Wire Line
	5200 1700 5200 2500
Wire Wire Line
	5200 2500 4100 2500
Wire Wire Line
	4100 2500 4100 1400
Wire Wire Line
	4100 2200 4300 2200
Wire Wire Line
	4100 1800 4300 1800
Connection ~ 4100 2200
Wire Wire Line
	4100 1400 4300 1400
Connection ~ 4100 1800
Wire Wire Line
	4600 1400 5300 1400
Wire Wire Line
	4600 2200 5000 2200
Wire Wire Line
	5000 2200 5000 1600
Wire Wire Line
	5000 1600 5300 1600
Wire Wire Line
	4600 1800 4800 1800
Wire Wire Line
	4800 1800 4800 1500
Wire Wire Line
	4800 1500 5300 1500
$Comp
L misc-boards-rescue:Mounting_Hole MK101
U 1 1 5D8D304D
P 6300 1700
AR Path="/5D8D304D" Ref="MK101"  Part="1" 
AR Path="/5D89DA36/5D8D304D" Ref="MK101"  Part="1" 
F 0 "MK101" H 6300 1900 50  0000 C CNN
F 1 "Mounting_Hole" H 6300 1825 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3" H 6300 1700 50  0001 C CNN
F 3 "" H 6300 1700 50  0001 C CNN
	1    6300 1700
	1    0    0    -1  
$EndComp
$Comp
L misc-boards-rescue:Mounting_Hole MK102
U 1 1 5D8D3054
P 6900 1700
AR Path="/5D8D3054" Ref="MK102"  Part="1" 
AR Path="/5D89DA36/5D8D3054" Ref="MK102"  Part="1" 
F 0 "MK102" H 6900 1900 50  0000 C CNN
F 1 "Mounting_Hole" H 6900 1825 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3" H 6900 1700 50  0001 C CNN
F 3 "" H 6900 1700 50  0001 C CNN
	1    6900 1700
	1    0    0    -1  
$EndComp
$EndSCHEMATC
