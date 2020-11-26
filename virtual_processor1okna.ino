int IR,PC,WREG,BREG,PRTB,PRTA, LEADINGEDGECHECK,FZERO;
bool CLOCK, PCLOCK;

void setup() 
{
  // put your setup code here, to run once:
int q;
// PORT B
for(q=5; q<9; q++) { pinMode(q, OUTPUT); }
//PC OUTPUT
pinMode(A0,OUTPUT);
pinMode(A1,OUTPUT);
pinMode(A2,OUTPUT);
pinMode(A3,OUTPUT);
// PORTA
for(q=2; q<5; q++) { pinMode(q, INPUT); }
pinMode(A4,INPUT);
// DATA PROGRAM INPUT
for(q=9; q<13; q++) { pinMode(q, INPUT); }
//CLOCK
pinMode(13,INPUT);
//Reset all Registers
CLOCK=0;
WREG=0;
BREG=0;
IR=0;
FZERO=0;
PORTB=0;
PC=0;
CLOCK=HIGH;
PCLOCK=HIGH;
LEADINGEDGECHECK=0;
//Setup Serial Tracer
Serial.begin(9600);
}



void PCOUTNOW(int a, int b, int c, int d, int f)

{
     bool bits[4];
     int pns[4];
     int i;
     bits[0]=bitRead(f,0);
     bits[1]=bitRead(f,1);
     bits[2]=bitRead(f,2);
     bits[3]=bitRead(f,3);
     pns[0]=a;
     pns[1]=b;
     pns[2]=c;
     pns[3]=d;
    for(i=0; i<4; i++)
      {
      if( bits[i]==HIGH ) { digitalWrite(pns[i],HIGH); } else { digitalWrite(pns[i],LOW); }
      }
return;
}

 

int getdata(int a, int b, int c, int d)
{
int datir=0; 
bool cee[4];
int i;
cee[0]=digitalRead(a);
cee[1]=digitalRead(b);
cee[2]=digitalRead(c);
cee[3]=digitalRead(d);
Serial.print(d); Serial.print(c); Serial.print(b); Serial.print(a); Serial.println();
Serial.print(cee[3]); Serial.print(cee[2]); Serial.print(cee[1]); Serial.print(cee[0]); Serial.println();
    for(i=0; i<4; i++)
      {
     if(cee[i] == HIGH) { bitWrite(datir,i,1); } else { bitWrite(datir,i,0); }
      }
Serial.println(datir);
delay(5000);
return datir;
}

void decodeIR(int PPIR)
{
int pir=PPIR;
int ltrl;
if(PPIR<8)
        {
        int pir, latch;
        int literal;
        latch=52;
  		do{
          CLOCK=digitalRead(13);
		  bitWrite(LEADINGEDGECHECK,1,PCLOCK);
          bitWrite(LEADINGEDGECHECK,0,CLOCK);
          PCLOCK=CLOCK;
          if(LEADINGEDGECHECK==1){ latch=70; }
          Serial.print(LEADINGEDGECHECK); Serial.print("|| "); Serial.print(latch);
          Serial.println(" wait for clk next");
          }while(latch<53);
        ltrl=getdata(9,10,11,12); 
        Serial.print(ltrl); Serial.println("LITERAL ");
        PC++;
        PCOUTNOW(A0,A1,A2,A3,PC);
        } 
else   { ltrl=0; }
instexecute(pir, ltrl);
pcinc(PPIR);
return;
}

 

void pcinc(int cmd)
{
PC++; 
return;
}


void instexecute(int PIR, int ltr)
{
int nltrl;
           int j, cpins;
           int pbpins=5;  // pin for LSB for Port B
switch(PIR)
{
case 0:  // LOAD B
            BREG = ltr;
            Serial.print("LOAD B Selected: Literal is "); Serial.println(ltr);
             break;
case 1: // LOAD A
            WREG=ltr;
            Serial.print("LOAD A Selected: Literal is "); Serial.println(ltr);
             break;
case 2: // ADL
            WREG=WREG + ltr;
            Serial.print("ADD L Selected: Literal is "); Serial.println(ltr);
             break;
case 3: // SUBL
           WREG = ltr - WREG;
           Serial.print("SUBL Selected: Literal is "); Serial.println(ltr);
           break;
case 4: // ANDL
        WREG = ltr && WREG;  
        Serial.print("ANDL Selected: Literal is "); Serial.println(ltr); 
        break;
case 5: // IORL
            WREG = ltr || WREG;  
            Serial.print("IOR Selected: Literal is "); Serial.println(ltr); 
             break;
case 6: // XORL
            int nltrl, nwreg, n1, n2;
            nltrl=!ltr;
           nwreg=!WREG;
           n1 = nltrl &&  WREG;
          n2 = nwreg && ltr;
          WREG = n1 || n2;
          Serial.print("XOR Selected: Literal is "); Serial.println(ltr);
             break;
case 7: // JMP
           PC=ltr - 2;
                       Serial.print("JMP Selected: Literal is "); Serial.println(ltr);
             break;
case 8: // DIVAB
            WREG=BREG/WREG;
                        Serial.println("DIV AB Selected");
             break;
case 9: // MULAB
            WREG=BREG*WREG;
                        Serial.println("MUL AB Selected");
             break;
case 10: //MOV W,PB
             bool pb[4];
           PRTB=WREG;
            for(j=0; j<4; j++)
           {
              pb[j]=bitRead(WREG,j); 
              cpins=pbpins+j;
              Serial.print(pb[j]); Serial.print("--> data  ");
              Serial.print(cpins); Serial.print("--> cpins ");
              if(pb[j]==HIGH) { digitalWrite(cpins,HIGH); 
                                Serial.println("HIGH");} 
                                else  
                                { digitalWrite(cpins,LOW); 
                                 Serial.println("LOW");} 
            }
                        Serial.println("MOV W,PB Selected");
             break;
case 11: // MOV PA,W
          PRTA = getdata(2,3,4,A4);
          WREG=PRTA;
             Serial.println("MOV PA,W Selected");
             break;
case 12: // NOT
           WREG = !WREG;
                       Serial.print("NOT Selected");
             break;
case 13: // ADD AB
           WREG = WREG + BREG;
                       Serial.print("ADD AB Selected");
             break;
case 14: // SUB AB
            WREG = BREG - WREG;
            Serial.print("SUB AB Selected");
             break;
case 15: // NOP
             break;
default: break;
}
if(WREG==0) { FZERO=255; } else {FZERO=0; }
Serial.println();
return;
}

void loop() 
{
CLOCK=digitalRead(13);
bitWrite(LEADINGEDGECHECK,1,PCLOCK);
bitWrite(LEADINGEDGECHECK,0,CLOCK);
PCLOCK=CLOCK;
if(LEADINGEDGECHECK==1) 
                      {
                      PCOUTNOW(A0,A1,A2,A3,PC);
                      IR=getdata(9,10,11,12);
                      decodeIR(IR); 
                      }
Serial.print(LEADINGEDGECHECK); Serial.print("|LB ");
Serial.print(WREG); Serial.print("|WREG ");
Serial.print(BREG); Serial.print("|BREG ");
Serial.print(PC); Serial.print("|PC ");
Serial.print(IR); Serial.print("|IR ");
Serial.print(PRTB); Serial.print("|PRTB ");
Serial.print(PRTA); Serial.print("|PRTA ");
Serial.print(FZERO); Serial.println("|FZ ");
delay(2000);
}