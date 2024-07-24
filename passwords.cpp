/* Datos necesarios para guardar los codigos: */ 
extern int n_arrows,n_coins,n_keys;
extern int player_max[2],player_energy[2],player_denergy[2];
extern int character;
extern bool live_character[2];
extern bool item[38];
extern bool cross,world_key;
extern bool world_item[10][4];
extern bool world_doors_open[10];
extern int next_world;
extern bool can_revive_character[2];



void digit2BCD(int digit,int BCD[4])
{
	if ((digit&0x08)==0) BCD[0]=0;
					else BCD[0]=1;
	if ((digit&0x04)==0) BCD[1]=0;
					else BCD[1]=1;
	if ((digit&0x02)==0) BCD[2]=0;
					else BCD[2]=1;
	if ((digit&0x01)==0) BCD[3]=0;
					else BCD[3]=1;
} /* digit2BCD */ 


int BCD2digit(int BCD[4])
{
	return BCD[3]+BCD[2]*2+BCD[1]*4+BCD[0]*8;
} /* BCD2digit */ 


void value2block(int val,int block[4])
{
	if ((val&0x10)==0) block[0]=0;
				  else block[0]=1;
	if ((val&0x08)==0) block[1]=0;
				  else block[1]=1;
	if ((val&0x04)==0) block[2]=0;
				  else block[2]=1;
	if ((val&0x02)==0) block[3]=0;
				  else block[3]=1;
	if ((val&0x01)==0) block[4]=0;
				  else block[4]=1;
} /* value2block */ 


int block2value(int block[5])
{
	return block[4]+block[3]*2+block[2]*4+block[1]*8+block[0]*16;
} /* block2value */ 


void writebits(int *bits,int *dest,int start,int len)
{
	int i;

	for(i=0;i<len;i++) {
		dest[start+i]=bits[i];
	} /* for */ 
} /* writebits */ 


void readbits(int *bits,int *data,int start,int len)
{
	int i;

	for(i=0;i<len;i++) {
		bits[i]=data[start+i];
	} /* for */ 
} /* readbits */ 


void bits2characters(int *bits,char *passwd)
{
	int i;
	int pos,val;
	int lbits[5];
	char *str="F3RU72G4X5BKN90PQTHVE6OMWAJ18DSILYCZ";

	for(i=0;i<45;i++) {
		pos=i&0x03;
		readbits(lbits,bits,i*5,5);
		val=block2value(lbits);
		passwd[i]=str[val+3-pos];
	} /* for */ 
} /* bits2characters */ 


void characters2bits(int *bits,char *passwd)
{
	int i,j;
	int pos,val;
	int lbits[5];
	char *str="F3RU72G4X5BKN90PQTHVE6OMWAJ18DSILYCZ";

	for(i=0;i<45;i++) {
		pos=i&0x03;
		val=0;
		for(j=0;j<36;j++) {
			if (str[j]==passwd[i]) val=j+pos-3;
		} /* for */ 
		
		value2block(val,lbits);
		writebits(lbits,bits,i*5,5);
	} /* for */ 
} /* bits2characters */ 


void addchecksum(char *passwd)
{
	char *str="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char *hex="0123456789ABCDEF";

	int sum=0;
	int pos=0,i;


	for(pos=0;pos<43;pos++) {
		for(i=0;i<36;i++) {
			if (passwd[pos]==str[i]) {
				sum+=i+1;
			} /* if */ 
		} /* for */ 
	} /* while */ 


	passwd[43]=hex[(sum>>4)%16];
	passwd[44]=hex[sum%16];
} /* addchecksum */ 


