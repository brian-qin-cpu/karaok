#ifndef _FIR_REVERB_H__
#define _FIR_REVERB_H__

typedef struct {
	float room_size;
	float rt60;
	float wet_gain;
	float dry_gain;
} reverb_params_t;

typedef struct {
	short *ir_buffer;
	float *delay_line;
	size_t ir_length;
	size_t write_pos;
	int sample_rate;
	reverb_params_t params;
} reverb_t;

reverb_t *reverb_create(int samplerate, size_t ir_ms);
void reverb_destroy(reverb_t *rv);
void reverb_set_param(reverb_t *rv, const char *name, float value);
short reverb_process(reverb_t *rv, short input);

void reverb_process_block(reverb_t *rv, short *in_out, size_t num_samples);

#endif /* _FIR_REVERB_H__ */
