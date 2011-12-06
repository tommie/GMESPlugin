#include "local_file_plugin_api.h"
#include "gme/gme.h"

struct GMEParserContext {
	int trackno;
	int track_count;
	track_info_t gmetrack;
	char *uri;
};

void *GMEParserOpen(struct sppb_plugin_description *plugin, struct sppb_byte_input *input, int song_index);
void GMEParserClose(struct sppb_plugin_description *plugin, void *ctx);

unsigned int GMEParserSongCount(struct sppb_plugin_description *plugin, void *ctx);
enum sppb_channel_format GMEParserGetChannelFormat(struct sppb_plugin_description *plugin, void *ctx);
unsigned int GMEParserSampleRate(struct sppb_plugin_description *plugin, void *ctx);
unsigned int GMEParserLengthInSamples(struct sppb_plugin_description *plugin, void *ctx);

spbool GMEParserHasField(struct sppb_plugin_description *plugin, void *ctx, enum sppb_field_type frame);
spbool GMEParserReadField(struct sppb_plugin_description *plugin, void *ctx, enum sppb_field_type frame, char *dest, size_t *length);

void GMEParserInitialize(struct sppb_parser_plugin *plugin);

unsigned lengthInSamplesOfTrack(const track_info_t *track);