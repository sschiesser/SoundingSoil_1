/*
 * recorder.h
 *
 * Created: 22.01.2018 14:46:57
 *  Author: schiesser
 */ 


#ifndef RECORDER_H_
#define RECORDER_H_

#define PCM_BUF_SIZE				16
#define NB_BUFFERS					4
#define AUDIO_NUM_CHANNELS			2
#define NUMBER_OF_CHANNELS			0x02
/* Byte rate = SampleRate * NumChannels * BitPerChannel/8
 *    br     =   44100    *      2      *       16/8     = 176'400 (0x2B110) */
#define BYTE_RATE_LL				0x10
#define BYTE_RATE_LH				0xB1
#define BYTE_RATE_HL				0x02
#define BYTE_RATE_HH				0x00
//int16_t audio_ch0_buffer[NB_BUFFERS][PCM_BUF_SIZE] = {{0}};
//int16_t audio_ch1_buffer[NB_BUFFERS][PCB_BUF_SIZE] = {{0}};

/*Wave header for PCM sound file */
COMPILER_WORD_ALIGNED int8_t wave_header[44]= {
	0x52, 0x49, 0x46, 0x46,  /*'R', 'I, 'F', 'F' */
	0x00, 0x00, 0x00, 0x00,  /* Chunk Size = subchunk1+subchunk2 size */
	0x57, 0x41, 0x56, 0x45,  /*'W', 'A, 'V', 'E' */
	0x66, 0x6d, 0x74, 0x20,  /* 'f', 'm', 't', 0x20 */
	0x10, 0x00, 0x00, 0x00,  /* Subchunk1 size - 16 for PCM */
	0x01, 0x00,              /* Audio format - PCM */
	NUMBER_OF_CHANNELS, 0x00,/* Number of channels - 1 for mono 2 for stereo*/
	0x44, 0xAC, 0x00, 0x00,  /* Sampling rate -  44100 (0xAC44)*/
	BYTE_RATE_LL, BYTE_RATE_LH, BYTE_RATE_HL, BYTE_RATE_HH,  /* Byte rate = SampleRate * NumChannels 
								* BitsPerSample/8 */
	NUMBER_OF_CHANNELS*2, 0x00, /* Block Alignment 
							  * = NumChannels * BitsPerSample/8 = 4*/
	0x10, 0x00,              /* Bits per samples = 16*/
	0x64, 0x61, 0x74, 0x61,  /*'d', 'a', 't', 'a' */
	0x00, 0x00, 0x00, 0x00,  /* subchunk2 size - total size of samples */
};




#endif /* RECORDER_H_ */