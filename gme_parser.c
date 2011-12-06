#include "gme_parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MIN(a, b) ( (a) < (b) ? (a) : (b) )

static const unsigned samplerate = 44100;
#define self ((struct GMEParserContext*)ctx)

unsigned lengthInSamplesOfTrack(const track_info_t *track)
{
	unsigned m = samplerate/1000; // convert millisecs to sample count
	if (track->length && track->length != -1) {
		return track->length*m;
	} else if (track->loop_length && track->loop_length != -1) {
		// Loop thrice
		return (track->intro_length + track->loop_length*3)*m;
	} else {
		// Play for two minutes
		return 2*60*1000*m;
	}
}


void *GMEParserOpen(struct sppb_plugin_description *plugin, struct sppb_byte_input *input, int song_index)
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
		fprintf(stderr, "GMEParserOpen(%s): failed to read input\n", input->uri);
		free(data);
		return spfalse;
	}

	struct GMEParserContext *ctx = calloc(sizeof(struct GMEParserContext), 1);

	Music_Emu *emu;
	gme_err_t err;
	err = gme_open_data(data, len, &emu, gme_info_only);
	if (err) {
		fprintf(stderr, "GMEParserOpen(%s): %s\n", input->uri, err);
		free(ctx);
		free(data);
		return spfalse;
	}

	free(data);

	self->track_count = gme_track_count(emu);

	self->trackno = song_index;
	err = gme_track_info(emu, &self->gmetrack, self->trackno);
	if (err) {
		fprintf(stderr, "GMEParserOpen(%s): %s\n", input->uri, err);
		free(ctx);
		return spfalse;
	}

	if (input->uri)
		self->uri = strdup(input->uri);
	else
		self->uri = strdup("<unknown>");

	return ctx;
}
void GMEParserClose(struct sppb_plugin_description *plugin, void *ctx)
{
	free(self->uri);
	free(ctx);
}

unsigned int GMEParserSongCount(struct sppb_plugin_description *plugin, void *ctx)
{
	return self->track_count;
}
enum sppb_channel_format GMEParserGetChannelFormat(struct sppb_plugin_description *plugin, void *ctx)
{
	return SPPB_CHANNEL_FORMAT_STEREO;
}
unsigned int GMEParserSampleRate(struct sppb_plugin_description *plugin, void *ctx)
{
	return samplerate;
}
unsigned int GMEParserLengthInSamples(struct sppb_plugin_description *plugin, void *ctx)
{
	return lengthInSamplesOfTrack(&self->gmetrack);
}

spbool GMEParserHasField(struct sppb_plugin_description *plugin, void *ctx, enum sppb_field_type frame)
{
	size_t length;
	spbool status = GMEParserReadField(plugin, ctx, frame, NULL, &length);
	return status && length > 0;
}
spbool GMEParserReadField(struct sppb_plugin_description *plugin, void *ctx, enum sppb_field_type type, char *dest, size_t *length)
{
	#define copy_setlen(attr) { if(dest) strncpy(dest, self->gmetrack.attr, *length); *length = MIN(strlen(self->gmetrack.attr), *length); return sptrue; }
	switch (type) {
		case SPPB_FIELD_TYPE_TITLE:
			if(strlen(self->gmetrack.song) > 0)
				copy_setlen(song)
			else {
				if(self->track_count==1) {
					// If single track in file, use file name as song
					const char *filename = strrchr(self->uri, '/')+1;
					if(dest) {
						strncpy(dest, filename, *length);
						strrchr(dest, '.')[0] = '\0';
						*length = strlen(dest);
					} else
						*length = strlen(filename);
				} else {
					if(dest)
						*length = snprintf(dest, *length, "Track %02d", self->trackno+1);
					else
						*length = 20;
				}

			}
			return sptrue;
		case SPPB_FIELD_TYPE_ARTIST: copy_setlen(author);
		case SPPB_FIELD_TYPE_ALBUM:
		case SPPB_FIELD_TYPE_ALBUM_ARTIST:;
			if(strlen(self->gmetrack.game) > 0)
				copy_setlen(game)
			else {
				if(self->track_count==1) {
					// If single track in file, use folder name as game
					if(dest) {
						strncpy(dest, self->uri, *length);
						strrchr(dest, '/')[0] = '\0';
						memmove(dest, strrchr(dest, '/')+1, strrchr(dest, '/') - dest-1);
					} else
						*length = strlen(self->uri);
				} else
					*length = 0;
			}
			return sptrue;
		case SPPB_FIELD_TYPE_COMMENT: copy_setlen(comment);
		case SPPB_FIELD_TYPE_COMPOSER: copy_setlen(dumper);
		case SPPB_FIELD_TYPE_COPYRIGHT: copy_setlen(copyright);
		case SPPB_FIELD_TYPE_PUBLISHER: copy_setlen(system);
		case SPPB_FIELD_TYPE_TRACK:
			if(dest)
				*length = snprintf(dest, *length, "%d", self->trackno+1);
			else
				*length = 20;
			return sptrue;
		default:
			return spfalse;
	}
}



void GMEParserInitialize(struct sppb_parser_plugin *plugin)
{
	plugin->create = GMEParserOpen;
	plugin->destroy = GMEParserClose;
	plugin->get_song_count = GMEParserSongCount;
	plugin->get_channel_format = GMEParserGetChannelFormat;
	plugin->get_sample_rate = GMEParserSampleRate;
	plugin->get_length_in_samples = GMEParserLengthInSamples;
	plugin->has_field = GMEParserHasField;
	plugin->read_field = GMEParserReadField;
}