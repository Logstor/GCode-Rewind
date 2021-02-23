    T0 ; Select head 0
    M302 P1 ; Allow cold extrusion
    G90 ; Absolute position
    M83 ; Relative extrusion
    G21 ; Metric values
    G1 U0 F6000.00 ; Set default printing speed
    
    G1	X435	Y620	Z30	U90	V4.58	E17.67	F6000
G1	X430	Y620	Z30	U90	V4.42	E17.58	F6000
G1	X425	Y620	Z30	U90	V4.26	E17.49	F6000
G1	X420	Y620	Z30	U90	V4.10	E17.40	F6000
G1	X415	Y620	Z30	U90	V3.94	E17.30	F6000
G1	X410	Y620	Z30	U90	V3.77	E17.20	F6000
G1	X405	Y620	Z30	U90	V3.61	E17.09	F6000
G1	X400	Y620	Z30	U90	V3.45	E16.99	F6000
G1	X395	Y620	Z30	U90	V3.29	E16.88	F6000
G1	X390	Y620	Z30	U90	V3.12	E16.77	F6000
G1	X385	Y620	Z30	U90	V2.96	E16.65	F6000
G1	X380	Y620	Z30	U90	V2.80	E16.53	F6000
G1	X375	Y620	Z30	U90	V2.63	E16.41	F6000
G1	X370	Y620	Z30	U90	V2.47	E16.28	F6000
G1	X365	Y620	Z30	U90	V2.30	E16.15	F6000
G1	X360	Y620	Z30	U90	V2.14	E16.02	F6000
G1	X355	Y620	Z30	U90	V1.98	E15.89	F6000
G1	X350	Y620	Z30	U90	V1.81	E15.75	F6000
G1	X345	Y620	Z30	U90	V1.65	E15.60	F6000
G1	X340	Y620	Z30	U90	V1.48	E15.46	F6000
G1	X335	Y620	Z30	U90	V1.32	E15.31	F6000
G1	X330	Y620	Z30	U90	V1.15	E15.16	F6000
G1	X325	Y620	Z30	U90	V0.99	E15.00	F6000
G1	X320	Y620	Z30	U90	V0.82	E14.84	F6000
G1	X315	Y620	Z30	U90	V0.66	E14.68	F6000
G1	X310	Y620	Z30	U90	V0.49	E14.52	F6000
G1	X305	Y620	Z30	U90	V0.33	E14.35	F6000
G1	X300	Y620	Z30	U90	V0.16	E14.17	F6000

G1 V0

; MinE 5 MaxE 30  FullCycle Distance 2000 mm MLML; Layer #0 / Print #0

G1 E6.00 X300 Y618.67 Z30.00
G1 F6000.00
G1 X300 Y620 Z90.00 U90.00 F6000
G92 U0.00 ; Reset rotation
;G1 X0.00 Y0.00 Z90.00
G1 F6000.00
; layer 0, Z = 20.00mm
; tool H20.00 W300.00 - Set visualization width aG1 U0.00 F6000.00 ; Set default print speed
G21 ; Metric values
M83 ; Relative extrusion
G90 ; Absolute position
M302 P1 ; Allow cold extrusion
T0 ; Select head 0
