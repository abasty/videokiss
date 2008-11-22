------------------------------------------------------------------------------
--    autoprod config file                                                  --
--                                                                          --
--    Copyright (C) 2008 by Alain Basty                                     --
--    alain.basty@free.fr                                                   --
--                                                                          --
--    This program is free software; you can redistribute it and-or modify  --
--    it under the terms of the GNU General Public License as published by  --
--    the Free Software Foundation; either version 2 of the License, or     --
--    (at your option) any later version.                                   --
--                                                                          --
--    This program is distributed in the hope that it will be useful,       --
--    but WITHOUT ANY WARRANTY; without even the implied warranty of        --
--    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         --
--    GNU General Public License for more details.                          --
--                                                                          --
--    You should have received a copy of the GNU General Public License     --
--    along with this program; if not, write to the                         --
--    Free Software Foundation, Inc.,                                       --
--    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             --
------------------------------------------------------------------------------

-- Path to home directory
home_dir = os.getenv("HOME")

-- several paths relative to home directory
clips_dir = home_dir .. "/Desktop"
themes_dir = home_dir .. "/Desktop"
movie_dir = home_dir .. "/Desktop"

-- Output formats
formats = {
	{
		name = "Ecran (SDL)"
	},
	{
		name = "XVid Basse Résolution",
		consumer = "avformat:$clips.avi real_time=0 size=$size",
		codecs = "format=avi video_rc_min_rate=0 video_bit_rate=3000000 audio_bit_rate=64000 frequency=32000 vcodec=xvid progressive=1",
	},
}

-- Output sizes
sizes = {
	"720x576",
	"360x288"
}

-- rules
rules = {
	{ 
		-- Data file
		pattern = "(\\d+) -- (.*)\.(.*)", 
		value = {
			"\"$0\"",
			"-mix 25 -mixer luma -mixer mix:-1"
		}
	},

	{
		-- Title 
		pattern = "(\\d+) -- (.*)", 
		value = {
			"colour:black out=24",
			"colour:black out=99",
			"-attach watermark:+\"$2.txt\" composite.progressive=1 producer.align=centre composite.valign=c composite.halign=c",
			"-mix 25 -mixer luma",
			"colour:black out=25",
			"-mix 25 -mixer luma"
		}
	},
}
