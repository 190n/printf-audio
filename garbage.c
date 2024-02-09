#include "bruh.h"
#include "miniaudio.h"
#include "stb.h"

#include <printf.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <threads.h>

typedef struct {
	stb_vorbis *vorbis_file;
	atomic_bool finished;
} UserData;

static void miniaudio_callback(
    ma_device *device, void *output, const void *input, ma_uint32 frame_count) {
	(void) input;
	UserData *user_data = device->pUserData;
	int samples_returned = stb_vorbis_get_samples_short_interleaved(user_data->vorbis_file,
	    (int) device->playback.channels, output,
	    (int) frame_count * (int) device->playback.channels);

	if (samples_returned == 0) {
		user_data->finished = true;
	}
}

static int custom_specifier(FILE *stream, const struct printf_info *info, const void *const *args) {
	(void) info;

	const unsigned char *data = *(const unsigned char **) args[0];
	int len = *(int *) args[1];
	int bytes_written = 0;

	int vorb_error;
	stb_vorbis *vorb = stb_vorbis_open_memory(data, len, &vorb_error, nullptr);
	if (!vorb) {
		bytes_written += fprintf(stream, "stb_vorbis error %d\n", vorb_error);
		return bytes_written;
	}

	stb_vorbis_info vorb_info = stb_vorbis_get_info(vorb);
	UserData user_data = {
		.vorbis_file = vorb,
		.finished = false,
	};

	ma_device_config device_config = ma_device_config_init(ma_device_type_playback);
	device_config.playback.format = ma_format_s16;
	device_config.playback.channels = 2;
	device_config.sampleRate = vorb_info.sample_rate;
	device_config.dataCallback = miniaudio_callback;
	device_config.pUserData = &user_data;

	ma_device device;
	if (ma_device_init(nullptr, &device_config, &device) != MA_SUCCESS) {
		bytes_written += fprintf(stream, "ma_device_init error\n");
		return bytes_written;
	}
	if (ma_device_start(&device) != MA_SUCCESS) {
		bytes_written += fprintf(stream, "ma_device_start error\n");
		return bytes_written;
	}

	bytes_written += fprintf(stream, "played 2ch s16 %uHz\n", vorb_info.sample_rate);

	while (!user_data.finished) {
		thrd_sleep(&(struct timespec) { .tv_sec = 0, .tv_nsec = 10 * 1000 * 1000 }, nullptr);
	}

	ma_device_uninit(&device);
	stb_vorbis_close(vorb);

	return bytes_written;
}

static int custom_arginfo(const struct printf_info *info, size_t n, int *argtypes, int *size) {
	(void) info;
	if (n > 1) {
		argtypes[0] = PA_POINTER;
		argtypes[1] = PA_INT;
		size[0] = sizeof(const unsigned char *);
		size[1] = sizeof(int);
	}
	return 2;
}

int main(void) {
	register_printf_specifier('G', custom_specifier, custom_arginfo);
	printf("%G", bruh_ogg, bruh_ogg_len);

	return 0;
}
