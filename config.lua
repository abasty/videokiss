------------------------------------------------------------------------------
--    videokiss config file                                                  --
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
theme_file = home_dir .. "/Desktop"
movie_dir = home_dir .. "/Desktop"

-- Output formats
-- NOTE: xvid codec does square pixels while mpeg4 alone respects source pixel ratio. For a 4:3 video, use widht and height with width:size = 1.33
-- NOTE: This is not needed for the mpeg4 encoder that will preserve a video aspect ratio of 4:3 even with a size of 720:576 (1.25) 
-- NOTE: No special file characters in name and size. They are used to build the final file name.
-- NOTE: For the same video rate, decreasing size will increase quality.
formats = {
	{
		name = "Preview",
		consumer = "sdl",
		codecs = "profile=dv_pal",
		size = "720x576 (1.25)"
	},
	{
		name = "TVIX (XVID 3M)",
		consumer = "'avformat:$file' real_time=0 qscale=1",
 		codecs = "format=avi vcodec=xvid video_rc_min_rate=0 video_bit_rate=3000000 audio_bit_rate=128000 frequency=48000 acodec=mp2 progressive=1";
-- 		codecs = "format=avi b=3000K ab=128K frequency=48000 acodec=mp2 progressive=1";
		ext = "avi",
		size = "360x288 (1.25)"
	},
	{
		name = "Internet (MPEG-4 100K)",
		consumer = "'avformat:$file' real_time=0 qscale=1",
 		codecs = "format=avi video_rc_min_rate=0 video_bit_rate=100000 audio_bit_rate=64000 frequency=32000 acodec=mp2 progressive=1";
		ext = "avi",
		size = "240x180 (1.33)"
	},
	{
		name = "Internet (MPEG-4 150K)",
		consumer = "'avformat:$file' real_time=0 qscale=1",
 		codecs = "format=avi video_rc_min_rate=0 video_bit_rate=150000 audio_bit_rate=64000 frequency=32000 acodec=mp2 progressive=1";
		ext = "avi",
		size = "240x180 (1.33)"
	},
	{
		name = "Internet (MPEG-4 300K)",
		consumer = "'avformat:$file' real_time=0 qscale=1",
 		codecs = "format=avi video_rc_min_rate=0 video_bit_rate=300000 audio_bit_rate=64000 frequency=32000 acodec=mp2 progressive=1";
		ext = "avi",
		size = "300x225 (1.33)"
	},
	{
		name = "Internet (MPEG-4 450K)",
		consumer = "'avformat:$file' real_time=0 qscale=1",
 		codecs = "format=avi video_rc_min_rate=0 video_bit_rate=450000 audio_bit_rate=64000 frequency=32000 acodec=mp2 progressive=1";
		ext = "avi",
		size = "360x288 (1.25)"
	},
	{
		name = "RIP CD 4:3 (MPEG-4 800K) qscale=1",
		consumer = "'avformat:$file' real_time=0",
 		codecs = "format=avi video_rc_min_rate=0 video_bit_rate=800000 audio_bit_rate=128000 frequency=48000 acodec=mp2 progressive=1";
		ext = "avi",
		size = "360x288 (1.25)"
	},
	{
		name = "PC LQ (MPEG-4 3M)",
		consumer = "'avformat:$file' real_time=0 qscale=1",
 		codecs = "format=avi video_rc_min_rate=0 video_bit_rate=3000000 audio_bit_rate=128000 frequency=48000 acodec=mp2 progressive=1";
		ext = "avi",
		size = "720x576 (1.25)"
	},
	{
		name = "PC MQ (MPEG-4 5M)",
		consumer = "'avformat:$file' real_time=0 qscale=1",
		codecs = "format=avi video_rc_min_rate=0 video_bit_rate=5000000 audio_bit_rate=128000 frequency=48000 acodec=mp2 progressive=1";
		ext = "avi",
		size = "720x576 (1.25)"
	},
	{
		name = "PC HQ (MPEG-4 7M)",
		consumer = "'avformat:$file' real_time=0 qscale=1",
		codecs = "format=avi video_rc_min_rate=0 video_bit_rate=7000000 audio_bit_rate=128000 frequency=48000 acodec=mp2 progressive=1";
		ext = "avi",
		size = "720x576 (1.25)"
	},
	{
		name = "Video Editing (DV)",
		consumer = "'avformat:$file' real_time=0 qscale=1",
		codecs = "format=avi vcodec=dvvideo ildct=1 pix_fmt=yuv420p acodec=pcm_s16le profile=dv_pal",
		ext = "avi",
		size = "720x576 (1.25)"
	},
	
	
}

-- Output sizes
sizes = {
	"192x144 (1.33)",
	"240x180 (1.33)",
	"300x225 (1.33)",
	"360x270 (1.33)",
	"360x288 (1.25)",
	"480x360 (1.33)",
	"640x480 (1.33)",
	"720x576 (1.25)",
}

-- Movie making rules

clip_rule = {
	"$0",
	"-mix", "25", "-mixer", "luma", "-mixer", "mix:-1"
}

rules = {
	-- Movie files
	{ 
		pattern = "%.mpg$",
		value = clip_rule
	},

	{ 
		pattern = "%.wmv$", 
		value = clip_rule
	},

	{ 
		pattern = "%.avi$", 
		value = clip_rule
	},

	-- Title
	{
		pattern = "([^/]*) %-%- ([^/]*) %-%- ([^/]*)$", 
		value = {
			"colour:black", "out=99",
 			"-attach-cut", "watermark:+$2.txt", "composite.progressive=1", "producer.align=c", "producer.size=80", "composite.halign=c", "composite.geometry=0,10%:100%,100%", 
  			"-attach-cut", "watermark:+$3.txt", "producer.align=c", "producer.fgcolour=red", "producer.size=40", "composite.progressive=1", "composite.valign=b", "composite.halign=c", "composite.geometry=0,0:100%,90%", 
		},
	},
	
	-- Chapter
	{
		pattern = "([^/]*) %-%- ([^/]*)$", 
		value = {
			"colour:black", "out=99",
			"-attach", "watermark:+$2.txt", "composite.progressive=1", "producer.align=centre", "composite.valign=c", "composite.halign=c",
			"-mix", "25", "-mixer", "luma",
			"colour:black", "out=25",
			"-mix", "25", "-mixer", "luma"
		}
	}
}

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
		print ("Theme:", theme)
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

function runFile(file)
	local d = io.open(file, "r")
	if d then
		io.close(d)
		dofile(file)
		return 0
	else
		return 1
	end
end

function init()
	local defaultExist = runFile(home_dir .. "/.videokiss/default.lua")
	runFile(home_dir .. "/.videokiss/config.lua")
	default_rules = rules
	default_clip_rule = clip_rule
	return defaultExist
end

function loadDefaultRules()
	rules = default_rules
	clip_rule = default_clip_rule
end
