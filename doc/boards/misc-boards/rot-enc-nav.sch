EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 6
Title "Pédale Vite v2 — Auxiliary boards"
Date "2019-10-22"
Rev ""
Comp "Laurent DE SORAS"
Comment1 "Licensed under WTFPL v2 / CC0"
Comment2 ""
Comment3 ""
Comment4 "Rotary encoders for navigation"
$EndDescr
$Comp
L conn:CONN_01X04 P301
U 1 1 5D8A84AF
P 4300 2950
F 0 "P301" H 4300 3200 50  0000 C CNN
F 1 "ROTENC 7 SW" V 4400 2950 50  0000 C CNN
F 2 "Sockets_MOLEX_KK-System:Socket_MOLEX-KK-RM2-54mm_Lock_4pin_straight" H 4300 2950 50  0001 C CNN
F 3 "" H 4300 2950 50  0000 C CNN
	1    4300 2950
	-1   0    0    -1  
$EndComp
$Comp
L conn:CONN_01X04 P300
U 1 1 5D8A84B6
P 4300 1950
F 0 "P300" H 4300 2200 50  0000 C CNN
F 1 "ROTENC 6 SW" V 4400 1950 50  0000 C CNN
F 2 "Sockets_MOLEX_KK-System:Socket_MOLEX-KK-RM2-54mm_Lock_4pin_straight" H 4300 1950 50  0001 C CNN
F 3 "" H 4300 1950 50  0000 C CNN
	1    4300 1950
	-1   0    0    -1  
$EndComp
$Comp
L misc-boards-rescue:Rotary_Encoder_Switch SW300
U 1 1 5D8AE547
P 5300 1900
F 0 "SW300" H 5300 2160 50  0000 C CNN
F 1 "UP-DOWN" H 5300 1640 50  0000 C CNN
F 2 "pedale-vite:RotaryEncoder_Bourns_PEC11R-Switch_Vertical" H 5200 2060 50  0001 C CNN
F 3 "" H 5300 2160 50  0001 C CNN
	1    5300 1900
	1    0    0    -1  
$EndComp
$Comp
L misc-boards-rescue:Rotary_Encoder_Switch SW301
U 1 1 5D8AE58D
P 5300 2900
F 0 "SW301" H 5300 3160 50  0000 C CNN
F 1 "LEFT-RIGHT" H 5300 2640 50  0000 C CNN
F 2 "pedale-vite:RotaryEncoder_Bourns_PEC11R-Switch_Vertical" H 5200 3060 50  0001 C CNN
F 3 "" H 5300 3160 50  0001 C CNN
	1    5300 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 1800 5000 1800
Wire Wire Line
	4500 1900 5000 1900
Wire Wire Line
	4500 2000 5000 2000
Wire Wire Line
	4500 2100 4800 2100
Wire Wire Line
	4800 2100 4800 2300
Wire Wire Line
	4800 2300 5800 2300
Wire Wire Line
	5800 2300 5800 2000
Wire Wire Line
	5800 2000 5600 2000
Wire Wire Line
	4800 1800 4800 1500
Wire Wire Line
	4800 1500 5800 1500
Wire Wire Line
	5800 1500 5800 1800
Wire Wire Line
	5800 1800 5600 1800
Connection ~ 4800 1800
Wire Wire Line
	4500 2800 5000 2800
Wire Wire Line
	4500 2900 5000 2900
Wire Wire Line
	4500 3000 5000 3000
Wire Wire Line
	4500 3100 4800 3100
Wire Wire Line
	4800 3100 4800 3300
Wire Wire Line
	4800 3300 5800 3300
Wire Wire Line
	5800 3300 5800 3000
Wire Wire Line
	5800 3000 5600 3000
Wire Wire Line
	4800 2800 4800 2500
Wire Wire Line
	4800 2500 5800 2500
Wire Wire Line
	5800 2500 5800 2800
Wire Wire Line
	5800 2800 5600 2800
Connection ~ 4800 2800
$EndSCHEMATC
