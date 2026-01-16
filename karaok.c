#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <alsa/asoundlib.h>
#include <signal.h>

#include "ringbuf.h"
#include "reverb_engine.h"


#define SAMPLE_RATE	44100
#define CHANNELS	2
#define FORMAT		SND_PCM_FORMAT_S16_LE

#define PERIOD_SIZE	1024
#define BUF_PERIODS	4

#define BUFFER_TIME   200000  // 200 ms
#define CAPTURE_PERIOD_TIME   10000  // 10ms
#define PLAY_PERIOD_TIME	  20000  // 20ms


static ring_buffer_t g_ring;
static volatile int g_running = 1;
static pthread_t alsa_capture;
static pthread_t alsa_playback;

static inline size_t frame_to_bytes(snd_pcm_uframes_t frames)
{
	return frames * CHANNELS * snd_pcm_format_size(FORMAT, 1);
}

static void *capture_thread(void *arg)
{
	(void)arg;
	snd_pcm_t *capture_handle;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_sw_params_t *sw_params;
	char *buffer;
	unsigned int buffer_time = BUFFER_TIME;
	unsigned int period_time = CAPTURE_PERIOD_TIME;

	if (snd_pcm_open(&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0) < 0) {
		printf("Cannot open capture device\n");
		g_running = 0;
		return NULL;
	}

	snd_pcm_hw_params_alloca(&hw_params);
	snd_pcm_hw_params_any(capture_handle, hw_params);
	snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(capture_handle, hw_params, FORMAT);
	snd_pcm_hw_params_set_channels(capture_handle, hw_params, CHANNELS);
	unsigned int rate = SAMPLE_RATE;
	snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &rate, 0);
	snd_pcm_hw_params_set_buffer_time_near(capture_handle, hw_params, &buffer_time, NULL);
	snd_pcm_hw_params_set_period_time_near(capture_handle, hw_params, &period_time, NULL);
	snd_pcm_hw_params(capture_handle, hw_params);

	snd_pcm_uframes_t period_size;
	snd_pcm_hw_params_get_period_size(hw_params, &period_size, NULL);

	snd_pcm_sw_params_alloca(&sw_params);
	snd_pcm_sw_params_current(capture_handle, sw_params);
	snd_pcm_sw_params_set_start_threshold(capture_handle, sw_params, 3 * period_size);
	snd_pcm_sw_params(capture_handle, sw_params);
	
	snd_pcm_prepare(capture_handle);
	snd_pcm_uframes_t frames_to_read = period_size * 3;
	buffer = (char *)malloc(frame_to_bytes(frames_to_read));
	printf("Capture thread started, period_size = %ld \n", period_size);

	struct timespec start, end;
	while(g_running) {
		clock_gettime(CLOCK_MONOTONIC, &start);
		int err = snd_pcm_readi(capture_handle, buffer, frames_to_read);
		if (err == -EPIPE) {
			//printf("Capture overrun!\n");
			snd_pcm_recover(capture_handle, err, 0);
			continue;
		} else if (err < 0) {
			printf("Capture error: %s\n", snd_strerror(err));
			break;
		}
	
		size_t to_write = frame_to_bytes(err);
		while (to_write > 0 && g_running) {
			size_t space = ring_avail_write(&g_ring);
			if (space == 0) {
				usleep(1000);
				//printf("no space to write ring buf.\n");
				continue;
			}
			size_t write_now = (to_write < space) ? to_write : space;
			ring_write(&g_ring, buffer + (frame_to_bytes(err) - to_write), write_now);
			to_write -= write_now;
		}
		clock_gettime(CLOCK_MONOTONIC, &end);
		double elapsed = end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9;
		//printf("%s: to_write = %d, elapsed = %f \n", __func__, err, elapsed);
	}
	free(buffer);
	snd_pcm_close(capture_handle);
	printf("Capture thread exit\n");

	return NULL;
}

