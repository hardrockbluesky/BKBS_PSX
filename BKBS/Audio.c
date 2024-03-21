
#define SOUND_MALLOC_MAX 10

SpuCommonAttr l_c_attr;
SpuVoiceAttr  g_s_attr;
unsigned long l_vag1_spu_addr;
CdlLOC songLoc;

// CDAudio Track Array, update this to include the tracks from your makefile
// the DS library docs say this must end in 0, I guess that means you have to have at least two audio tracks!
int audioTracks[] = {2,3,0};

u_char loaded = 0;
u_char CDCode[8];

void loadCallback(){
	loaded = 1;
}

void audio_init() {
	SpuInit();
	SpuInitMalloc (SOUND_MALLOC_MAX, (char*)(SPU_MALLOC_RECSIZ * (SOUND_MALLOC_MAX + 1)));
	l_c_attr.mask = (SPU_COMMON_MVOLL | SPU_COMMON_MVOLR | SPU_COMMON_CDVOLL | SPU_COMMON_CDVOLR | SPU_COMMON_CDMIX);
	l_c_attr.mvol.left  = 0x3fff; // set master left volume
	l_c_attr.mvol.right = 0x3fff; // set master right volume
	l_c_attr.cd.volume.left  = 0x3fff; // set CD left volume
	l_c_attr.cd.volume.right = 0x3fff; // set CD right volume
	l_c_attr.cd.mix = SPU_ON; // I honestly don't know
	SpuSetCommonAttr (&l_c_attr);
	SpuSetTransferMode (SPU_TRANSFER_BY_DMA);
}

void audio_transfer_vag_to_spu(char* sound, int sound_size, int voice_channel) {
	SpuSetTransferMode (SpuTransByDMA); // set transfer mode to DMA
	l_vag1_spu_addr = SpuMalloc(sound_size); // allocate SPU memory for sound 1
	SpuSetTransferStartAddr(l_vag1_spu_addr); // set transfer starting address to malloced area
	SpuWrite (sound + 0x30, sound_size); // perform actual transfer
	SpuIsTransferCompleted (SPU_TRANSFER_WAIT); // wait for DMA to complete
	g_s_attr.mask =
			(
					SPU_VOICE_VOLL |
					SPU_VOICE_VOLR |
					SPU_VOICE_PITCH |
					SPU_VOICE_WDSA |
					SPU_VOICE_ADSR_AMODE |
					SPU_VOICE_ADSR_SMODE |
					SPU_VOICE_ADSR_RMODE |
					SPU_VOICE_ADSR_AR |
					SPU_VOICE_ADSR_DR |
					SPU_VOICE_ADSR_SR |
					SPU_VOICE_ADSR_RR |
					SPU_VOICE_ADSR_SL
			);

	g_s_attr.voice = (voice_channel);

	g_s_attr.volume.left  = 0x1fff;
	g_s_attr.volume.right = 0x1fff;

	g_s_attr.pitch        = 0x1000;
	g_s_attr.addr         = l_vag1_spu_addr;
	g_s_attr.a_mode       = SPU_VOICE_LINEARIncN;
	g_s_attr.s_mode       = SPU_VOICE_LINEARIncN;
	g_s_attr.r_mode       = SPU_VOICE_LINEARDecN;
	g_s_attr.ar           = 0x0;
	g_s_attr.dr           = 0x0;
	g_s_attr.sr           = 0x0;
	g_s_attr.rr           = 0x0;
	g_s_attr.sl           = 0xf;

	SpuSetVoiceAttr (&g_s_attr);
}

void audio_play(int voice_channel) {
	SpuSetKey(SpuOn, voice_channel);
}

void audio_free(unsigned long spu_address) {
	SpuFree(spu_address);
}

// Adapted from Sparkletron's PSYQ_Examples on github
// Not accurate enough for a rhythm game unfortunately
int playCDtracks(int *p_tracks, int tracknum) {	
	cd_open();
	if(DsPlay(1, p_tracks, tracknum) < 0)
	{
	return 0;
	}
	return 1;
}

int PlaySyncedTrack() {	
	cd_open();
	
	// The seek for CdlPlay is blocking - the play command callback does return when the seek is completed but play is then interrupted by cached GetStats from DSlib.
	// This gets us out of sync, so instead we call play and pause back to back, allow both of those commands to process, then call play again for a much shorter seek time.
	// When that final play is confirmed from our own GetStat, we can start the game. This method of waiting to get the correct play code results in an extremely small (one sector, I think)
	// sync issue between notes and audio playing, but this game is not very picky about timing so I'm not too concerned about finding a better way.
	// Could use 0x02 [set seek location] + 0x16 [seek to audio sector] to clean this up but it works and I gotta move on for now!
	
	// Use info from .cue file, track 02 audio: index 01, add one second to defend against crappy CD burners [like mine]
	//songLoc.minute = 0x00;
	//songLoc.second = 0x20;
	//songLoc.sector = 0x38;
	songLoc.minute = 0x00;
	songLoc.second = 0x22;
	songLoc.sector = 0x55;
	
	// CdlPlay code = 0x03
	DsCommand(0x03, (u_char*)&songLoc, NULL, 500);
	// CdlPause code = 0x09
	DsCommand(0x09, NULL, NULL, 500);
	
	while ( CDCode[0] != 0x82 ) {
		// Get status of CD drive, 0x82 = playing CDDA, 0x02 = spinning/paused, use regular CdControl instead of DsCommand because we don't need the queue system of DSLib for these commands.
		CdControl(0x01, NULL, &CDCode[0]);
		printf("waiting for CDDA play to start...\n");
	}
	
	DsCommand(0x09, NULL, NULL, 500);
	
	while ( CDCode[0] != 0x02 ) {
		CdControl(0x01, NULL, &CDCode[0]);
		printf("CDDA started, waiting for pause...\n");
		//FntPrint("CDDA started, waiting for pause...\n");
	}
	
	printf("CDDA paused!\n");
	
	DsCommand(0x03, (u_char*)&songLoc, NULL, 500);
	
	while ( CDCode[0] != 0x82 ) {
		CdControl(0x01, NULL, &CDCode[0]);
		printf("waiting for CDDA play to restart...\n");
		//FntPrint("waiting for CDDA play to restart...\n");
	}
	
	printf("CDDA play restarted!\n");
	
	CDCode[0] = 0;
	
	return 1;
	
}