unsigned char flagtable[128];			//Table to contain key flags

extern void SetKb();				//Declare external procedures
extern void ResetKb();

void main()
{
	SetKb();				//Set up new handler
	do
	{ 
		printf("%d",flagtable[72]);	//Print status of up arrow
	}
	while (flagtable[28]!=1);		//Until Enter is pressed
	ResetKb();				//Restore normal BIOS interrupt
	exit();
}

