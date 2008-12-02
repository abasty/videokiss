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
clips_dir = home_dir .. "/Documents/SVN/dev/autoprod/src/sample"

clips_dir = "/home/alain/Desktop/Documents/SVN/dev/autoprod/Rushes/Essai"
themes_dir = home_dir .. "/Desktop"
movie_dir = home_dir .. "/Desktop"

-- Output formats
formats = {
	{
		name = "Ecran (SDL)",
		consumer = "sdl",
		codecs = "profile=dv_pal"
	},
	{
		name = "XVid Basse Qualité",
		consumer = "'avformat:$file' real_time=0",
		codecs = "format=avi video_rc_min_rate=0 video_bit_rate=3000000 audio_bit_rate=128000 frequency=48000 vcodec=xvid acodec=mp2 progressive=1";
		ext = "avi"
	},
	{
		name = "XVid Moyenne Qualité",
		consumer = "'avformat:$file' real_time=0",
		codecs = "format=avi video_rc_min_rate=0 video_bit_rate=5000000 audio_bit_rate=128000 frequency=48000 vcodec=xvid acodec=mp2 progressive=1";
		ext = "avi"
	},
	{
		name = "XVid Haute Qualité",
		consumer = "'avformat:$file' real_time=0",
		codecs = "format=avi video_rc_min_rate=0 video_bit_rate=7000000 audio_bit_rate=128000 frequency=48000 vcodec=xvid acodec=mp2 progressive=1";
		ext = "avi"
	},
	{
		name = "DV Haute Qualité",
		consumer = "'avformat:$file' real_time=0",
		codecs = "format=avi vcodec=dvvideo ildct=1 pix_fmt=yuv420p acodec=pcm_s16le profile=dv_pal",
		ext = "avi"
	},
	
	
}

-- Output sizes
sizes = {
	"720x576",
	"360x288",
	"180x144"
}

-- rules
rules = {
	{ 
		-- Data file
		pattern = "(.*) %-%- .*%.mpg", 
		value = {
			"$0",
			"-mix", "25", "-mixer", "luma", "-mixer", "mix:-1"
		}
	},

	{
		-- Title 
		pattern = "(.*) %-%- (.*)", 
		value = {
			"colour:black", "out=24",
			"colour:black", "out=99",
			"-attach", "watermark:+$2.txt", "composite.progressive=1", "producer.align=centre", "composite.valign=c", "composite.halign=c",
			"-mix", "25", "-mixer", "luma",
			"colour:black", "out=25",
			"-mix", "25", "-mixer", "luma"
		}
	},
}

-- TODO
-- Add code to create ~/.autoprod and to store the default settings (directories, formats, sizes, rules) ?

function runRule(clip, rule, args)
	for i,command in ipairs(rule.value) do
		local line = string.gsub(command, "%$0", clip)
		line = string.gsub(line, "%$(%d)", function (s) return args[0 + s] end)
		table.insert(inigo, line)
	end
end

function montage(clips, theme)
	-- load the theme (a file with a "rules" var definition)
	if theme then
		print ("theme =", theme)
		dofile(theme)
	end
	
	inigo = { }
	
	-- foreach clip
	for i,clip in ipairs(clips) do
		-- find the right rule
		for j,rule in ipairs(rules) do
			capture = { string.match(clip, rule.pattern) }
			if #capture > 0 then
				-- If found: apply the rule
				runRule(clip, rule, capture)
				break
			end
		end
	end
	
	return inigo
end