static void *playback_thread(void *arg)
{
	(void)arg;
	snd_pcm_t *playback_handle;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_sw_params_t *sw_params;
	char *buffer;
	unsigned int rate = SAMPLE_RATE;
	//snd_pcm_uframes_t frames = PERIOD_SIZE;
	//snd_pcm_uframes_t buffer_size = frames * BUF_PERIODS;
	unsigned int buffer_time = BUFFER_TIME;
	unsigned int period_time = PLAY_PERIOD_TIME;

	if (snd_pcm_open(&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
		printf("Cannot open playback device\n");
		g_running = 0;
		return NULL;
	}

	snd_pcm_hw_params_alloca(&hw_params);
	snd_pcm_hw_params_any(playback_handle, hw_params);
	snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(playback_handle, hw_params, FORMAT);
	snd_pcm_hw_params_set_channels(playback_handle, hw_params, CHANNELS);

	snd_pcm_hw_params_set_rate_near(playback_handle, hw_params, &rate, 0);
	snd_pcm_hw_params_set_buffer_time_near(playback_handle, hw_params, &buffer_time, NULL);
	snd_pcm_hw_params_set_period_time_near(playback_handle, hw_params, &period_time, NULL);
	snd_pcm_hw_params(playback_handle, hw_params);

	snd_pcm_uframes_t play_start_buffer_size = 3 * rate * PLAY_PERIOD_TIME / (1000 * 1000);
	snd_pcm_sw_params_alloca(&sw_params);
	snd_pcm_sw_params_current(playback_handle, sw_params);
	snd_pcm_sw_params_set_start_threshold(playback_handle, sw_params, play_start_buffer_size);
	snd_pcm_sw_params(playback_handle, sw_params);

	snd_pcm_prepare(playback_handle);
	snd_pcm_uframes_t period_size;
	snd_pcm_hw_params_get_period_size(hw_params, &period_size, NULL);
	buffer = (char *)malloc(frame_to_bytes(period_size));

	printf("Playback thread started, period_size=%ld \n", period_size);

	struct timespec start, end;
	while(g_running) {
		clock_gettime(CLOCK_MONOTONIC, &start);
		size_t needed = frame_to_bytes(period_size);
		size_t total_read = 0;
		while (total_read < needed && g_running) {
			size_t avail = ring_avail_read(&g_ring);
			if (avail == 0) {
				usleep(5000);
				//printf("no data to read in ringbuf\n");
				continue;
			}
			size_t read_now = (needed - total_read < avail) ? needed - total_read : avail;
			ring_read(&g_ring, buffer + total_read, read_now);
			total_read += read_now;
		}
		if (!g_running)
			break;
		struct timespec prbegin, prend;
		clock_gettime(CLOCK_MONOTONIC, &prbegin);

		reverb_engine_process(buffer, period_size * CHANNELS);

		clock_gettime(CLOCK_MONOTONIC, &prend);
		int err = snd_pcm_writei(playback_handle, buffer, period_size);
		if (err == -EPIPE) {
			//printf("Playback underrun!\n");
			snd_pcm_recover(playback_handle, err, 0);
			continue;
		} else if (err < 0) {
			printf("Playback error: %s\n", snd_strerror(err));
			break;
		}
		clock_gettime(CLOCK_MONOTONIC, &end);
		double elapsed = end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9;
		double prelapsed = prend.tv_sec - prbegin.tv_sec + (prend.tv_nsec - prbegin.tv_nsec) / 1e9;
		//printf("%s: elapsed = %f, process cost: %f\n", __func__, elapsed, prelapsed);
	}
	free(buffer);
	snd_pcm_close(playback_handle);
	printf("playback thread exit.\n");

	return NULL;
}

static void signal_handler(int sig)
{
	if (SIGINT == sig) {
		g_running = 0;
		reverb_engine_destroy();
		pthread_join(alsa_capture, NULL);
		pthread_join(alsa_playback, NULL);
		if (g_ring.buffer) {
			free(g_ring.buffer);
			g_ring.buffer = NULL;
		}
		printf("karaok stopped!\n");
	}
}

int main(int argc, char **argv)
{
	(void)argv;
	(void)argc;
	size_t ring_bytes = frame_to_bytes(PERIOD_SIZE * BUF_PERIODS * 2);


	if (ring_init(&g_ring, ring_bytes) != 0) {
		printf("fail to init ring buffer\n");
		return -1;
	}

	signal(SIGINT, signal_handler);

	reverb_engine_create(FREE_REVERB, SAMPLE_RATE);
	//reverb_engine_create(FIR_REVERB, SAMPLE_RATE);
	reverb_engine_set_params(0.8f, 3.0f, 0.6f, 0.6f);


	if (pthread_create(&alsa_capture, NULL, capture_thread, NULL) != 0 ||
		pthread_create(&alsa_playback, NULL, playback_thread, NULL) != 0) {
		printf("Fail to create threads\n");
		g_running = 0;
		return -1;
	}

	printf("Loopback running... Press Ctrl+C to stop.\n");
	pause();

	printf("karaok exit!\n");

	return 0;
}
