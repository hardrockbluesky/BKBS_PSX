// Just a list of cinePos, cineRot, and speed of transition for the camera movement
// speed 1-4096, 4096 is instant transition. 0 = hold wiggle cam for ~13.5 seconds

#define NUM_CINE 35

VECTOR cinePos[NUM_CINE];
SVECTOR cineRot[NUM_CINE];
int cineSpeed[NUM_CINE];
int cineCount;

void initCine(){

	cineCount = 0;

	// sky to ground
	cineSpeed[cineCount] = 6;
	cinePos[cineCount].vx = -616;
	cinePos[cineCount].vy = 4840;
	cinePos[cineCount].vz = 3551;
	cineRot[cineCount].vx = -100;
	cineRot[cineCount].vy = 470;
	cineRot[cineCount].vz = 0;
	cineCount++;
	cinePos[cineCount].vx = -616;
	cinePos[cineCount].vy = 770;
	cinePos[cineCount].vz = 3551;
	cineRot[cineCount].vx = -100;
	cineRot[cineCount].vy = 470;
	cineRot[cineCount].vz = 0;
	cineCount++;

	// sky to ground 2
	cineSpeed[cineCount] = 25;
	cinePos[cineCount].vx = 3066;
	cinePos[cineCount].vy = 3400;
	cinePos[cineCount].vz = -2154;
	cineRot[cineCount].vx = -160;
	cineRot[cineCount].vy = 490;
	cineRot[cineCount].vz = 0;
	cineCount++;
	cinePos[cineCount].vx = -687;
	cinePos[cineCount].vy = 620;
	cinePos[cineCount].vz = 194;
	cineRot[cineCount].vx = -160;
	cineRot[cineCount].vy = 490;
	cineRot[cineCount].vz = 0;
	cineCount++;

	// side porch sky to bookshelves
	cineSpeed[cineCount] = 25;
	cinePos[cineCount].vx = 2289;
	cinePos[cineCount].vy = 410;
	cinePos[cineCount].vz = 1364;
	cineRot[cineCount].vx = -190;
	cineRot[cineCount].vy = 250;
	cineRot[cineCount].vz = 0;
	cineCount++;
	cinePos[cineCount].vx = 1606;
	cinePos[cineCount].vy = 410;
	cinePos[cineCount].vz = 3556;
	cineRot[cineCount].vx = -80;
	cineRot[cineCount].vy = 350;
	cineRot[cineCount].vz = 0;
	cineCount++;

	// bushes to lattic
	cineSpeed[cineCount] = 25;
	cinePos[cineCount].vx = 149;
	cinePos[cineCount].vy = 280;
	cinePos[cineCount].vz = 579;
	cineRot[cineCount].vx = 40;
	cineRot[cineCount].vy = 690;
	cineRot[cineCount].vz = 0;
	cineCount++;
	cinePos[cineCount].vx = 347;
	cinePos[cineCount].vy = 280;
	cinePos[cineCount].vz = 1581;
	cineRot[cineCount].vx = 40;
	cineRot[cineCount].vy = 540;
	cineRot[cineCount].vz = 0;
	cineCount++;

/*
	// behind bush facing girl //Lags on real hardware
	cineSpeed[cineCount] = 0;
	cinePos[cineCount].vx = 251;
	cinePos[cineCount].vy = 370;
	cinePos[cineCount].vz = 3312;
	cineRot[cineCount].vx = -10;
	cineRot[cineCount].vy = 420;
	cineRot[cineCount].vz = 0;
	cineCount++;
*/

	// lattice to left bush
	cineSpeed[cineCount] = 25;
	cinePos[cineCount].vx = -485;
	cinePos[cineCount].vy = 210;
	cinePos[cineCount].vz = 1360;
	cineRot[cineCount].vx = -10;
	cineRot[cineCount].vy = 820;
	cineRot[cineCount].vz = 0;
	cineCount++;
	cinePos[cineCount].vx = -149;
	cinePos[cineCount].vy = 210;
	cinePos[cineCount].vz = 2537;
	cineRot[cineCount].vx = -10;
	cineRot[cineCount].vy = 820;
	cineRot[cineCount].vz = 0;
	cineCount++;
	
	// left pass far
	cineSpeed[cineCount] = 25;
	cinePos[cineCount].vx = -1726;
	cinePos[cineCount].vy = 1120;
	cinePos[cineCount].vz = 1565;
	cineRot[cineCount].vx = 140;
	cineRot[cineCount].vy = 890;
	cineRot[cineCount].vz = 0;
	cineCount++;
	cinePos[cineCount].vx = -166;
	cinePos[cineCount].vy = 1120;
	cinePos[cineCount].vz = 2100;
	cineRot[cineCount].vx = 140;
	cineRot[cineCount].vy = 890;
	cineRot[cineCount].vz = 0;
	cineCount++;
	
	

	// behind bush looking at rock
	cineSpeed[cineCount] = 0;
	cinePos[cineCount].vx = 238;
	cinePos[cineCount].vy = 380;
	cinePos[cineCount].vz = 3484;
	cineRot[cineCount].vx = -20;
	cineRot[cineCount].vy = 320;
	cineRot[cineCount].vz = 0;
	cineCount++;

	// behind bush facing girl 2
	cineSpeed[cineCount] = 0;
	cinePos[cineCount].vx = 523;
	cinePos[cineCount].vy = 280;
	cinePos[cineCount].vz = 2705;
	cineRot[cineCount].vx = -80;
	cineRot[cineCount].vy = 620;
	cineRot[cineCount].vz = 0;
	cineCount++;

	// looking down at girl
	cineSpeed[cineCount] = 0;
	cinePos[cineCount].vx = -833;
	cinePos[cineCount].vy = 1500;
	cinePos[cineCount].vz = 2634;
	cineRot[cineCount].vx = 270;
	cineRot[cineCount].vy = 740;
	cineRot[cineCount].vz = 0;
	cineCount++;


	// zoom past girl from right
	cineSpeed[cineCount] = 25;
	cinePos[cineCount].vx = 86;
	cinePos[cineCount].vy = 280;
	cinePos[cineCount].vz = 299;
	cineRot[cineCount].vx = -20;
	cineRot[cineCount].vy = 1440;
	cineRot[cineCount].vz = 0;
	cineCount++;
	cinePos[cineCount].vx = 1400;//1450;//1554;
	cinePos[cineCount].vy = 380;
	cinePos[cineCount].vz = 2000;//2041;
	cineRot[cineCount].vx = -20;
	cineRot[cineCount].vy = 730;
	cineRot[cineCount].vz = 0;
	cineCount++;

	// big shot
	cineSpeed[cineCount] = 0;
	cinePos[cineCount].vx = -1482;
	cinePos[cineCount].vy = 190;
	cinePos[cineCount].vz = 3867;
	cineRot[cineCount].vx = -110;
	cineRot[cineCount].vy = 540;
	cineRot[cineCount].vz = 0;
	cineCount++;

	// pan inside
	cineSpeed[cineCount] = 25;
	cinePos[cineCount].vx = 2006;
	cinePos[cineCount].vy = 400;
	cinePos[cineCount].vz = 2880;
	cineRot[cineCount].vx = -30;
	cineRot[cineCount].vy = 400;
	cineRot[cineCount].vz = 0;
	cineCount++;
	cinePos[cineCount].vx = 2017;
	cinePos[cineCount].vy = 400;
	cinePos[cineCount].vz = 1562;
	cineRot[cineCount].vx = -30;
	cineRot[cineCount].vy = 400;
	cineRot[cineCount].vz = 0;
	cineCount++;
	
	// tight house shot
	cineSpeed[cineCount] = 0;
	cinePos[cineCount].vx = -911;
	cinePos[cineCount].vy = 550;
	cinePos[cineCount].vz = 3281;
	cineRot[cineCount].vx = -160;
	cineRot[cineCount].vy = 510;
	cineRot[cineCount].vz = 0;
	cineCount++;


	// zoomed girl left
	cineSpeed[cineCount] = 0;
	cinePos[cineCount].vx = 702;
	cinePos[cineCount].vy = 350;
	cinePos[cineCount].vz = 2220;
	cineRot[cineCount].vx = 20;
	cineRot[cineCount].vy = 590;
	cineRot[cineCount].vz = 0;
	cineCount++;


	// left pass far
	cineSpeed[cineCount] = 25;
	cinePos[cineCount].vx = -1726;
	cinePos[cineCount].vy = 1120;
	cinePos[cineCount].vz = 1565;
	cineRot[cineCount].vx = 140;
	cineRot[cineCount].vy = 890;
	cineRot[cineCount].vz = 0;
	cineCount++;
	cinePos[cineCount].vx = -166;
	cinePos[cineCount].vy = 1120;
	cinePos[cineCount].vz = 2100;
	cineRot[cineCount].vx = 140;
	cineRot[cineCount].vy = 890;
	cineRot[cineCount].vz = 0;
	cineCount++;

	// group right hover
	cineSpeed[cineCount] = 0;
	cinePos[cineCount].vx = 295;
	cinePos[cineCount].vy = 450;
	cinePos[cineCount].vz = 1106;
	cineRot[cineCount].vx = 30;
	cineRot[cineCount].vy = 1250;
	cineRot[cineCount].vz = 0;
	cineCount++;

	// tight right pass
	cineSpeed[cineCount] = 22;
	cinePos[cineCount].vx = 599;
	cinePos[cineCount].vy = 380;
	cinePos[cineCount].vz = 2377;
	cineRot[cineCount].vx = 30;
	cineRot[cineCount].vy = 620;
	cineRot[cineCount].vz = 0;
	cineCount++;
	cinePos[cineCount].vx = 316;
	cinePos[cineCount].vy = 380;
	cinePos[cineCount].vz = 1500;
	cineRot[cineCount].vx = 30;
	cineRot[cineCount].vy = 620;
	cineRot[cineCount].vz = 0;
	cineCount++;

	// pulled out normal hover
	cineSpeed[cineCount] = 0;
	cinePos[cineCount].vx = -1039;
	cinePos[cineCount].vy = 830;
	cinePos[cineCount].vz = 3369;
	cineRot[cineCount].vx = 40;
	cineRot[cineCount].vy = 570;
	cineRot[cineCount].vz = 0;
	cineCount++;

	// pulled out ground hover
	cineSpeed[cineCount] = 0;
	cinePos[cineCount].vx = -1185;
	cinePos[cineCount].vy = 200;
	cinePos[cineCount].vz = 340;
	cineRot[cineCount].vx = -80;
	cineRot[cineCount].vy = 600;
	cineRot[cineCount].vz = 0;
	cineCount++;

	// end pull
	cineSpeed[cineCount] = 25;
	cinePos[cineCount].vx = 362;
	cinePos[cineCount].vy = 500;
	cinePos[cineCount].vz = 2612;
	cineRot[cineCount].vx = 110;
	cineRot[cineCount].vy = 520;
	cineRot[cineCount].vz = 0;
	cineCount++;
	cinePos[cineCount].vx = 362;
	cinePos[cineCount].vy = 3160;
	cinePos[cineCount].vz = 2612;
	cineRot[cineCount].vx = -230;
	cineRot[cineCount].vy = 520;
	cineRot[cineCount].vz = 0;
	cineCount++;
	
	// hold in the sky for credits
	cineSpeed[cineCount] = 0;
	cinePos[cineCount].vx = 362;
	cinePos[cineCount].vy = 3160;
	cinePos[cineCount].vz = 2612;
	cineRot[cineCount].vx = -230;
	cineRot[cineCount].vy = 520;
	cineRot[cineCount].vz = 0;
	cineCount++;
	
	// hold in the sky for credits x2
	cineSpeed[cineCount] = 0;
	cinePos[cineCount].vx = 362;
	cinePos[cineCount].vy = 3160;
	cinePos[cineCount].vz = 2612;
	cineRot[cineCount].vx = -230;
	cineRot[cineCount].vy = 520;
	cineRot[cineCount].vz = 0;
	cineCount++;
	
	// hold in the sky for credits x3
	cineSpeed[cineCount] = 0;
	cinePos[cineCount].vx = 362;
	cinePos[cineCount].vy = 3160;
	cinePos[cineCount].vz = 2612;
	cineRot[cineCount].vx = -230;
	cineRot[cineCount].vy = 520;
	cineRot[cineCount].vz = 0;
	cineCount++;
	
	// hold in the sky for credits x4
	cineSpeed[cineCount] = 0;
	cinePos[cineCount].vx = 362;
	cinePos[cineCount].vy = 3160;
	cinePos[cineCount].vz = 2612;
	cineRot[cineCount].vx = -230;
	cineRot[cineCount].vy = 520;
	cineRot[cineCount].vz = 0;
	cineCount++;
	
}