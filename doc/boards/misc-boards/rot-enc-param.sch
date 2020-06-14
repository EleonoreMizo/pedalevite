EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 6
Title "Pédale Vite v2 — Auxiliary boards"
Date "2019-10-22"
Rev ""
Comp "Laurent DE SORAS"
Comment1 "Licensed under WTFPL v2 / CC0"
Comment2 ""
Comment3 ""
Comment4 "Rotary encoders for parameters"
$EndDescr
$Comp
L conn:CONN_01X03 P204
U 1 1 5D8A82DA
P 4000 5600
F 0 "P204" H 4000 5800 50  0000 C CNN
F 1 "ROTENC 5" V 4100 5600 50  0000 C CNN
F 2 "Sockets_MOLEX_KK-System:Socket_MOLEX-KK-RM2-54mm_Lock_3pin_straight" H 4000 5600 50  0001 C CNN
F 3 "" H 4000 5600 50  0000 C CNN
	1    4000 5600
	-1   0    0    -1  
$EndComp
$Comp
L conn:CONN_01X03 P200
U 1 1 5D8A82E1
P 4000 2800
F 0 "P200" H 4000 3000 50  0000 C CNN
F 1 "ROTENC 1" V 4100 2800 50  0000 C CNN
F 2 "Sockets_MOLEX_KK-System:Socket_MOLEX-KK-RM2-54mm_Lock_3pin_straight" H 4000 2800 50  0001 C CNN
F 3 "" H 4000 2800 50  0000 C CNN
	1    4000 2800
	-1   0    0    -1  
$EndComp
$Comp
L conn:CONN_01X03 P201
U 1 1 5D8A82E8
P 4000 3500
F 0 "P201" H 4000 3700 50  0000 C CNN
F 1 "ROTENC 2" V 4100 3500 50  0000 C CNN
F 2 "Sockets_MOLEX_KK-System:Socket_MOLEX-KK-RM2-54mm_Lock_3pin_straight" H 4000 3500 50  0001 C CNN
F 3 "" H 4000 3500 50  0000 C CNN
	1    4000 3500
	-1   0    0    -1  
$EndComp
$Comp
L conn:CONN_01X03 P202
U 1 1 5D8A82EF
P 4000 4200
F 0 "P202" H 4000 4400 50  0000 C CNN
F 1 "ROTENC 3" V 4100 4200 50  0000 C CNN
F 2 "Sockets_MOLEX_KK-System:Socket_MOLEX-KK-RM2-54mm_Lock_3pin_straight" H 4000 4200 50  0001 C CNN
F 3 "" H 4000 4200 50  0000 C CNN
	1    4000 4200
	-1   0    0    -1  
$EndComp
$Comp
L conn:CONN_01X03 P203
U 1 1 5D8A82F6
P 4000 4900
F 0 "P203" H 4000 5100 50  0000 C CNN
F 1 "ROTENC 4" V 4100 4900 50  0000 C CNN
F 2 "Sockets_MOLEX_KK-System:Socket_MOLEX-KK-RM2-54mm_Lock_3pin_straight" H 4000 4900 50  0001 C CNN
F 3 "" H 4000 4900 50  0000 C CNN
	1    4000 4900
	-1   0    0    -1  
$EndComp
Wire Wire Line
	4200 2700 5300 2700
Wire Wire Line
	4200 2800 5300 2800
Wire Wire Line
	4200 2900 5300 2900
Wire Wire Line
	4200 3400 5300 3400
Wire Wire Line
	5300 3500 4200 3500
Wire Wire Line
	4200 3600 5300 3600
Wire Wire Line
	5300 4100 4200 4100
Wire Wire Line
	4200 4200 5300 4200
Wire Wire Line
	5300 4300 4200 4300
Wire Wire Line
	4200 4800 5300 4800
Wire Wire Line
	5300 4900 4200 4900
Wire Wire Line
	4200 5000 5300 5000
Wire Wire Line
	5300 5500 4200 5500
Wire Wire Line
	4200 5600 5300 5600
Wire Wire Line
	5300 5700 4200 5700
$Comp
L misc-boards-rescue:Rotary_Encoder_Switch SW200
U 1 1 5D8B5CF1
P 5600 2800
F 0 "SW200" H 5600 3060 50  0000 C CNN
F 1 "PARAM 0" H 5600 2540 50  0000 C CNN
F 2 "pedale-vite:RotaryEncoder_Bourns_PEC11R-Switch_Vertical" H 5500 2960 50  0001 C CNN
F 3 "" H 5600 3060 50  0001 C CNN
	1    5600 2800
	1    0    0    -1  
$EndComp
$Comp
L misc-boards-rescue:Rotary_Encoder_Switch SW201
U 1 1 5D8B5E2F
P 5600 3500
F 0 "SW201" H 5600 3760 50  0000 C CNN
F 1 "PARAM 1" H 5600 3240 50  0000 C CNN
F 2 "pedale-vite:RotaryEncoder_Bourns_PEC11R-Switch_Vertical" H 5500 3660 50  0001 C CNN
F 3 "" H 5600 3760 50  0001 C CNN
	1    5600 3500
	1    0    0    -1  
$EndComp
$Comp
L misc-boards-rescue:Rotary_Encoder_Switch SW202
U 1 1 5D8B5E6E
P 5600 4200
F 0 "SW202" H 5600 4460 50  0000 C CNN
F 1 "PARAM 2" H 5600 3940 50  0000 C CNN
F 2 "pedale-vite:RotaryEncoder_Bourns_PEC11R-Switch_Vertical" H 5500 4360 50  0001 C CNN
F 3 "" H 5600 4460 50  0001 C CNN
	1    5600 4200
	1    0    0    -1  
$EndComp
$Comp
L misc-boards-rescue:Rotary_Encoder_Switch SW203
U 1 1 5D8B5EAE
P 5600 4900
F 0 "SW203" H 5600 5160 50  0000 C CNN
F 1 "PARAM 3" H 5600 4640 50  0000 C CNN
F 2 "pedale-vite:RotaryEncoder_Bourns_PEC11R-Switch_Vertical" H 5500 5060 50  0001 C CNN
F 3 "" H 5600 5160 50  0001 C CNN
	1    5600 4900
	1    0    0    -1  
$EndComp
$Comp
L misc-boards-rescue:Rotary_Encoder_Switch SW204
U 1 1 5D8B5EF0
P 5600 5600
F 0 "SW204" H 5600 5860 50  0000 C CNN
F 1 "PARAM 4" H 5600 5340 50  0000 C CNN
F 2 "pedale-vite:RotaryEncoder_Bourns_PEC11R-Switch_Vertical" H 5500 5760 50  0001 C CNN
F 3 "" H 5600 5860 50  0001 C CNN
	1    5600 5600
	1    0    0    -1  
$EndComp
NoConn ~ 5900 2700
NoConn ~ 5900 2900
NoConn ~ 5900 3400
NoConn ~ 5900 3600
NoConn ~ 5900 4100
NoConn ~ 5900 4300
NoConn ~ 5900 4800
NoConn ~ 5900 5000
NoConn ~ 5900 5500
NoConn ~ 5900 5700
$EndSCHEMATC
