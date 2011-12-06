#include "gme_player.h"
#include "gme_parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define self ((struct GMEPlayerContext*)ctx)
static unsigned samplerate = 44100;

static unsigned samples_to_ms(unsigned sample_count) {
	return sample_count/(samplerate/1000);
}

void *GMEPlayerOpen(struct sppb_plugin_description *plugin, struct sppb_byte_input *input, int song_index)
{
	if (!input->get_length)
		return spfalse;

	ssize_t len = input->get_length(input);

	if (len < 0)
		return spfalse;

	void *data = malloc(len);

	if (!data)
		return spfalse;

	if (input->read(input, data, len) != len) {
		fprintf(stderr, "GMEPlayerOpen(%s): failed to read input\n", input->uri);
		free(data);
		return spfalse;
	}

	struct GMEPlayerContext *ctx = calloc(sizeof(struct GMEPlayerContext), 1);

	self->trackno = song_index;

	gme_err_t err;
	err = gme_open_data(data, len, &self->emu, samplerate);
	if (err) {
		fprintf(stderr, "GME couldn't open file %s: %s\n", input->uri, err);
		free(ctx);
		free(data);
		return spfalse;
	}

	free(data);

	err = gme_track_info(self->emu, &self->track, self->trackno);
	if (err) {
		fprintf(stderr, "GME couldn't get track info of track %d in %s: %s\n", song_index, input->uri, err);
		free(ctx);
		return spfalse;
	}

	self->length_in_samples = lengthInSamplesOfTrack(&self->track);

	err = gme_start_track(self->emu, self->trackno);
	if (err) {
		fprintf(stderr, "GME couldn't play track %d in %s: %s\n", song_index, input->uri, err);
		free(ctx);
		return spfalse;
	}

	// Fade out during the last two seconds of playback
	gme_set_fade(self->emu, samples_to_ms(self->length_in_samples)-2000);

	return ctx;
}
void GMEPlayerClose(struct sppb_plugin_description *plugin, void *ctx)
{
	gme_delete(self->emu);
	free(ctx);
}

spbool GMEPlayerDecode(struct sppb_plugin_description *plugin, void *ctx, spbyte *dest, size_t *destlen, spbool *final)
{
	gme_err_t err;
	err = gme_play(self->emu, *destlen/2, (short*)dest);
	if (err) {
		fprintf(stderr, "GME error while decoding: %s\n", err);
		*destlen = 0;
		return spfalse;
	}
	*final = gme_track_ended(self->emu);

	return sptrue;
}
spbool GMEPlayerSeek(struct sppb_plugin_description *plugin, void *ctx, unsigned sample)
{
	gme_err_t err;
	err = gme_seek(self->emu, samples_to_ms(sample));
	if (err) {
		fprintf(stderr, "GME couldn't seek %s\n", err);
		return spfalse;
	}
	return sptrue;
}
size_t GMEPlayerGetMinimumOutputBufferSize(struct sppb_plugin_description *plugin, void *ctx)
{
	return 1024*16;
}
unsigned int GMEPlayerGetLengthInSamples(struct sppb_plugin_description *plugin, void *ctx)
{
	return self->length_in_samples;
}

void GMEPlayerGetAudioFormat(struct sppb_plugin_description *plugin, void *ctx, unsigned int *samplerate_, enum sppb_sound_format *format, enum sppb_channel_format *channelFormat)
{
	*samplerate_ = samplerate;
	*format = SPPB_SOUND_FORMAT_16BITS_PER_SAMPLE;
	*channelFormat = SPPB_CHANNEL_FORMAT_STEREO;
}


void GMEPlayerInitialize(struct sppb_playback_plugin *plugin)
{
	plugin->create = GMEPlayerOpen;
	plugin->destroy = GMEPlayerClose;
	plugin->decode = GMEPlayerDecode;
	plugin->seek = GMEPlayerSeek;
	plugin->get_minimum_output_buffer_size = GMEPlayerGetMinimumOutputBufferSize;
	plugin->get_length_in_samples = GMEPlayerGetLengthInSamples;
	plugin->get_audio_format = GMEPlayerGetAudioFormat;
}