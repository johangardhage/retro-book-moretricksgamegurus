/* listing 11-7 */
/* example compiled sprite */

void  brick(int x,int y, char far *vscreen)
{
asm {
push es
mov ax,[y]
mov bx,320
mul bx
add ax,[x]
les di,[vscreen]
mov di,ax


mov word ptr[es:di+0], 0xDEDD
mov word ptr[es:di+2], 0xDEDD
mov word ptr[es:di+4], 0xDBDD
mov word ptr[es:di+6], 0xDEDD
mov word ptr[es:di+8], 0xDED7
mov word ptr[es:di+10], 0xD4D5
mov word ptr[es:di+12], 0xD9DB
mov word ptr[es:di+14], 0xDDDD
mov word ptr[es:di+320], 0xD9D9
mov word ptr[es:di+322], 0xD9D9
mov word ptr[es:di+324], 0xD9D9
mov word ptr[es:di+326], 0xD9D9
mov word ptr[es:di+328], 0xD9D5
mov word ptr[es:di+330], 0xD4D4
mov word ptr[es:di+332], 0xD6D8
mov word ptr[es:di+334], 0xD8D9
mov word ptr[es:di+640], 0xD5D6
mov word ptr[es:di+642], 0xD6D6
mov word ptr[es:di+644], 0xD78
mov word ptr[es:di+646], 0xD68
mov word ptr[es:di+648], 0x8D6
mov word ptr[es:di+650], 0xD5D5
mov word ptr[es:di+652], 0xD6D7
mov word ptr[es:di+654], 0xD7D6
mov word ptr[es:di+960], 0xD3D5
mov word ptr[es:di+962], 0x8D8
mov word ptr[es:di+964], 0xD8D6
mov word ptr[es:di+966], 0xD48
mov word ptr[es:di+968], 0xD8D9
mov word ptr[es:di+970], 0xD9D9
mov word ptr[es:di+972], 0xD9D9
mov word ptr[es:di+974], 0xD9D6
mov word ptr[es:di+1280], 0xD4D5
mov word ptr[es:di+1282], 0xD97
mov word ptr[es:di+1284], 0xDDD9
mov word ptr[es:di+1286], 0xD7D9
mov word ptr[es:di+1288], 0xDCDB
mov word ptr[es:di+1290], 0xDC7
mov word ptr[es:di+1292], 0x7DB
mov word ptr[es:di+1294], 0xDBD9
mov word ptr[es:di+1600], 0xD4D5
mov word ptr[es:di+1602], 0xD9DB
mov word ptr[es:di+1604], 0xD9D8
mov word ptr[es:di+1606], 0xD7D9
mov word ptr[es:di+1608], 0xDADB
mov word ptr[es:di+1610], 0x77
mov word ptr[es:di+1612], 0x7DB
mov word ptr[es:di+1614], 0xDAD9
mov word ptr[es:di+1920], 0xD4D4
mov word ptr[es:di+1922], 0xD6D8
mov word ptr[es:di+1924], 0xD8D6
mov word ptr[es:di+1926], 0x8D6
mov word ptr[es:di+1928], 0xD7D9
mov word ptr[es:di+1930], 0xD9D9
mov word ptr[es:di+1932], 0xD9D9
mov word ptr[es:di+1934], 0xD8D6
mov word ptr[es:di+2240], 0xD5D5
mov word ptr[es:di+2242], 0xD6D5
mov word ptr[es:di+2244], 0x88
mov word ptr[es:di+2246], 0xD5D5
mov word ptr[es:di+2248], 0xD6D6
mov word ptr[es:di+2250], 0x8D7
mov word ptr[es:di+2252], 0xD8D8
mov word ptr[es:di+2254], 0xD78
mov word ptr[es:di+2560], 0xD9D9
mov word ptr[es:di+2562], 0xD6D6
mov word ptr[es:di+2564], 0xD7D9
mov word ptr[es:di+2566], 0xD9DA
mov word ptr[es:di+2568], 0xD6D4
mov word ptr[es:di+2570], 0x8D9
mov word ptr[es:di+2572], 0xD9D9
mov word ptr[es:di+2574], 0xD9D9
mov word ptr[es:di+2880], 0x7D9
mov word ptr[es:di+2882], 0xD9D8
mov word ptr[es:di+2884], 0xD97
mov word ptr[es:di+2886], 0xDCDA
mov word ptr[es:di+2888], 0xD88
mov word ptr[es:di+2890], 0xD8DB
mov word ptr[es:di+2892], 0x77
mov word ptr[es:di+2894], 0x77
mov word ptr[es:di+3200], 0xD9D8
mov word ptr[es:di+3202], 0xD8D9
mov word ptr[es:di+3204], 0xD8DE
mov word ptr[es:di+3206], 0xDDDD
mov word ptr[es:di+3208], 0xD9D6
mov word ptr[es:di+3210], 0xD97
mov word ptr[es:di+3212], 0xDDDA
mov word ptr[es:di+3214], 0xDADA
mov word ptr[es:di+3520], 0xD8D8
mov word ptr[es:di+3522], 0xD5D5
mov word ptr[es:di+3524], 0xD8D8
mov word ptr[es:di+3526], 0xD8D8
mov word ptr[es:di+3528], 0xD4D5
mov word ptr[es:di+3530], 0xD58
mov word ptr[es:di+3532], 0xD9D8
mov word ptr[es:di+3534], 0xD8D8
mov word ptr[es:di+3840], 0xD7D7
mov word ptr[es:di+3842], 0x8D4
mov word ptr[es:di+3844], 0xD7D8
mov word ptr[es:di+3846], 0xD78
mov word ptr[es:di+3848], 0xD5D4
mov word ptr[es:di+3850], 0xD4D5
mov word ptr[es:di+3852], 0xD6D6
mov word ptr[es:di+3854], 0x8D7
mov word ptr[es:di+4160], 0xD9D8
mov word ptr[es:di+4162], 0xDAD9
mov word ptr[es:di+4164], 0xD9D9
mov word ptr[es:di+4166], 0xD9D9
mov word ptr[es:di+4168], 0xD9D4
mov word ptr[es:di+4170], 0xD3D4
mov word ptr[es:di+4172], 0x8D8
mov word ptr[es:di+4174], 0xD8D8
mov word ptr[es:di+4480], 0xDDDE
mov word ptr[es:di+4482], 0xDDDE
mov word ptr[es:di+4484], 0xDCDE
mov word ptr[es:di+4486], 0xDEDE
mov word ptr[es:di+4488], 0xDDD8
mov word ptr[es:di+4490], 0xD4D5
mov word ptr[es:di+4492], 0xD9DC
mov word ptr[es:di+4494], 0x7DD
mov word ptr[es:di+4800], 0xDEDE
mov word ptr[es:di+4802], 0xDEDE
mov word ptr[es:di+4804], 0xDDDE
mov word ptr[es:di+4806], 0xDEDE
mov word ptr[es:di+4808], 0xDED8
mov word ptr[es:di+4810], 0xD5D5
mov word ptr[es:di+4812], 0xD97
mov word ptr[es:di+4814], 0xDDDE
pop es
}
}
