#include "local_file_plugin_api.h"
#include "gme/gme.h"

struct GMEPlayerContext {
	unsigned trackno;
	track_info_t track;
	Music_Emu *emu;
	unsigned length_in_samples;
};

void *GMEPlayerOpen(struct sppb_plugin_description *plugin, struct sppb_byte_input *input, int song_index);
void GMEPlayerClose(struct sppb_plugin_description *plugin, void *ctx);

spbool GMEPlayerDecode(
	struct sppb_plugin_description *plugin, void *ctx,
	spbyte *dest,
	size_t *destlen,
	spbool *final
);
spbool GMEPlayerSeek(struct sppb_plugin_description *plugin, void *ctx, unsigned sample);
size_t GMEPlayerGetMinimumOutputBufferSize(struct sppb_plugin_description *plugin, void *ctx);
unsigned int GMEPlayerGetLengthInSamples(struct sppb_plugin_description *plugin, void *ctx);

void GMEPlayerGetAudioFormat(
	struct sppb_plugin_description *plugin, void *ctx,
	unsigned int *samplerate,
	enum sppb_sound_format *format,
	enum sppb_channel_format *channelFormat
);

void GMEPlayerInitialize(struct sppb_playback_plugin *plugin);