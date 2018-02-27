/*
 * audio_in.h
 *
 * Created: 23.01.2018 14:10:54
 *  Author: schiesser
 */ 


#ifndef AUDIO_IN_H_
#define AUDIO_IN_H_

void audio_in_init(void);
bool audio_record_init(void);
bool audio_record_close(void);
void audio_record_1samp(uint8_t buf);
//bool audio_write_1samp(bool ub);
bool audio_write_chunk(uint8_t buf);
//void audio_sync_reached_callback(void);

void audio_sync_init(void);


#endif /* ADC_H_ */