#include "miniaudio.h"
#include "stb.h"

#include <printf.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <threads.h>

typedef struct {
	int monkey;
} Foo;

typedef struct {
	stb_vorbis *vorbis_file;
	atomic_bool finished;
} UserData;

static int custom_specifier(FILE *stream, const struct printf_info *info, const void *const *args) {
	(void) info;
	Foo *arg = *(Foo **) args[0];
	return fprintf(stream, "Foo@%p { monkey = %d }", (void *) arg, arg->monkey);
}

static int custom_arginfo(const struct printf_info *info, size_t n, int *argtypes, int *size) {
	(void) info;
	if (n > 0) {
		argtypes[0] = PA_POINTER;
		*size = sizeof(void *);
	}
	return 1;
}

static void callback(ma_device *device, void *output, const void *input, ma_uint32 frame_count) {
	(void) input;
	UserData *user_data = device->pUserData;
	int samples_returned = stb_vorbis_get_samples_short_interleaved(user_data->vorbis_file,
	    (int) device->playback.channels, output,
	    (int) frame_count * (int) device->playback.channels);

	if (samples_returned == 0) {
		user_data->finished = true;
	}
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s <filename>.ogg\n", argv[0]);
		return 1;
	}

	register_printf_specifier('M', custom_specifier, custom_arginfo);
	Foo xyz = { .monkey = 5 };
	printf("%M\n", &xyz);

	int error;
	stb_vorbis *vorb = stb_vorbis_open_filename(argv[1], &error, nullptr);
	if (!vorb) {
		fprintf(stderr, "stb_vorbis error %d\n", error);
		return 1;
	}

	stb_vorbis_info info = stb_vorbis_get_info(vorb);
	UserData user_data = {
		.vorbis_file = vorb,
		.finished = false,
	};

	ma_device_config device_config = ma_device_config_init(ma_device_type_playback);
	device_config.playback.format = ma_format_s16;
	device_config.playback.channels = 2;
	device_config.sampleRate = info.sample_rate;
	device_config.dataCallback = callback;
	device_config.pUserData = &user_data;

	ma_device device;
	if (ma_device_init(nullptr, &device_config, &device) != MA_SUCCESS) {
		fprintf(stderr, "ma_device_init error\n");
		return 1;
	}
	if (ma_device_start(&device) != MA_SUCCESS) {
		fprintf(stderr, "ma_device_start error\n");
		return 1;
	}

	while (!user_data.finished) {
		thrd_sleep(&(struct timespec) { .tv_sec = 0, .tv_nsec = 10 * 1000 * 1000 }, nullptr);
	}
	ma_device_uninit(&device);

	stb_vorbis_close(vorb);
	return 0;
}
