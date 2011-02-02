/*
 * melt.c -- MLT command line utility
 * Copyright (C) 2002-2008 Ushodaya Enterprises Limited
 * Author: Charles Yates <charles.yates@pandora.be>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <libgen.h>

#include <framework/mlt.h>

#ifdef __DARWIN__
#include <SDL.h>
#endif

#include "io.h"

static void transport_action( mlt_producer producer, char *value )
{
	mlt_properties properties = MLT_PRODUCER_PROPERTIES( producer );
	mlt_multitrack multitrack = mlt_properties_get_data( properties, "multitrack", NULL );
	mlt_consumer consumer = mlt_properties_get_data( properties, "transport_consumer", NULL );

	mlt_properties_set_int( properties, "stats_off", 1 );

	if ( strlen( value ) == 1 )
	{
		switch( value[ 0 ] )
		{
			case 'q':
				mlt_properties_set_int( properties, "done", 1 );
				break;
			case '0':
				mlt_producer_set_speed( producer, 1 );
				mlt_producer_seek( producer, 0 );
				break;
			case '1':
				mlt_producer_set_speed( producer, -10 );
				break;
			case '2':
				mlt_producer_set_speed( producer, -5 );
				break;
			case '3':
				mlt_producer_set_speed( producer, -2 );
				break;
			case '4':
				mlt_producer_set_speed( producer, -1 );
				break;
			case '5':
				mlt_producer_set_speed( producer, 0 );
				break;
			case '6':
			case ' ':
				mlt_producer_set_speed( producer, 1 );
				break;
			case '7':
				mlt_producer_set_speed( producer, 2 );
				break;
			case '8':
				mlt_producer_set_speed( producer, 5 );
				break;
			case '9':
				mlt_producer_set_speed( producer, 10 );
				break;
			case 'd':
				if ( multitrack != NULL )
				{
					int i = 0;
					mlt_position last = -1;
					fprintf( stderr, "\n" );
					for ( i = 0; 1; i ++ )
					{
						mlt_position time = mlt_multitrack_clip( multitrack, mlt_whence_relative_start, i );
						if ( time == last )
							break;
						last = time;
						fprintf( stderr, "%d: %d\n", i, (int)time );
					}
				}
				break;

			case 'g':
				if ( multitrack != NULL )
				{
					mlt_position time = mlt_multitrack_clip( multitrack, mlt_whence_relative_current, 0 );
					mlt_producer_seek( producer, time );
				}
				break;
			case 'H':
				if ( producer != NULL )
				{
					mlt_position position = mlt_producer_position( producer );
					mlt_producer_seek( producer, position - ( mlt_producer_get_fps( producer ) * 60 ) );
				}
				break;
			case 'h':
				if ( producer != NULL )
				{
					mlt_position position = mlt_producer_position( producer );
					mlt_producer_set_speed( producer, 0 );
					mlt_producer_seek( producer, position - 1 );
				}
				break;
			case 'j':
				if ( multitrack != NULL )
				{
					mlt_position time = mlt_multitrack_clip( multitrack, mlt_whence_relative_current, 1 );
					mlt_producer_seek( producer, time );
				}
				break;
			case 'k':
				if ( multitrack != NULL )
				{
					mlt_position time = mlt_multitrack_clip( multitrack, mlt_whence_relative_current, -1 );
					mlt_producer_seek( producer, time );
				}
				break;
			case 'l':
				if ( producer != NULL )
				{
					mlt_position position = mlt_producer_position( producer );
					if ( mlt_producer_get_speed( producer ) != 0 )
						mlt_producer_set_speed( producer, 0 );
					else
						mlt_producer_seek( producer, position + 1 );
				}
				break;
			case 'L':
				if ( producer != NULL )
				{
					mlt_position position = mlt_producer_position( producer );
					mlt_producer_seek( producer, position + ( mlt_producer_get_fps( producer ) * 60 ) );
				}
				break;
		}

		mlt_properties_set_int( MLT_CONSUMER_PROPERTIES( consumer ), "refresh", 1 );
	}

	mlt_properties_set_int( properties, "stats_off", 0 );
}

static mlt_consumer create_consumer( mlt_profile profile, char *id )
{
	char *arg = id != NULL ? strchr( id, ':' ) : NULL;
	if ( arg != NULL )
		*arg ++ = '\0';
	mlt_consumer consumer = mlt_factory_consumer( profile, id, arg );
	if ( consumer != NULL )
	{
		mlt_properties properties = MLT_CONSUMER_PROPERTIES( consumer );
		mlt_properties_set_data( properties, "transport_callback", transport_action, 0, NULL, NULL );
	}
	return consumer;
}

#ifdef __DARWIN__

static void event_handling( mlt_producer producer, mlt_consumer consumer )
{
	SDL_Event event;

	while ( SDL_PollEvent( &event ) )
	{
		switch( event.type )
		{
			case SDL_QUIT:
				mlt_properties_set_int( MLT_PRODUCER_PROPERTIES( consumer ), "done", 1 );
				break;

			case SDL_KEYDOWN:
				if ( event.key.keysym.unicode < 0x80 && event.key.keysym.unicode > 0 )
				{
					char keyboard[ 2 ] = { event.key.keysym.unicode, 0 };
					transport_action( producer, keyboard );
				}
				break;
		}
	}
}

#endif

static void transport( mlt_producer producer, mlt_consumer consumer )
{
	mlt_properties properties = MLT_PRODUCER_PROPERTIES( producer );
	int silent = mlt_properties_get_int( MLT_CONSUMER_PROPERTIES( consumer ), "silent" );
	int progress = mlt_properties_get_int( MLT_CONSUMER_PROPERTIES( consumer ), "progress" );
	struct timespec tm = { 0, 40000 };
	int total_length = mlt_producer_get_length( producer );
	int last_position = 0;

	if ( mlt_properties_get_int( properties, "done" ) == 0 && !mlt_consumer_is_stopped( consumer ) )
	{
		if ( !silent && !progress )
		{
			term_init( );

			fprintf( stderr, "+-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+\n" );
			fprintf( stderr, "|1=-10| |2= -5| |3= -2| |4= -1| |5=  0| |6=  1| |7=  2| |8=  5| |9= 10|\n" );
			fprintf( stderr, "+-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+\n" );

			fprintf( stderr, "+---------------------------------------------------------------------+\n" );
			fprintf( stderr, "|               H = back 1 minute,  L = forward 1 minute              |\n" );
			fprintf( stderr, "|                 h = previous frame,  l = next frame                 |\n" );
			fprintf( stderr, "|           g = start of clip, j = next clip, k = previous clip       |\n" );
			fprintf( stderr, "|                0 = restart, q = quit, space = play                  |\n" );
			fprintf( stderr, "+---------------------------------------------------------------------+\n" );
		}

		while( mlt_properties_get_int( properties, "done" ) == 0 && !mlt_consumer_is_stopped( consumer ) )
		{
			int value = ( silent || progress )? -1 : term_read( );

			if ( value != -1 )
			{
				char string[ 2 ] = { value, 0 };
				transport_action( producer, string );
			}

#ifdef __DARWIN__
			event_handling( producer, consumer );
#endif

			if ( !silent && mlt_properties_get_int( properties, "stats_off" ) == 0 )
			{
				if ( progress )
				{
					int current_position = mlt_producer_position( producer );
					if ( current_position > last_position )
					{
						fprintf( stderr, "Current Frame: %10d, percentage: %10d\r",
							current_position, 100 * current_position / total_length );
						last_position = current_position;
					}
				}
				else
				{
					fprintf( stderr, "Current Position: %10d\r", (int)mlt_producer_position( producer ) );
				}
			}

			if ( silent )
				nanosleep( &tm, NULL );
		}

		if ( !silent )
			fprintf( stderr, "\n" );
	}
}

static void query_metadata( mlt_repository repo, mlt_service_type type, const char *typestr, char *id )
{
	mlt_properties metadata = mlt_repository_metadata( repo, type, id );
	if ( metadata )
	{
		char *s = mlt_properties_serialise_yaml( metadata );
		fprintf( stderr, "%s", s );
		free( s );
	}
	else
	{
		fprintf( stderr, "# No metadata for %s \"%s\"\n", typestr, id );
	}
}

static void query_services( mlt_repository repo, mlt_service_type type )
{
	mlt_properties services = NULL;
	const char *typestr = NULL;
	switch ( type )
	{
		case consumer_type:
			services = mlt_repository_consumers( repo );
			typestr = "consumers";
			break;
		case filter_type:
			services = mlt_repository_filters( repo );
			typestr = "filters";
			break;
		case producer_type:
			services = mlt_repository_producers( repo );
			typestr = "producers";
			break;
		case transition_type:
			services = mlt_repository_transitions( repo );
			typestr = "transitions";
			break;
		default:
			return;
	}
	fprintf( stderr, "---\n%s:\n", typestr );
	if ( services )
	{
		int j;
		for ( j = 0; j < mlt_properties_count( services ); j++ )
			fprintf( stderr, "  - %s\n", mlt_properties_get_name( services, j ) );
	}
	fprintf( stderr, "...\n" );
}

int inigo( int argc, char **argv )
{
	int i;
	mlt_consumer consumer = NULL;
	mlt_producer melt = NULL;
	FILE *store = NULL;
	char *name = NULL;
	mlt_profile profile = NULL;
	int is_progress = 0;
	int is_silent = 0;

	// Construct the factory
	mlt_repository repo = mlt_factory_init( NULL );

	for ( i = 1; i < argc; i ++ )
	{
		// Check for serialisation switch
		if ( !strcmp( argv[ i ], "-serialise" ) )
		{
			name = argv[ ++ i ];
			if ( name != NULL && strstr( name, ".melt" ) )
				store = fopen( name, "w" );
			else
			{
				if ( name == NULL || name[0] == '-' )
					store = stdout;
				name = NULL;
			}
		}
		// Look for the profile option
		else if ( !strcmp( argv[ i ], "-profile" ) )
		{
			const char *pname = argv[ ++ i ];
			if ( pname && pname[0] != '-' )
				profile = mlt_profile_init( pname );
		}
		else if ( !strcmp( argv[ i ], "-progress" ) )
		{
			is_progress = 1;
		}
		// Look for the query option
		else if ( !strcmp( argv[ i ], "-query" ) )
		{
			const char *pname = argv[ ++ i ];
			if ( pname && pname[0] != '-' )
			{
				if ( !strcmp( pname, "consumers" ) || !strcmp( pname, "consumer" ) )
					query_services( repo, consumer_type );
				else if ( !strcmp( pname, "filters" ) || !strcmp( pname, "filter" ) )
					query_services( repo, filter_type );
				else if ( !strcmp( pname, "producers" ) || !strcmp( pname, "producer" ) )
					query_services( repo, producer_type );
				else if ( !strcmp( pname, "transitions" ) || !strcmp( pname, "transition" ) )
					query_services( repo, transition_type );
				
				else if ( !strncmp( pname, "consumer=", 9 ) )
					query_metadata( repo, consumer_type, "consumer", strchr( pname, '=' ) + 1 );
				else if ( !strncmp( pname, "filter=", 7 ) )
					query_metadata( repo, filter_type, "filter", strchr( pname, '=' ) + 1 );
				else if ( !strncmp( pname, "producer=", 9 ) )
					query_metadata( repo, producer_type, "producer", strchr( pname, '=' ) + 1 );
				else if ( !strncmp( pname, "transition=", 11 ) )
					query_metadata( repo, transition_type, "transition", strchr( pname, '=' ) + 1 );
				else
					goto query_all;
			}
			else
			{
query_all:
				query_services( repo, consumer_type );
				query_services( repo, filter_type );
				query_services( repo, producer_type );
				query_services( repo, transition_type );
				fprintf( stderr, "# You can query the metadata for a specific service using:\n"
					"# -query <type>=<identifer>\n"
					"# where <type> is one of: consumer, filter, producer, or transition.\n" );
			}
			goto exit_factory;
		}
		else if ( !strcmp( argv[ i ], "-silent" ) )
		{
			is_silent = 1;
		}
		else if ( !strcmp( argv[ i ], "-verbose" ) )
		{
			mlt_log_set_level( MLT_LOG_VERBOSE );
		}
		else if ( !strcmp( argv[ i ], "-version" ) || !strcmp( argv[ i ], "--version" ) )
		{
			fprintf( stderr, "MLT %s 0.5\n"
				"Copyright (C) 2002-2009 Ushodaya Enterprises Limited\n"
				"<http://www.mltframework.org/>\n"
				"This is free software; see the source for copying conditions.  There is NO\n"
				"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n",
				basename( argv[0] ) );
			goto exit_factory;
		}
		else if ( !strcmp( argv[ i ], "-debug" ) )
		{
			mlt_log_set_level( MLT_LOG_DEBUG );
		}
	}

	// Create profile if not set explicitly
	if ( profile == NULL )
		profile = mlt_profile_init( NULL );

	// Look for the consumer option
	for ( i = 1; i < argc; i ++ )
	{
		if ( !strcmp( argv[ i ], "-consumer" ) )
		{
			consumer = create_consumer( profile, argv[ ++ i ] );
			if ( consumer )
			{
				mlt_properties properties = MLT_CONSUMER_PROPERTIES( consumer );
				while ( argv[ i + 1 ] != NULL && strstr( argv[ i + 1 ], "=" ) )
					mlt_properties_parse( properties, argv[ ++ i ] );
			}
		}
	}

	// If we have no consumer, default to sdl
	if ( store == NULL && consumer == NULL )
		consumer = create_consumer( profile, NULL );

	// Get melt producer
	if ( argc > 1 )
		melt = mlt_factory_producer( profile, "melt", &argv[ 1 ] );

	// Set transport properties on consumer and produder
	if ( consumer != NULL && melt != NULL )
	{
		mlt_properties_set_data( MLT_CONSUMER_PROPERTIES( consumer ), "transport_producer", melt, 0, NULL, NULL );
		mlt_properties_set_data( MLT_PRODUCER_PROPERTIES( melt ), "transport_consumer", consumer, 0, NULL, NULL );
		if ( is_progress )
			mlt_properties_set_int(  MLT_CONSUMER_PROPERTIES( consumer ), "progress", is_progress );
		if ( is_silent )
			mlt_properties_set_int(  MLT_CONSUMER_PROPERTIES( consumer ), "silent", is_silent );
	}

	if ( argc > 1 && melt != NULL && mlt_producer_get_length( melt ) > 0 )
	{
		// Parse the arguments
		for ( i = 1; i < argc; i ++ )
		{
			if ( !strcmp( argv[ i ], "-serialise" ) )
			{
				if ( store != stdout )
					i ++;
			}
			else
			{
				if ( store != NULL )
					fprintf( store, "%s\n", argv[ i ] );

				i ++;

				while ( argv[ i ] != NULL && argv[ i ][ 0 ] != '-' )
				{
					if ( store != NULL )
						fprintf( store, "%s\n", argv[ i ] );
					i += 1;
				}

				i --;
			}
		}

		if ( consumer != NULL && store == NULL )
		{
			// Get melt's properties
			mlt_properties melt_props = MLT_PRODUCER_PROPERTIES( melt );
	
			// Get the last group
			mlt_properties group = mlt_properties_get_data( melt_props, "group", 0 );
	
			// Apply group settings
			mlt_properties properties = MLT_CONSUMER_PROPERTIES( consumer );
			mlt_properties_inherit( properties, group );

			// Connect consumer to melt
			mlt_consumer_connect( consumer, MLT_PRODUCER_SERVICE( melt ) );

			// Start the consumer
			mlt_consumer_start( consumer );

			// Transport functionality
			transport( melt, consumer );

			// Stop the consumer
			mlt_consumer_stop( consumer );
		}
		else if ( store != NULL && store != stdout && name != NULL )
		{
			fprintf( stderr, "Project saved as %s.\n", name );
			fclose( store );
		}
	}
	else
	{
		fprintf( stderr,
"Usage: %s [options] [producer [name=value]* ]+\n"
"Options:\n"
"  -attach filter[:arg] [name=value]*       Attach a filter to the output\n"
"  -attach-cut filter[:arg] [name=value]*   Attach a filter to a cut\n"
"  -attach-track filter[:arg] [name=value]* Attach a filter to a track\n"
"  -attach-clip filter[:arg] [name=value]*  Attach a filter to a producer\n"
"  -audio-track | -hide-video               Add an audio-only track\n"
"  -blank frames                            Add blank silence to a track\n"
"  -consumer id[:arg] [name=value]*         Set the consumer (sink)\n"
"  -debug                                   Set the logging level to debug\n"
"  -filter filter[:arg] [name=value]*       Add a filter to the current track\n"
"  -group [name=value]*                     Apply properties repeatedly\n"
"  -help                                    Show this message\n"
"  -join clips                              Join multiple clips into one cut\n"
"  -mix length                              Add a mix between the last two cuts\n"
"  -mixer transition                        Add a transition to the mix\n"
"  -null-track | -hide-track                Add a hidden track\n"
"  -profile name                            Set the processing settings\n"
"  -progress                                Display progress along with position\n"
"  -remove                                  Remove the most recent cut\n"
"  -repeat times                            Repeat the last cut\n"
"  -query                                   List all of the registered services\n"
"  -query \"consumers\" | \"consumer\"=id       List consumers or show info about one\n"
"  -query \"filters\" | \"filter\"=id           List filters or show info about one\n"
"  -query \"producers\" | \"producer\"=id       List producers or show info about one\n"
"  -query \"transitions\" | \"transition\"=id   List transitions, show info about one\n"
"  -serialise [filename]                    Write the commands to a text file\n"
"  -silent                                  Do not display position/transport\n"
"  -split relative-frame                    Split the last cut into two cuts\n"
"  -swap                                    Rearrange the last two cuts\n"
"  -track                                   Add a track\n"
"  -transition id[:arg] [name=value]*       Add a transition\n"
"  -verbose                                 Set the logging level to verbose\n"
"  -version                                 Show the version and copyright\n"
"  -video-track | -hide-audio               Add a video-only track\n"
"For more help: <http://www.mltframework.org/>\n",
		basename( argv[0] ) );
	}

	// Close the producer
	if ( melt != NULL )
		mlt_producer_close( melt );

	// Close the consumer
	if ( consumer != NULL )
		mlt_consumer_close( consumer );

	// Close the factory
	mlt_profile_close( profile );

exit_factory:
		
	mlt_factory_close( );

	return 0;
}