void generatepassword(char *passwd)
{
	int i;
	int val;
	int bits[45*5];
	int BCD[4],block[5];

	for(i=0;i<45*5;i++) bits[i]=0;

	/* GENERAR PASSWORD: */ 
	/* Store arrows, keys and coins: */ 
	/* Arrows: */ 
	val=n_arrows/100;
	digit2BCD(val,BCD);
	writebits(BCD,bits,12,4);
	val=(n_arrows/10)%10;
	digit2BCD(val,BCD);
	writebits(BCD,bits,0,4);
	val=n_arrows%10;
	digit2BCD(val,BCD);
	writebits(BCD,bits,4,4);
	/* Coins: */ 
	val=n_coins/100;
	digit2BCD(val,BCD);
	writebits(BCD,bits,28,4);
	val=(n_coins/10)%10;
	digit2BCD(val,BCD);
	writebits(BCD,bits,16,4);
	val=n_coins%10;
	digit2BCD(val,BCD);
	writebits(BCD,bits,20,4);
	/* keys: */ 
	val=n_keys/100;
	digit2BCD(val,BCD);
	writebits(BCD,bits,44,4);
	val=(n_keys/10)%10;
	digit2BCD(val,BCD);
	writebits(BCD,bits,32,4);
	val=n_keys%10;
	digit2BCD(val,BCD);
	writebits(BCD,bits,36,4);

	/* Store lifemeters: */ 
	/* Popolon: */ 
	val=player_max[0]/16;
	value2block(val,block);
	writebits(block,bits,14*5+4,5);
	value2block(6,block);
	writebits(block,bits,13*5+1,5);
	/* Aphrodite: */ 
	val=player_max[1]/16;
	value2block(val,block);
	writebits(block,bits,11*5+3,5);
	value2block(6,block);
	writebits(block,bits,10*5,5);

	/* Store world information: */ 
	int wstart[10]={85,93,101,109,117,125,133,141,149,157};
	for(i=0;i<10;i++) {
		BCD[0]=world_item[i][2];
		BCD[1]=world_item[i][1];
		BCD[2]=world_item[i][0];
		BCD[3]=world_item[i][3];
		writebits(BCD,bits,wstart[i],4);

		BCD[0]=0;
		if ((next_world-1)==i && !world_doors_open[i]) BCD[2]=1;
									 		      else BCD[2]=0;
		if ((next_world-1)>i) BCD[1]=1;
					     else BCD[1]=0;
		if (world_doors_open[i]) BCD[3]=1;
							else BCD[3]=0;
		writebits(BCD,bits,wstart[i]+4,4);
	} /* for */ 

	/* Store objects: */ 
	/* Standard objects: */ 
	for(i=0;i<30;i++) {
		if (i==27) {
			/* escudo: */ 
			if (item[27] || item[30] || item[31]) bits[35*5+2+27]=1;
			if (item[31] && !item[27]) bits[35*5+2+30]=1;
			if (item[27]) bits[35*5+2+31]=1;
		} else {
			if (item[i]) bits[35*5+2+i]=1;
		} /* if */ 
	} /* for */ 
	/* Weapons: */ 
	for(i=0;i<4;i++) {
		if (item[32+i]) bits[33*5+i]=1;
	} /* for */ 
	if (item[32+4]) bits[33*5+5]=1;
	if (item[32+5]) bits[33*5+4]=1;
	/* Special items: */ 
	if (world_key) bits[35*5+1]=1;
	if (cross) bits[35*5]=1;
	/* Store characters: */ 
	if (character==0) {
		if (live_character[1]) val=17;
						  else val=16;
	} else {
		if (live_character[0]) val=2;
						  else val=0;
	} /* if */ 
	if (can_revive_character[0]) val+=8;
	if (can_revive_character[1]) val+=4;
	value2block(val,block);
	writebits(block,bits,42*5,5);

	bits2characters(bits,passwd);

	/* GENERAR CHECKSUM: */ 
	addchecksum(passwd);

} /* generatepassword */ 


bool readpassword(char *passwd)
{
	int i;
	char pt[45];
	int bits[45*5];
	int BCD[4];
	int block[5];
	int val;

	for(i=0;i<45;i++) pt[i]=passwd[i];
	addchecksum(pt);
	if (pt[43]!=passwd[43] || pt[44]!=passwd[44]) return false;

	characters2bits(bits,passwd);

	/* LEER PASSWORD: */ 
	/* Read arrows, keys and coins: */ 
	/* Arrows: */ 
	readbits(BCD,bits,12,4);
	val=BCD2digit(BCD);
	n_arrows+=val*100;
	readbits(BCD,bits,0,4);
	val=BCD2digit(BCD);
	n_arrows+=val*10;
	readbits(BCD,bits,4,4);
	val=BCD2digit(BCD);
	n_arrows+=val;
	/* Coins: */ 
	readbits(BCD,bits,28,4);
	val=BCD2digit(BCD);
	n_coins+=val*100;
	readbits(BCD,bits,16,4);
	val=BCD2digit(BCD);
	n_coins+=val*10;
	readbits(BCD,bits,20,4);
	val=BCD2digit(BCD);
	n_coins+=val;
	/* keys: */ 
	readbits(BCD,bits,44,4);
	val=BCD2digit(BCD);
	n_keys+=val*100;
	readbits(BCD,bits,32,4);
	val=BCD2digit(BCD);
	n_keys+=val*10;
	readbits(BCD,bits,36,4);
	val=BCD2digit(BCD);
	n_keys+=val;

	/* Read lifemeters: */ 
	/* Popolon: */ 
	readbits(block,bits,14*5+4,5);
	val=block2value(block);
	player_max[0]=player_energy[0]=player_denergy[0]=val*16;
	/* Aphrodite: */ 
	readbits(block,bits,11*5+3,5);
	val=block2value(block);
	player_max[1]=player_energy[1]=player_denergy[1]=val*16;

	/* Read objects: */ 
	/* Standard objects: */ 
	for(i=0;i<30;i++) {
		if (i==27) {
			/* escudo: */ 
			if (bits[35*5+2+31]==1) item[27]=true;
			if (bits[35*5+2+30]==1) item[31]=true;
			if (bits[35*5+2+31]==0 && bits[35*5+2+30]==0 &&
				bits[35*5+2+27]==1) item[30]=true;
		} else {
			if (bits[35*5+2+i]==1) item[i]=true;
		} /* if */ 
	} /* for */ 
	/* Weapons: */ 
	for(i=0;i<4;i++) {
		if (bits[33*5+i]==1) item[32+i]=true;
	} /* for */ 
	if (bits[33*5+5]==1) item[32+4]=true;
	if (bits[33*5+4]==1) item[32+5]=true;
	/* Special items: */ 
	if (bits[35*5+1]==1) world_key=true;
	if (bits[35*5]==1) cross=true;

	/* Read world information: */ 
	int wstart[10]={85,93,101,109,117,125,133,141,149,157};
	next_world=0;
	if (world_key) next_world=1;
	for(i=0;i<10;i++) {
		readbits(BCD,bits,wstart[i],4);
		world_item[i][2]=(BCD[0]==1);
		world_item[i][1]=(BCD[1]==1);
		world_item[i][0]=(BCD[2]==1);
		world_item[i][3]=(BCD[3]==1);

		readbits(BCD,bits,wstart[i]+4,4);
		if (BCD[3]==1) {
			world_doors_open[i]=true;
			if (world_key) next_world=i+2;
					  else next_world=i+1;
		} /* if */ 
	} /* for */ 

	/* Read characters: */ 
	readbits(block,bits,42*5,5);
	can_revive_character[0]=can_revive_character[1]=true;
	if (block[1]==1) can_revive_character[0]=true;
	if (block[2]==1) can_revive_character[1]=true;
	block[1]=0;
	block[2]=0;
	val=block2value(block);
	switch(val) {
	case 17:
		live_character[0]=live_character[1]=true;
		character=0;
		break;
	case 16:
		live_character[0]=true;
		live_character[1]=false;
		character=0;
		break;
	case 2:
		live_character[0]=live_character[1]=true;
		character=1;
		break;
	case 0:
		live_character[0]=false;
		live_character[1]=true;
		character=1;
		break;
	} /* switch */ 

	return true;
} /* readpassword */ 

